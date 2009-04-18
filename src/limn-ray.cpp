/*
 * File: limn-ray.cpp
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

#include <iostream>
#include <limits>

#include "scene.h"
#include "parser.h"

int main(int argc, char **argv) {
  std::cout << "Limn-Ray v0.666 \n";
  std::string configFile = "";

  int verbose = 0;
//  omp_set_num_threads(omp_get_max_threads());

  Scene *scene = new Scene();

  for(int i = 1; i < argc; i++) {
    if(strcmp(argv[i], "-t") == 0) {
      int threads = atoi(argv[i+1]);
      omp_set_num_threads(threads);
      i++;
    }
    if(strcmp(argv[i], "-f") == 0) {
      configFile = argv[i+1];
      i++;
    }
    if(strcmp(argv[i], "-v") == 0) {
      verbose = 1;
    }
  }
  SceneParser *sceneConfig = new SceneParser();
  if(verbose)
    std::cout << "Opening scene file: " << configFile << std::endl;

  if(configFile.size() == 0) {
    std::cout << "No file specified, running demo scene.\n";
    scene->demoScene();
  }
  else
    sceneConfig->readSceneFile(configFile, scene);

  if(verbose) {
    std::cout << "\nScene Settings:\n" <<
      "\tRender Resolution:" <<
      scene->image_width << "x" << scene->image_height << "\n" <<
      "\tCamera Position: [" <<
      scene->cameraPos[0] << ", " <<
      scene->cameraPos[1] << ", " <<
      scene->cameraPos[2] << "]\n" <<
      "\tCamera LookAt: [" <<
      scene->cameraLookAt[0] << ", " <<
      scene->cameraLookAt[1] << ", " <<
      scene->cameraLookAt[2] << "]\n" <<
      "\tRoll Angle: " << scene->cameraRollAngle << " degrees\n" <<
      "\tFocal Length: " << scene->focalLength << "\n" <<
      "\tFocus Distance: " << scene->focusDistance << "\n" <<
      "\tSecondary Rays Depth: " << scene->secondaryRaysDepth << "\n";
      if(scene->shadows) std::cout << "\tShadows Enabled\n";
      else std::cout << "\tShadows Disabled\n";
      std::cout << std::endl <<
      "\tScene Lights: " << scene->sceneLights.size() << "\n" <<
      "\tScene Materials: " << scene->sceneMaterials.size() << "\n" <<
      "\tScene Objects: " << scene->sceneObjects.size() << "\n\n";
  }
  if(scene->sceneLights.size() != 0 &&
      scene->sceneObjects.size() != 0)
    scene->render();
  else {
    std::cout << "Done, nothing to render!\n";
    std::cerr << "Error: Image has no lights or objects\n";
    return 0;
  }
}
