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
  int res = image_w * image_h;
  pngwriter image_out(image_w, image_h, 0, "out.png");
  register int i = 0;
  register int j = 1;
  while(j <= res) {
    image_out.plot(j%image_w, j/image_w, image[i++], image[i++], image[i++]);
    j++;
  }
  image_out.close();
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

static void render(Scene *s, double *image) {

  int cols = s->plane_w;
  int rows = s->plane_h;
  int res = cols * rows;

  register int pixel;

  double aspect = (rows*1.0) / cols;
  double delta = 1.0/cols;
  register double xDelta = -0.5;
  register double yDelta = aspect/2.0;

  Ray *primaryRay = new Ray();
  double primaryRayPos[3];
  primaryRay->setPos(s->focalPointPos[0],
                     s->focalPointPos[1],
                     s->focalPointPos[2]);
  //for(register int k = 0; k < 100; k++)
  for(register int i = 0; i < rows; i++) {
    pixel = i*cols*3;
    for(register int j = 0; j < cols; j++) {
      primaryRay->setLookAt(xDelta, yDelta, 0);
      primaryRay->setIntersection(std::numeric_limits<float>::infinity(), NULL);

      s->intersect(primaryRay);
      if(std::numeric_limits<float>::infinity() != primaryRay->t_intersect) {
        s->shader(primaryRay, image+pixel+(j*3));

      }
      xDelta += delta;
    }
    xDelta = -0.5;
    yDelta -= delta;
  }
}

int main(int argc, char* argv[]) {

  std::cout << "Limn-Ray v0.1" << std::endl;

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
  delete [] image;
  image = NULL;
  //testBlas();
  return 0;
}
