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
  register int k = 0;
  for(register int y = 1; y <= image_h; y++)
    for(register int x = 1; x <= image_w; x++) {
      image_out.plot(x, y, image[k], image[k+1], image[k+2]);
      k+=3;
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

void raytrace(Scene *s, Ray **rays, int res) {
  for(register int i = 0; i < res; i++) {
    s->intersect(rays[i]);
    if(std::numeric_limits<float>::infinity() != rays[i]->t_intersect)
      s->shader(rays[i]);
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

  Ray **rays = new Ray *[res];
  register int k = 0;
  for(int i = 0; i < rows; i++) {
    pixel = i*cols*3;
    for(int j = 0; j < cols; j++) {
      rays[k] = new Ray(
          image+pixel+(j*3), s->focalPointPos, xDelta, yDelta, 0);
      k++;
      xDelta += delta;
    }
    xDelta = -0.5;
    yDelta += delta;
  }
  raytrace(s, rays, res);
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
  delete [] image;
  image = NULL;
  return 0;
}
