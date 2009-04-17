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

  cameraPos[0] = 50; cameraPos[1] = 100; cameraPos[2] = 0;
  cameraLookAt[0] = 0; cameraLookAt[1] = 20; cameraLookAt[2] = 100;
  cameraRollAngle = 0;

  focalLength = 100/10;
  focusPoint = 100;
  zBufferMaxDepth = 64;
  saveZbuffer = 1;

  samplesPerPixel = 2;
  secondaryRaysDepth = 6;
  shadows = 1;

// Hardcoded Scene

  sceneMaterials.push_back(new Material(0.1, 0.1, 0.1, 1.0, 0.0, 0.0, 0.1, 0.9, 64, 1.0, 1.0));
  sceneMaterials.push_back(new Material(0, 0.0, 0.6, 0.1, 0.8, 0.0, 0.2, 0.5, 32, 1.0, 0.95));
  sceneMaterials.push_back(new Material(0.9, 0.9, 0.9, 1.0, 0.0, 0.0, 0.8, 1.0, 128, 0.66, 1.0));
  sceneMaterials.push_back(new Material(0.0, 0.8, 0.0, 0.1, 0.9, 0.0, 0.8, 0.6, 128, 0.66, 1.5));
  sceneMaterials.push_back(new Material(0.4, 0.36, 0.25, 1.0, 0.0, 0.0, 0.5, 0.1, 128, 0.66, 1.5));

  MaterialList::iterator mi = sceneMaterials.begin();

  sceneLights.push_back(new AreaLight(6, 6, 3, 3, 1, -17.3, 25, 90, 0, -1, 0, 0.99, 0.77, 0.33, 1, 1));
  sceneLights.push_back(new AreaLight(6, 6, 3, 3, 1, 17.3, 25, 90, 0, -1, 0, 0.95, 1, 0.33, 1, 1));
  sceneLights.push_back(new AreaLight(6, 6, 3, 3, 1, 0, 25, 110, 0, -1, 0, 0.2, 0.77, 1, 1, 1));

  sceneObjects.push_back(new Sphere(-8.33, 5, 100, 5, *mi));
  mi++;
  sceneObjects.push_back(new Sphere(8.33, 5, 100, 5, *mi));
  mi++;
  sceneObjects.push_back(new Sphere(0, 5, 110, 5, *mi));
  mi++;
  sceneObjects.push_back(new Sphere(0, 15, 105, 5, *mi));
  mi++;
  sceneObjects.push_back(new Plane(0, -10, 0, 0, 1, 0, *mi));
  sceneObjects.push_back(new Plane(0, 0, 135, 0, 0, -1, *mi));
  sceneObjects.push_back(new Plane(-30, 0, 0, 1, 0, 0, *mi));

}

void Scene::deleteRayArray(VisionRay **rays, int nRays) {
  for(int i = 0; i < nRays; i++)
    delete rays[i];
}

void Scene::outputImage(double* image,
    int image_w, int image_h,
    int imageFlag, char* imageName) {

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
  int i, nSecRays;
  nSecRays = 0;
  for(i = 0; i < nRays; i++) {
    intersectRay(rays[i]);
    if(INF_LIMIT != rays[i]->intersectionT) {
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

  omp_set_num_threads(1);

  double lookAtDir[3];
  blasSubstract(cameraLookAt, cameraPos, lookAtDir);
  blasNormalize(lookAtDir);
  blasBuildRotMatDir(lookAtDir, rayTransformationMat);

  renderedImage = new double[res*3];
  zBuffer = new double[res];

  // Initialize Rendered Image Array
  for(register int i = 0; i < res*3; i++)
    renderedImage[i] = 0.0;

  // Initialize zBufferArray
  for(register int i = 0; i < res; i++)
    zBuffer[i] = -INF_LIMIT;

  clock_t begin = clock();

  // Projection Plane Pixel Grid
  double aspect = (image_height*1.0) / image_width;
  double delta = 3.6/image_width;

  // Pixel
  int sqrSamples = samplesPerPixel*samplesPerPixel;
  int rowLength = image_width*sqrSamples;
  double sDelta = delta/(samplesPerPixel + 1);
  double sOffset = delta/2;

  // Rows
  VisionRay **rays;
  int x, y;

  #pragma omp parallel for schedule(runtime) default(shared) \
  private(x, y, rays)
  for(y = 0; y < image_height; y++) {
    std::cout << "                                                     " <<
    "\rRendering: " << y*100/image_height << "% completed...";

    int k, nRays, nSecRays, currDepth, zbufferPixel, imagePixel;
    nRays = rowLength;
    nSecRays = 0;
    currDepth = 0;
    k = 0;

    rays = new VisionRay *[nRays*sqrSamples];

    double xDelta = -1.8;
    double yDelta = xDelta*aspect + delta*y;
    double xsDelta, ysDelta;
    double lookAtRay[3];

    zbufferPixel = y*image_width;
    imagePixel = zbufferPixel*3;

    // Row Columns
    for(x = 0; x < image_width; x++) {
      // Multisampling
      if(samplesPerPixel > 1) {
        ysDelta = sDelta - sOffset;
        for(int i = 0; i < samplesPerPixel; i++) {
          xsDelta = sDelta - sOffset;
          for(int j = 0; j < samplesPerPixel; j++) {
            rays[k] = new VisionRay(
                renderedImage+imagePixel, zBuffer+zbufferPixel, cameraPos);
            lookAtRay[0] = xDelta + xsDelta;
            lookAtRay[1] = yDelta + ysDelta;
            lookAtRay[2] = focalLength;
            blasVecMatrixProd(lookAtRay, rayTransformationMat, rays[k]->direction);
            blasNormalize(rays[k]->direction);
            k++;
            xsDelta += sDelta;
          }
          ysDelta += sDelta;
        }
      }
      // No Multisampling
      else {
        rays[k] = new VisionRay(
            renderedImage+imagePixel, zBuffer+zbufferPixel, cameraPos);
        lookAtRay[0] = xDelta;
        lookAtRay[1] = yDelta;
        lookAtRay[2] = focalLength;
        blasVecMatrixProd(lookAtRay, rayTransformationMat, rays[k]->direction);
        blasNormalize(rays[k]->direction);
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
      deleteRayArray(rays, image_width*sqrSamples);
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
      deleteRayArray(rays, image_width*sqrSamples);
      delete [] rays;
    }
    yDelta += delta;
  }
  clock_t end = clock();
  std::cout << "\rDone! Render Time: ~" << diffclock(end, begin) << " seconds." <<
  std::endl << "Writing output...\r";

  // Saving Rendered Image
  outputImage(renderedImage, image_width, image_height, 1, "image.png");

  // Saving Render ZBuffer Image
  if(saveZbuffer) {
    for(register int i = 0; i < res; i++) {
      if(zBuffer[i] != -std::numeric_limits<float>::infinity()) {
        zBuffer[i] = -fabs(focusPoint - zBuffer[i]);
        zBuffer[i] /= zBufferMaxDepth;
        zBuffer[i] += 1.0;
      }
    }
    outputImage(zBuffer, image_width, image_height, 0, "zbuffer.png");
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
    r->sumTs = INF_LIMIT;
    r->intersectionT = INF_LIMIT;
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

    int lSamples;
    double samplePos[3] = {0.0};
    Light *lightSource;
    LightList::iterator i;

    for(i = sceneLights.begin(); i != sceneLights.end(); ++i) {

      int currSample = 0;
      lightSource = *i;
      lSamples = lightSource->getSamples();

      while (currSample < lSamples) {
        lightSource->getPosI(currSample, samplePos);
        blasSubstract(samplePos, intersectionPoint, lVec);
        lDistance = blasNrm2(lVec);
        blasNormalize(lVec);

        ShadowRay *shadowRay = NULL;
        double shadowLight = 1.0;
  //      double shadowColors[3] = {1.0};

        if(shadows && m->opacy != 0) {
          shadowRay = new ShadowRay(intersectionPoint, lVec);
          double rayLight = 0;

          do {
            shadowRay->intersectionT = INF_LIMIT;
            rayLight = intersectRay(shadowRay);
            if(shadowRay->sumTs < lDistance) {
              shadowLight -= rayLight;
              shadowRay->nextStep();
            }
            else break;
            }
          while(shadowRay->intersectionT != INF_LIMIT
              && shadowLight > 0);
        }

        lDistance += r->sumTs;

        double luminosity = lightSource->intensity*LIGHT_SCALE*shadowLight;
        luminosity /= lDistance*lDistance*lightSource->damping;

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

        r->pixel[0] += lightSource->color[0] * r->weight[0] *
            (pixelBaseColor[0] * (phongD + m->ambient) + phongS);
        r->pixel[1] += lightSource->color[1] * r->weight[1] *
            (pixelBaseColor[1] * (phongD + m->ambient) + phongS);
        r->pixel[2] += lightSource->color[2] * r->weight[2] *
            (pixelBaseColor[2] * (phongD + m->ambient) + phongS);

        currSample++;
        if(shadows) delete shadowRay;
      }
    }
    return secRays;
  }
  else {
    return 0;
  }
}

double Scene::diffclock(clock_t clock1, clock_t clock2) {
  double diffticks = clock1-clock2;
  double diffms=diffticks/CLOCKS_PER_SEC;
  return diffms;
}
