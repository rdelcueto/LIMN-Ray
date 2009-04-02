/*
 * File: limn-ray.cpp
 * Author: Rodrigo González del Cueto
 */

/*
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

#define TILE_SIZE 4
#define TILE_RES (TILE_SIZE*TILE_SIZE)

#include <iostream>
#include <fstream>
#include <pngwriter.h>
#include <limits>

#include "scene.h"
#include "primitives.h"

double diffclock(clock_t clock1, clock_t clock2) {
    double diffticks = clock1-clock2;
    double diffms=diffticks/CLOCKS_PER_SEC;
    return diffms;
}

int outputImage(double* image, int image_w, int image_h) {
  //int res = image_w * image_h;
  pngwriter image_out(image_w, image_h, 0, "out.png");
  register int k = 0;
  for(register int y = 1; y <= image_h; y++)
    for(register int x = 1; x <= image_w; x++) {
      image_out.plot(x, y, image[k], image[k+1], image[k+2]);
      k+=3;
    }

  image_out.close();
  image_out.clear();
  return 0;
}

void printSceneDesc(Scene *s) {
  std::cout << "\tRender Resolution: "
    << s->plane_w << "x" << s->plane_h << " = " << s->plane_w*s->plane_h << " pixels.\n";
  std::cout << "\tCamera Position: ("
    << s->planePos[0] << ", " << s->planePos[1] << ", " << s->planePos[2] << ")\n";
  std::cout << "\tCamera LookAt: ("
        << s->lookAt[0] << ", " << s->lookAt[1] << ", " << s->lookAt[2] << ")\n";
  std::cout << "\tRecursive Limit: " << s->recurLimit
    << "\n\tSamples: " << s->samples << "\n\n";
}

static int raytrace(Scene *s, Ray **rays, int nRays) {
  int newRays = 0;
  for(int i = 0; i < nRays; i++) {
    s->intersect(rays[i]);
    if(std::numeric_limits<float>::infinity() != rays[i]->t_intersect)
      newRays += s->shader(rays[i]);
  }
  return newRays;
}

void createSecondaryRays(Ray **oldRays, Ray **newRays, int nRays) {
  int j = 0;
  for(int i = 0; i < nRays; i++) {
    if(oldRays[i]->type != 0) {
      if(oldRays[i]->type % 2 != 0) {
        newRays[j] = new Ray();
        newRays[j]->pixel = oldRays[i]->pixel;
        newRays[j]->setPos(oldRays[i]->int_p);
        newRays[j]->setDir(oldRays[i]->dir);
        j++;
      }
    }
  }
}

static void render(Scene *s, double *image) {

  int cols = s->plane_w;
  int rows = s->plane_h;
  int res = cols * rows;

  register int pixel;

  double aspect = (rows*1.0) / cols;
  double delta = 1.0/cols;
  double xDelta = -0.5;
  double yDelta = -aspect/2.0;

  double tileXDelta;

  int k = 0;
  int xTiles = cols/TILE_SIZE;
  int yTiles = rows/TILE_SIZE;
  int xPos;
  int yPos;
  int nRays = TILE_RES;
  int depth = s->recurLimit;

  for(int ty = 0; ty < yTiles; ty++) {
    for(int tx = 0; tx < xTiles; tx++) {
      // Create Tile
      Ray **rays = new Ray *[TILE_RES];
      xPos = tx*TILE_SIZE;
      yPos = ty*TILE_SIZE;
      tileXDelta = -0.5 + delta*xPos;
      yDelta = -aspect/2.0 + delta*yPos;
      k = 0;
      for(int y = 0; y < TILE_SIZE; y++) {
        pixel = (yPos + y)*cols*3;
        xDelta = tileXDelta;
        for(int x = 0; x < TILE_SIZE; x++) {
          rays[k] = new Ray(
                        image+pixel+((xPos + x)*3),
                        s->focalPointPos,
                        xDelta,
                        yDelta,
                        0);
          k++;
          xDelta += delta;
        }
        yDelta += delta;
      }
      // Render Tile: Primary Rays
      int nSecRays = raytrace(s, rays, nRays);

      // Secondary Rays
      int currDepth = 0;
      Ray **secRays = new Ray *[nSecRays];

      while(nRays != 0 && currDepth < depth) {
        createSecondaryRays(rays, secRays, nRays);
        nSecRays = raytrace(s, secRays, nSecRays);
        secRays = new Ray *[nSecRays];
        currDepth++;
      }

    }
  }
  // Untiled Pixels
  //xTiles = rows % xTiles == 0 ? xTiles : xTiles + 1;
  //yTiles = rows % yTiles == 0 ? yTiles : yTiles + 1;
}

int main(int argc, char* argv[]) {

  std::cout << "Limn-Ray v0.2" << std::endl;

  // Parse Scene
  Scene *s = new Scene();
  printSceneDesc(s);

  // Initialize Image
  double* image = NULL;
  int res = s->plane_w * s->plane_h; // Width * Height
  int imageArraySize = res * 3; // RES * RGB
  image = new double[imageArraySize];

  for(register int i = 0; i < imageArraySize; i++)
    image[i] = 0.0;

  // Render Scene
  std::cout << "Rendering... ";
  clock_t begin = clock();
  render(s, image);
  clock_t end = clock();
  std::cout << "Done!\nRender Time: ~" << diffclock(end, begin) << "s.\n";

  // Output Render
  outputImage(image, s->plane_w, s->plane_h);

  // Mem clean
  s->sceneLights.clear();
  s->sceneMaterials.clear();
  s->sceneObjects.clear();
  delete s;
  delete [] image;
  return 0;
}
