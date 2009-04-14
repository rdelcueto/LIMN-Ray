/*
 * File: scene.h
 * Author: Rodrigo González del Cueto
 */

/*
 * Copyright (C) 2009 Rodrigo González del Cueto
 *
 * This file is part of Limn-Ray.
 * Limn-Ray is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * Limn-Ray is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with Limn-Ray.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "scene.h"

Scene::Scene() {
  image_width = 1152;
  image_height = 768;
  res = image_height*image_width;

  cameraPos[0] = 0; cameraPos[1] = 0; cameraPos[2] = 0;
  cameraLookAt[0] = 0; cameraLookAt[1] = 0; cameraLookAt[2] = 1;
  cameraRollAngle = 0;

  focalLength = 30;
  focusPoint = 100;
  zBufferMaxDepth = 128;
  saveZbuffer = 1;

  samplesPerPixel = 1;
  secondaryRaysDepth = 1;
  shadows = 1;

// Hardcoded Scene

  sceneLights.push_back(new Omnidirectional(0, 100, 100, 1, 1, 1, 10, 1));
//  sceneLights.push_back(new Omnidirectional(-7.07, 75, 100, 1, 0.6, 0.6, 10, 2));
//  sceneLights.push_back(new Omnidirectional(7.07, 75, 100, 0.6, 0.6, 1, 10, 2));
//  sceneLights.push_back(new Omnidirectional(0, 75, 107.07, 0.6, 1, 0.6, 10, 2));

//  sceneLights.push_back(new AreaLight(10, 10, 4, 4, 1, 0, 0, 100, 1, 1, 1, 5, 1));

  sceneMaterials.push_back(new Material(1.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.33, 1, 8, 1.0, 1.0));
  sceneMaterials.push_back(new Material(0.0, 0.0, 1.0, 1.0, 0.0, 0.0, 0.33, 1, 8, 1.0, 1.0));
  sceneMaterials.push_back(new Material(0.0, 1.0, 0.0, 1.0, 0.0, 0.0, 0.33, 1, 8, 1.0, 1.0));
  sceneMaterials.push_back(new Material(1.0, 0.0, 0.0, 0.0, 0.2, 0, 0, 0.666, 16, 0.5, 1.05));
  sceneMaterials.push_back(new Material(0.333, 0.333, 0.333, 1.0, 0.0, 0.0, 0.25, 0.5, 16, 0.666, 1.0));

  MaterialList::iterator mi = sceneMaterials.begin();

//  sceneObjects.push_back(new Sphere(0, 0, 50, 10, *mi));
  sceneObjects.push_back(new Sphere(-8.66, 8.66, 100, 6, *mi));
  mi++;
  sceneObjects.push_back(new Sphere(8.66, 8.66, 100, 6, *mi));
  mi++;
  sceneObjects.push_back(new Sphere(0, -5, 100, 6, *mi));
  mi++;
//  sceneObjects.push_back(new Sphere(0, 5, 100, 25, *mi));
  mi++;
  sceneObjects.push_back(new Plane(0, -25, 0, 0, 1, 0, *mi));

}

void Scene::deleteRayArray(VisionRay **rays, int nRays) {
  for(int i = 0; i < nRays; i++)
    delete rays[i];
}

void Scene::outputImage(double* image,
    int image_w, int image_h, int imageFlag, char* imageName) {

  int s = samplesPerPixel*samplesPerPixel;
  int k = 0;
  pngwriter image_out(image_w, image_h, 0, imageName);

  if(imageFlag) {
    for(register int y = 1; y <= image_h; y++)
      for(register int x = 1; x <= image_w; x++) {
        if(samplesPerPixel > 1) {
          image_out.plot(x, y,
              image[k]/s, image[k+1]/s, image[k+2]/s);
          k+=3;
        }
        else {
          image_out.plot(x, y, image[k], image[k+1], image[k+2]);
          k+=3;
        }
      }
  }
  else {
    for(register int y = 1; y <= image_h; y++) {
      for(register int x = 1; x <= image_w; x++) {
        image_out.plot(x, y, image[k], image[k], image[k]);
        k++;
      }
    }
  }

  image_out.close();
  image_out.clear();
}

int Scene::raytrace(VisionRay **rays, int nRays) {

  double *z;
  int nSecRays = 0;

  for(int i = 0; i < nRays; i++) {
    intersectRay(rays[i]);
    if(std::numeric_limits<float>::infinity() != rays[i]->intersectionT) {
      if(saveZbuffer && rays[i]->zBufferPixel != NULL) {
        z = rays[i]->zBufferPixel;
        z[0] = rays[i]->intersectionT;
      }
      nSecRays += shadeRayIntersection(rays[i]);
    }
  }
  return nSecRays;
}

void Scene::buildSecondaryRays(VisionRay **oldRays, VisionRay **newRays, int nRays) {
  int j = 0;
  for(int i = 0; i < nRays; i++) {
    if(oldRays[i]->type != 0) {
      if(oldRays[i]->type % 2 != 0) {
        newRays[j] = new VisionRay(oldRays[i], 1);
        j++;
      }
      if(oldRays[i]->type > 1) {
        newRays[j] = new VisionRay(oldRays[i], 2);
        j++;
      }
    }
  }
}

void Scene::render() {

  getCameraMatrix(cameraPos, cameraLookAt, rayTransformationMat);

  renderedImage = new double[res*3];
  zBuffer = new double[res];

  // Initialize Rendered Image Array
  for(register int i = 0; i < res*3; i++)
    renderedImage[i] = 0.0;

  // Initialize zBufferArray
  for(register int i = 0; i < res; i++)
    zBuffer[i] = -std::numeric_limits<double>::infinity();

  clock_t begin = clock();
  int w, h, imagePixel, zbufferPixel, k;

  w = image_width;
  h = image_height;

  // Projection Plane Pixel Grid
  double aspect = (h*1.0) / w;
  double delta = 3.6/w;
  double xDelta = -1.8;
  double yDelta = -1.8*aspect;

  // Pixel
  int sqrSamples = samplesPerPixel*samplesPerPixel;
  double sDelta = delta/(samplesPerPixel + 1);
  double sOffset = delta/2;
  double xsDelta;
  double ysDelta;

  double pos[3];
  pos[0] = pos[1] = 0.0; pos[2] = -focalLength/10;

  int nRays = w*sqrSamples;
  int nSecRays = 0;
  int currDepth = 0;
  VisionRay **rays;

  // Rows
  for(int y = 0; y < h; y++) {
    k = 0;
    rays = new VisionRay *[nRays*sqrSamples];
    xDelta = -1.8;
    zbufferPixel = y*w;
    imagePixel = zbufferPixel*3;

    // Row Columns
    for(int x = 0; x < w; x++) {
      // Multisampling
      if(samplesPerPixel > 1) {
        ysDelta = sDelta - sOffset;
        for(int i = 0; i < samplesPerPixel; i++) {
          xsDelta = sDelta - sOffset;
          for(int j = 0; j < samplesPerPixel; j++) {
            rays[k] = new VisionRay(
                renderedImage+imagePixel, zBuffer+zbufferPixel, pos);
            rays[k]->setLookAt(xDelta + xsDelta, yDelta + ysDelta, 0);
            rotateRay(rays[k], rayTransformationMat);
            blasAdd(rays[k]->position, cameraPos, rays[k]->position);
            k++;
            xsDelta += sDelta;
          }
          ysDelta += sDelta;
        }
      }
      // No Multisampling
      else {
        rays[k] = new VisionRay(
            renderedImage+imagePixel, zBuffer+zbufferPixel, pos);
        rays[k]->setLookAt(xDelta, yDelta, 0);
        rotateRay(rays[k], rayTransformationMat);
        blasAdd(rays[k]->position, cameraPos, rays[k]->position);
        k++;
      }
      zbufferPixel++;
      imagePixel += 3;
      xDelta += delta;
    }
    // Render Primary Rays
    nSecRays = raytrace(rays, nRays);
    // Secondary Rays
    if(secondaryRaysDepth > 0) {
      currDepth = 0;
      VisionRay **secRays = new VisionRay *[nSecRays];
      buildSecondaryRays(rays, secRays, nRays);

      // Mem Clean: Primary Rays
      deleteRayArray(rays, w*sqrSamples);
      delete [] rays;

      while(currDepth < secondaryRaysDepth) {
        nRays = nSecRays;
        nSecRays = raytrace(secRays, nRays);
        if(nSecRays > 0) {
          rays = secRays;
          secRays = new VisionRay *[nSecRays];
          buildSecondaryRays(rays, secRays, nRays);

          // Mem Clean: Old Secondary Rays
          deleteRayArray(rays, nRays);
          delete [] rays;

          currDepth++;
        }
        else {
          // Mem Clean: Residual Rays
          deleteRayArray(secRays, nRays);
          delete [] secRays;

          currDepth = secondaryRaysDepth;
        }
      }
    }
    else {
      deleteRayArray(rays, w*sqrSamples);
      delete [] rays;
    }
    nRays = w*sqrSamples;
    yDelta += delta;
  }
  clock_t end = clock();
  std::cout << "Done! Render Time: ~" << diffclock(end, begin) << " seconds." <<
  std::endl << "Writing output...\n";

  // Saving Rendered Image
  outputImage(renderedImage, w, h, 1, "image.png");

  // Saving Render ZBuffer Image
  if(saveZbuffer) {
    for(register int i = 0; i < res; i++) {
      if(zBuffer[i] != -std::numeric_limits<float>::infinity()) {
        zBuffer[i] = -fabs(focusPoint - zBuffer[i]);
        zBuffer[i] /= zBufferMaxDepth;
        zBuffer[i] += 1.0;
      }
    }
    outputImage(zBuffer, w, h, 0, "zbuffer.png");
  }

  delete [] renderedImage;
  delete [] zBuffer;

  sceneLights.clear();
  sceneMaterials.clear();
  sceneObjects.clear();

  std::cout << "Done! Goodbye!" << std::endl;
}

double Scene::intersectRay(Ray *r) {

  // Intersection Variables
  double minT, currT;
  Primitive *p, *pMin;
  PrimitiveList::iterator i;

  minT = r->intersectionT;
  p = pMin = NULL;

  // Find closest Intersection
  for(i = sceneObjects.begin(); i != sceneObjects.end(); ++i) {
    p = *i;
    currT = p->intersect(r);
    if(currT < minT && currT > INTERSECT_EPSILON) {
      minT = currT;
      pMin = p;
    }
  }

  // If intersects => update ray intersection data
  if(minT < r->intersectionT) {
    r->intersectionT = minT;
    r->sumTs += minT;
    r->getPosAtT(minT, r->intersectionPoint);
    pMin->normalAtP(r->intersectionPoint, r->intersectionNormal);
    r->intersectionMaterial = pMin->material;
    return pMin->material->opacy;
  }
  else {
    r->sumTs = std::numeric_limits<double>::infinity();
    r->intersectionT = std::numeric_limits<double>::infinity();
    return 0.0;
  }
}

int Scene::shadeRayIntersection(VisionRay *r) {
  if(r->getWeight() > COLOR_EPS) {

    // Shading Variables
    int secRays = 0;
    double lDistance, phongD, phongS;
    Material *m = r->intersectionMaterial;

    if(m->opacy < 1.0 || m->filter < 1.0) {
      r->type++;
      secRays++;
    }
    if(m->reflection > 0.0) {
      r->type+=2;
      secRays++;
    }

    double intersectionPoint[3];
    blasCopy(r->intersectionPoint, intersectionPoint);
    double intersectionNormal[3];
    blasCopy(r->intersectionNormal, intersectionNormal);
    // View = Ray Pos - Intersection
    double v[3];
    blasSubstract(r->position, intersectionPoint, v);
    blasNormalize(v);

    // Light Unit Vector from Intersection Point
    double lVec[3];
    // h = (lVec + View)/2
    double h[3];
    // |d| = (hdotn)n-h Specular Phong Reformulation
    double d[3];

    Light *lightSample;
    LightList::iterator i;

    for(i = sceneLights.begin(); i != sceneLights.end(); ++i) {
      lightSample = *i;

      blasSubstract(lightSample->pos, intersectionPoint, lVec);
      lDistance = blasNrm2(lVec);
      blasNormalize(lVec);

      ShadowRay *shadowRay = NULL;
      double shadowLight = 1.0;
//      double shadowColors[3] = {1.0};

      if(shadows && m->opacy != 0) {
        shadowRay = new ShadowRay(intersectionPoint, lVec);
        double rayLight = 0;

        do {
          shadowRay->intersectionT = std::numeric_limits<double>::infinity();
          rayLight = intersectRay(shadowRay);
          if(shadowRay->sumTs < lDistance) {
            shadowLight -= rayLight;
            shadowRay->nextStep();
          }
          else break;
          }
        while(shadowRay->intersectionT != std::numeric_limits<double>::infinity()
            && shadowLight > 0);
      }

      lDistance += r->sumTs;

      double luminosity = lightSample->intensity*LIGHT_SCALE*shadowLight;
      luminosity /= lDistance*lDistance*lightSample->damping;

      // Diffuse
      if(m->diffuse != 0)
        phongD = blasDot(lVec, intersectionNormal)*m->diffuse*luminosity;
      else phongD = 0;

      // Specular
      if(m->specular != 0) {
        blasAdd(lVec, v, h); // LightPos + EyePos = Halfway = h
        blasScale(h, 0.5, h); // h = Halfway / 2
        blasCopy(intersectionPoint, d);
        blasScale(d, blasDot(h, d), d); // (NdotNormAtP)NormAtP
        blasSubstract(d, h, d);
        phongS = (m->specular_Hardness/2)*blasDot(d, d);

        if (phongS > 1) phongS = 0;
        else {
          phongS = (1-phongS)/2;
          phongS *= phongS;
        }
      }
      else phongS = 0;

      if(phongD < 0) phongD = 0;
      if(phongS < 0) phongS = 0;

      double pixelBaseColor[3];

      pixelBaseColor[0] = m->color[0] * m->opacy;
      pixelBaseColor[1] = m->color[1] * m->opacy;
      pixelBaseColor[2] = m->color[2] * m->opacy;

      r->pixel[0] += lightSample->color[0] * r->weight[0] *
          (pixelBaseColor[0] * (phongD + m->ambient) + phongS);
      r->pixel[1] += lightSample->color[1] * r->weight[1] *
          (pixelBaseColor[1] * (phongD + m->ambient) + phongS);
      r->pixel[2] += lightSample->color[2] * r->weight[2] *
          (pixelBaseColor[2] * (phongD + m->ambient) + phongS);

      if(shadows) delete shadowRay;
    }
    return secRays;
  }
  else {
    return 0;
  }
}

void Scene::rotateRay(Ray *r, double *mat) {
  blasVecMatrixProd(r->direction, mat, r->direction);
  blasNormalize(r->direction);
}

void Scene::getCameraMatrix(double *pos, double *dir, double *mat) {
  blasBuildRotMat(pos, dir, mat);
}

double Scene::diffclock(clock_t clock1, clock_t clock2) {
    double diffticks = clock1-clock2;
    double diffms=diffticks/CLOCKS_PER_SEC;
    return diffms;
}
