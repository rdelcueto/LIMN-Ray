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

  num_threads = omp_get_max_threads();
  fileOut = "";
  image_width = 1;
  image_height = 1;
  res = image_height*image_width;

  cameraPos[0] = 0; cameraPos[1] = 0; cameraPos[2] = 0;
  cameraLookAt[0] = 0; cameraLookAt[1] = 0; cameraLookAt[2] = 1;
  cameraRollAngle = 0;

  focalLength = 50/10;
  focusDistance = 50;
  zBufferMaxDepth = 64;
  saveZBuffer = 1;

  sqrtSamplesPerPixel = 1;
  secondaryRaysDepth = 1;
  shadows = 1;

}

void Scene::benchmarkScene() {

  // Hardcoded Benchmark Scene

  image_width = 640;
  image_height = 480;

  cameraPos[0] = -24; cameraPos[1] = 24; cameraPos[2] = -24;
  cameraLookAt[0] = 0; cameraLookAt[1] = -8; cameraLookAt[2] = 0;
  cameraRollAngle = 0;

  focalLength = 28/10;
  focusDistance = 42;
  zBufferMaxDepth = 48;
  saveZBuffer = 1;

  sqrtSamplesPerPixel = 1;
  secondaryRaysDepth = 2;
  shadows = 1;

  sceneMaterials.push_back(new
      Material(0.94, 0.94, 0.94, 1.0, 0.0, 0.1, 0.2, 0.75, 128, 0.66, 1.0));
  sceneMaterials.push_back(new
      Material(0.1, 0.15, 0.8, 1.0, 0.0, 0.0, 0.1, 1.0, 64, 1.0, 1.0));
  sceneMaterials.push_back(new
      Material(0.99, 0.2, 0.1, 0.3, 0.7, 0.0, 0.33, 1.0, 64, 0.75, 1.2));
  sceneMaterials.push_back(new
      Material(0.2, 0.9, 0.0, 0.5, 0.5, 0.0, 0.33, 1.0, 64, 0.75, 0.98));

  sceneMaterials.push_back(new
      Material(0.025, 0.025, 0.025, 0.25, 0.8, 0.0, 0.05, 0.66, 128, 0.9, 1.0));
  sceneMaterials.push_back(new
      Material(0.8, 0.85, 1.0, 0.1, 0.2, 0.0, 0.15, 0.66, 128, 0.33, 1.05));
  sceneMaterials.push_back(new
      Material(0.99, 0.8, 0.45, 0.1, 0.9, 0.0, 0.66, 0.9, 32, 0.8, 1.5));

  sceneMaterials.push_back(new
      Material(0.4, 0.36, 0.25, 1.0, 0.0, 0.0, 0.5, 0.1, 128, 0.5, 1.0));

    sceneLights.push_back(new
        AreaLight(.01, .01, 4, 4, 4, 0, 5, 0, 0, -1, 0, 0.95, 0.9, 0.7, 1, 2));

  MaterialList::iterator mi = sceneMaterials.begin();

  sceneObjects.push_back(new Sphere(10, 0, 17.32, 5, *mi));
  sceneObjects.push_back(new Sphere(-20, 0, 0, 5, *mi));
  sceneObjects.push_back(new Sphere(10, 0, -17.32, 5, *mi)); mi++;

  sceneObjects.push_back(new Sphere(-10, 0, 17.32, 5, *mi)); mi++;
  sceneObjects.push_back(new Sphere(-10, 0, -17.32, 5, *mi)); mi++;
  sceneObjects.push_back(new Sphere(20, 0, 0, 5, *mi)); mi++;

  sceneObjects.push_back(new Sphere(-4.33, 0, -2.5, 4, *mi)); mi++;
  sceneObjects.push_back(new Sphere(4.33, 0, -2.5, 4, *mi)); mi++;
  sceneObjects.push_back(new Sphere(0, 0, 5, 4, *mi)); mi++;

  sceneObjects.push_back(new Plane(0, -5.01, 0, 0, 1, 0, *mi));

}

void Scene::deleteRayArray(VisionRay **rays, int nRays) {
  for(int i = 0; i < nRays; i++) {
    delete rays[i];
    rays[i] = NULL;
  }
}

void Scene::outputImage(std::string imageName) {

  ssize_t columns = image_width;
  ssize_t rows = image_height;
  
  Magick::Image image(Magick::Geometry(columns, rows), "white");
  image.type(Magick::TrueColorType);
  
  const int s = sqrtSamplesPerPixel*sqrtSamplesPerPixel;
  int k = 0;

  
  Magick::PixelPacket *pixel_cache;
  Magick::PixelPacket *pixel;

  pixel_cache = image.getPixels(0, 0, columns, rows);
  
  register float r, g, b;
  for(int y = 0; y < image_height; y++)
    for(int x = 0; x < image_width; x++) {
      
      r = renderedImage[k];
      g = renderedImage[k+1];
      b = renderedImage[k+2];
        
      if(sqrtSamplesPerPixel > 1) {
        r /= s;
        g /= s;
        b /= s;
      }

      r = r > 1 ? 1 : r < 0 ? 0 : r;
      g = g > 1 ? 1 : g < 0 ? 0 : g;
      b = b > 1 ? 1 : b < 0 ? 0 : b;

      pixel = pixel_cache+(image_height - y - 1)*columns+x;
      *pixel = Magick::ColorRGB(r, g, b);
      k+=3;
    }

  image.syncPixels();
  std::stringstream render_filename;
  render_filename << "png32:" << imageName << ".png";
  image.write(render_filename.str());

  if(saveZBuffer) {
    Magick::Image zbuffer_image(Magick::Geometry(columns, rows), "black");
    pixel_cache = zbuffer_image.getPixels(0, 0, columns, rows);

    k = 0;

    register float z;
    for(int y = 0; y < image_height; y++) {
      for(int x = 0; x < image_width; x++) {

        z = zBuffer[k];
        z = z > 1 ? 1 : z < 0 ? 0 : z;
        pixel = pixel_cache+(image_height - y - 1)*columns+x;
        *pixel = Magick::ColorGray(z);
        k++;
      }
    }
    
    zbuffer_image.syncPixels();
    std::stringstream zbuffer_filename;
    zbuffer_filename << "png32:" << imageName << "_zbuffer.png";
    zbuffer_image.write(zbuffer_filename.str());
  }
}

int Scene::raytrace(VisionRay **rays, int nRays) {
  int i, nSecRays;
  nSecRays = 0;
  for(i = 0; i < nRays; i++) {
    intersectRay(rays[i]);
    if(INF_LIMIT != rays[i]->intersectionT) {
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

  float lookAtDir[3];
  blasSubstract(cameraLookAt, cameraPos, lookAtDir);
  blasNormalize(lookAtDir);

  float up[3] = {0.0}; up[1] = 1.0;
  if(cameraRollAngle != 0) {
    float roll[9] = {0.0};
    float zAxis[3] = {0.0}; zAxis[2] = 1.0;
    blasRotVec(zAxis, cosf(cameraRollAngle), sinf(cameraRollAngle), roll);
    blasVecMatrixProd(up, roll, up);
  }
  blasRotNV(lookAtDir, up, rayTransformationMat);

  res = image_height*image_width;
  renderedImage = new float[res*3];
  zBuffer = new float[res];
  // Initialize Rendered Image Array
  for(register int i = 0; i < res*3; i++)
    renderedImage[i] = 0.0;

  // Initialize zBufferArray
  for(register int i = 0; i < res; i++)
    zBuffer[i] = -INF_LIMIT;

  // Projection Plane Pixel Grid
  float aspect = (image_height*1.0) / image_width;
  float delta = 3.6/image_width;

  // Pixel
  int sqrSamples = sqrtSamplesPerPixel*sqrtSamplesPerPixel;
  int rowLength = image_width*sqrSamples;
  float sDelta = delta/(sqrtSamplesPerPixel + 1);
  float sOffset = delta/2;

  struct tm *currTimeA, *currTimeB;
  time_t start, end;
  time(&start);
  currTimeA = localtime(&start);
  std::cout << "Rendering started @ " <<
    currTimeA->tm_hour << ':' << currTimeA->tm_min << ':' << currTimeA->tm_sec << '\n';

  int p, oldp;
  p = oldp = 0;
  std::cout << "\r" << "Completed: " << p << "%";
  std::flush(std::cout);

  VisionRay **rays;
  int x, y;

  const int concurrent_rendering_threads = this->num_threads;
  
  // Rows
#pragma omp parallel for schedule(static,1) default(shared) private(x, y, rays) num_threads(concurrent_rendering_threads)
  for(y = 0; y < image_height; y++) {
    int k, nRays, nSecRays, currDepth, zbufferPixel, imagePixel;
    nRays = rowLength;
    nSecRays = 0;
    currDepth = 0;
    k = 0;

    rays = new VisionRay *[nRays*sqrSamples];

    float xDelta = -1.8;
    float yDelta = xDelta*aspect + delta*y;
    float xsDelta, ysDelta;
    float lookAtRay[3];

    zbufferPixel = y*image_width;
    imagePixel = zbufferPixel*3;

    // Row Columns
    for(x = 0; x < image_width; x++) {
      // Multisampling
      if(sqrtSamplesPerPixel > 1) {
        ysDelta = sDelta - sOffset;
        for(int i = 0; i < sqrtSamplesPerPixel; i++) {
          xsDelta = sDelta - sOffset;
          for(int j = 0; j < sqrtSamplesPerPixel; j++) {
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
        std::cout << "\r" << "Completed: " << p << "%";
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
  std::cout << "\nElapsed time: " << end << " seconds.";
  if(end == 0) std::cout << "\nAverage rendering speed: " << res << " px/s\n";
  else std::cout << "\nAverage rendering speed: " << res/end << " px/s\n";
  std::cout << "Writing output...";
  std::flush(std::cout);

  // Process Z-Buffer
  if(saveZBuffer) {
    for(register int i = 0; i < res; i++) {
      if(zBuffer[i] != -std::numeric_limits<float>::infinity()) {
        zBuffer[i] = -fabs(focusDistance - zBuffer[i]);
        zBuffer[i] /= zBufferMaxDepth;
        zBuffer[i] += 1.0;
      }
    }
  }

  // Saving Rendered Image
  outputImage(fileOut);

  delete [] renderedImage;
  delete [] zBuffer;

  sceneLights.clear();
  sceneMaterials.clear();
  sceneObjects.clear();

  std::cout << "\rGoodbye!                               " << std::endl;
}

void Scene::intersectRay(Ray *r) {

  // Intersection Variables
  float minT, currT;
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
    float lDistance, phongD, phongS;
    Material *m = r->intersectionMaterial;

    float tmpPixel[3] = {0.0};
    float pixelBaseColor[3];

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

    float intersectionPoint[3];
    blasCopy(r->intersectionPoint, intersectionPoint);
    float intersectionNormal[3];
    blasCopy(r->intersectionNormal, intersectionNormal);
    // View = Ray Pos - Intersection
    float v[3];
    blasCopy(r->direction, v);

    // Light Unit Vector from Intersection Point (Sample Independant)
    float lPos[3];
    // Light Unit Vector from Intersection Point (Sample Dependant)
    float lVec[3];
    // h = (lVec + View)/2
    float h[3];
    // |d| = (hdotn)n-h Specular Phong Reformulation
    float d[3];

    int lSamples;
    float samplePos[3] = {0.0};
    Light *lightSource;
    LightList::iterator i;

    for(i = sceneLights.begin(); i != sceneLights.end(); ++i) {

      int currSample = 0;
      lightSource = *i;

      #pragma omp critical
      {
        lSamples = lightSource->getSamples();
      }
      
      blasSubstract(intersectionPoint, lightSource->pos, lPos);
      blasVeryFastNormalize(lPos);

      while (currSample < lSamples) {
        lightSource->getPosI(currSample, samplePos);
        blasSubstract(samplePos, intersectionPoint, lVec);
        lDistance = blasNrm2(lVec);
        blasVeryFastNormalize(lVec);

        ShadowRay *shadowRay = NULL;
        float shadowLight = 1.0;
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

          float luminosity =
            LIGHT_SCALE/(lDistance*lDistance*lightSource->damping);

          // Diffuse
          if(m->diffuse > 0)
            phongD = blasDot(lVec, intersectionNormal)*m->diffuse*luminosity;
          else phongD = 0;

          // Specular
          if(m->specular > 0) {
            blasAdd(lPos, v, h); // LightPos + EyePos = 2x Halfway vector
            blasCopy(intersectionNormal, d);
            blasScale(d, blasDot(h, d), d); // (NdotNormAtP)NormAtP
            blasSubstract(d, h, d);
            phongS = (m->specular_Hardness/2)*blasDot(d, d);
            if (phongS > 4) phongS = 0;
            else {
              phongS = (1 - phongS/4);
              phongS *= phongS;
              phongS *= phongS * m->specular;
            }
          }
          else phongS = 0;

          tmpPixel[0] += lightSource->color[0] * lightSource->intensity *
              ((pixelBaseColor[0] * phongD) + phongS) * shadowRay->weight[0];
          tmpPixel[1] += lightSource->color[1] * lightSource->intensity *
              ((pixelBaseColor[1] * phongD) + phongS) * shadowRay->weight[1];
          tmpPixel[2] += lightSource->color[2] * lightSource->intensity *
              ((pixelBaseColor[2] * phongD) + phongS) * shadowRay->weight[2];
        }
        currSample++;
        delete shadowRay;
      }

      if(saveZBuffer && r->zBufferPixel != NULL)
        r->zBufferPixel[0] = r->sumTs;
      
      r->pixel[0] += r->weight[0] *
                     (pixelBaseColor[0] * m->ambient + tmpPixel[0]);
      r->pixel[1] += r->weight[1] *
                     (pixelBaseColor[1] * m->ambient + tmpPixel[1]);
      r->pixel[2] += r->weight[2] *
                     (pixelBaseColor[2] * m->ambient + tmpPixel[2]);
      
    }
    return secRays;
  }
  else {
    return 0;
  }
}
