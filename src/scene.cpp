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

  image_width = 1;
  image_height = 1;
  res = image_height*image_width;

  cameraPos[0] = -0; cameraPos[1] = 0; cameraPos[2] = 0;
  cameraLookAt[0] = 0; cameraLookAt[1] = 0; cameraLookAt[2] = 1;
  cameraRollAngle = 0;

  focalLength = 50/10;
  focusDistance = 50;
  zBufferMaxDepth = 64;
  saveZBuffer = 1;

  samplesPerPixel = 1;
  secondaryRaysDepth = 1;
  shadows = 1;

}

void Scene::demoScene() {
  image_width = 720;
  image_height = 480;

  cameraPos[0] = -50; cameraPos[1] = 25; cameraPos[2] = -25;
  cameraLookAt[0] = 0; cameraLookAt[1] = 8; cameraLookAt[2] = 50;
  cameraRollAngle = 0;

  focalLength = 50/10;
  focusDistance = 80;
  zBufferMaxDepth = 24;
  saveZBuffer = 1;

  samplesPerPixel = 1;
  secondaryRaysDepth = 6;
  shadows = 1;

// Hardcoded Demo Scene

  sceneMaterials.push_back(new Material(0.94, 0.94, 0.94, 0.9, 0.0, 0.0, 0.8, 1.0, 64, 0.66, 1.0));
  sceneMaterials.push_back(new Material(0.0, 0.8, 1.0, 1.0, 1.0, 0.0, 0.1, 0.6, 64, 1.0, 1.0));
  sceneMaterials.push_back(new Material(0.33, 0.9, 0.98, 0.1, 0.3, 0.0, 0.1, 1.0, 128, 0.1, 1.05));
  sceneMaterials.push_back(new Material(0.4, 0.36, 0.25, 1.0, 0.0, 0.0, 0.5, 0.1, 64, 0.5, 1.0));
  sceneMaterials.push_back(new Material(0, 0, 0, 1.0, 0.0, 0.0, 0, 0, 64, 0.0, 1.0));

  sceneLights.push_back(new AreaLight(4, 4, 4, 4, 2, 0, 20, 50, 0, -1, 0, 1.0, 0.9, 0.45, 2, 1));

  MaterialList::iterator mi = sceneMaterials.begin();

  sceneObjects.push_back(new Sphere(15, 5, 50, 5, *mi));
  sceneObjects.push_back(new Sphere(-15, 5, 50, 5, *mi));
  sceneObjects.push_back(new Sphere(0, 5, 65, 5, *mi));
  sceneObjects.push_back(new Sphere(0, 5, 35, 5, *mi));
  mi++;
  sceneObjects.push_back(new Sphere(0, 12, 50, 5, *mi));
  mi++;
  sceneObjects.push_back(new Sphere(0, 12, 50, 10, *mi));
  mi++;
  sceneObjects.push_back(new Plane(0, 0, 0, 0, 1, 0, *mi));
  mi++;
  sceneObjects.push_back(new Plane(-50, 0, 0, 1, 0, 0, *mi));
  sceneObjects.push_back(new Plane(0, 0, 100, 0, 0, -1, *mi));

}

void Scene::deleteRayArray(VisionRay **rays, int nRays) {
  for(int i = 0; i < nRays; i++) {
    delete rays[i];
    rays[i] = NULL;
  }
}

void Scene::outputImage(double* image,
    int image_w, int image_h,
    int imageFlag, std::string imageName) {

  int s = samplesPerPixel*samplesPerPixel;
  int k = 0;
  pngwriter image_out(image_w, image_h, 0, imageName.c_str());

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
      if(saveZBuffer && rays[i]->zBufferPixel != NULL) {
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
    // Mem Clean: Old Secondary Rays
    delete oldRays[i];
  }
  delete [] oldRays;
  oldRays = NULL;
}

void Scene::render() {

  double lookAtDir[3];
  blasSubstract(cameraLookAt, cameraPos, lookAtDir);
  blasNormalize(lookAtDir);
  blasBuildRotMatDir(lookAtDir, rayTransformationMat);

  res = image_height*image_width;
  renderedImage = new double[res*3];
  zBuffer = new double[res];
  // Initialize Rendered Image Array
  for(register int i = 0; i < res*3; i++)
    renderedImage[i] = 0.0;

  // Initialize zBufferArray
  for(register int i = 0; i < res; i++)
    zBuffer[i] = -INF_LIMIT;

  // Projection Plane Pixel Grid
  double aspect = (image_height*1.0) / image_width;
  double delta = 3.6/image_width;

  // Pixel
  int sqrSamples = samplesPerPixel*samplesPerPixel;
  int rowLength = image_width*sqrSamples;
  double sDelta = delta/(samplesPerPixel + 1);
  double sOffset = delta/2;

  struct tm *currTimeA, *currTimeB;
  time_t start, end;
  time(&start);
  currTimeA = localtime(&start);
  std::cout << "Rendering started @ " <<
    currTimeA->tm_hour << ':' << currTimeA->tm_min << ':' << currTimeA->tm_sec << '\n';

  char progress[52];
  for(register int i = 1; i < 51; i++)
    progress[i] = ' ';
  progress[0] = '['; progress[51] = ']';
  int p, oldp;
  p = oldp = 0;
  std::cout << "\r" << progress << "\t" << p << "%";
  std::flush(std::cout);

  VisionRay **rays;
  int x, y;

  // Rows
  #pragma omp parallel for schedule(runtime) default(shared) \
  private(x, y, rays)
  for(y = 0; y < image_height; y++) {
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

      while(currDepth < secondaryRaysDepth) {
        nRays = nSecRays;
        nSecRays = raytrace(secRays, nRays);
        if(nSecRays > 0) {
          rays = secRays;
          secRays = new VisionRay *[nSecRays];
          buildSecondaryRays(rays, secRays, nRays);
          currDepth++;
        }
        else {
          // Mem Clean: Residual Rays
          deleteRayArray(secRays, nRays);
          delete [] secRays;
          secRays = NULL;
          currDepth = secondaryRaysDepth;
        }
      }
    }
    else {
      deleteRayArray(rays, image_width*sqrSamples);
      delete [] rays;
      rays = NULL;
    }
    yDelta += delta;

    #pragma omp critical
    {
      p = y*100/image_height;
      if(p > oldp) {
        progress[(p >> 1) + 1] = '#';
        std::cout << "\r" << progress << "\t" << p << "%";
        std::flush(std::cout);
        oldp = p;
      }
    }
  }

  time(&end);
  currTimeB = localtime(&end);

  std::cout << "\r                                                                      ";
  std::cout << "\rRendering finished @ " <<
    currTimeB->tm_hour << ':' << currTimeB->tm_min << ':' << currTimeB->tm_sec;
  end = end - start;
  std::cout << "\nElapsed time: " << end << " seconds.\n";
  std::cout << "Writing output...";
  std::flush(std::cout);

  // Saving Rendered Image
  outputImage(renderedImage, image_width, image_height, 1, "image.png");

  // Saving Render ZBuffer Image
  if(saveZBuffer) {
    for(register int i = 0; i < res; i++) {
      if(zBuffer[i] != -std::numeric_limits<float>::infinity()) {
        zBuffer[i] = -fabs(focusDistance - zBuffer[i]);
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

  std::cout << "\rGoodbye!                               " << std::endl;
}

void Scene::intersectRay(Ray *r) {

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
  }
  else {
    r->sumTs = INF_LIMIT;
    r->intersectionT = INF_LIMIT;
  }
}

int Scene::shadeRayIntersection(VisionRay *r) {
  if(r->getWeight() > COLOR_EPS) {

    // Shading Variables
    int secRays = 0;
    double lDistance, phongD, phongS;
    Material *m = r->intersectionMaterial;

    double pixelBaseColor[3];

    pixelBaseColor[0] = m->color[0] * m->opacy;
    pixelBaseColor[1] = m->color[1] * m->opacy;
    pixelBaseColor[2] = m->color[2] * m->opacy;

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
    blasCopy(r->direction, v);
//    blasSubstract(r->position, intersectionPoint, v);
//    blasNormalize(v);

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
        shadowRay = new ShadowRay(intersectionPoint, lVec);

        if(shadows && m->opacy != 0) {
          shadowRay->move2Dir();

          do {
            intersectRay(shadowRay);
            if(shadowRay->sumTs < lDistance) {
              shadowRay->nextStep();
              shadowLight = shadowRay->getWeight();
            }
            else break;
          } while(shadowRay->intersectionT != INF_LIMIT
              && shadowLight > COLOR_EPS);
        }

        if(shadowLight > COLOR_EPS) {

          lDistance += r->sumTs;

          double luminosity = lightSource->intensity*LIGHT_SCALE;
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

          r->pixel[0] += lightSource->color[0] * r->weight[0] *
              (pixelBaseColor[0] * phongD + phongS) * shadowRay->weight[0];
          r->pixel[1] += lightSource->color[1] * r->weight[1] *
              (pixelBaseColor[1] * phongD + phongS) * shadowRay->weight[1];
          r->pixel[2] += lightSource->color[2] * r->weight[2] *
              (pixelBaseColor[2] * phongD + phongS) * shadowRay->weight[2];
        }
        currSample++;
        delete shadowRay;
      }
      r->pixel[0] += r->weight[0] * pixelBaseColor[0] * m->ambient;
      r->pixel[1] += r->weight[1] * pixelBaseColor[1] * m->ambient;
      r->pixel[2] += r->weight[2] * pixelBaseColor[2] * m->ambient;
    }
    return secRays;
  }
  else {
    return 0;
  }
}
