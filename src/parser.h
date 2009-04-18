/*
 * File: parser.h
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

#ifndef PARSER_H_
#define PARSER_H_

#include <xercesc/dom/DOM.hpp>
#include <xercesc/dom/DOMDocument.hpp>
#include <xercesc/dom/DOMDocumentType.hpp>
#include <xercesc/dom/DOMElement.hpp>
#include <xercesc/dom/DOMImplementation.hpp>
#include <xercesc/dom/DOMImplementationLS.hpp>
#include <xercesc/dom/DOMNodeIterator.hpp>
#include <xercesc/dom/DOMNodeList.hpp>
#include <xercesc/dom/DOMText.hpp>

#include <xercesc/parsers/XercesDOMParser.hpp>
#include <xercesc/util/XMLUni.hpp>

#include <string>
#include <stdexcept>

#include "scene.h"
#include "lights.h"
#include "materials.h"
#include "primitives.h"

// Error codes

enum {
   ERROR_ARGS = 1,
   ERROR_XERCES_INIT,
   ERROR_PARSE,
   ERROR_EMPTY_DOCUMENT
};

class SceneParser
{
public:
  SceneParser();
  ~SceneParser();
  void readSceneFile(std::string&, Scene *s) throw(std::runtime_error);

private:
   xercesc::XercesDOMParser *m_SceneFileParser;

   // Internal class use only. Hold Xerces data in UTF-16 SMLCh type.

   XMLCh* TAG_Scene;

   // Scene Parameters
   XMLCh* TAG_RenderSettings;
   XMLCh* ATTR_image_width;
   XMLCh* ATTR_image_height;
   XMLCh* ATTR_res;

   XMLCh* ATTR_cameraPosX;
   XMLCh* ATTR_cameraPosY;
   XMLCh* ATTR_cameraPosZ;
   XMLCh* ATTR_cameraLookAtX;
   XMLCh* ATTR_cameraLookAtY;
   XMLCh* ATTR_cameraLookAtZ;
   XMLCh* ATTR_cameraRollAngle;

   XMLCh* ATTR_focalLength;
   XMLCh* ATTR_focusDistance;
   XMLCh* ATTR_zBufferMaxDepth;
   XMLCh* ATTR_saveZBuffer;

   XMLCh* ATTR_samplesPerPixel;
   XMLCh* ATTR_secondaryRaysDepth;
   XMLCh* ATTR_shadows;

   // Light Parameters
   XMLCh* ATTR_LightPosX;
   XMLCh* ATTR_LightPosY;
   XMLCh* ATTR_LightPosZ;
   XMLCh* ATTR_LightColorR;
   XMLCh* ATTR_LightColorG;
   XMLCh* ATTR_LightColorB;
   XMLCh* ATTR_LightIntensity;
   XMLCh* ATTR_LightDamping;

   // Area light
   XMLCh* TAG_AreaLight;
   XMLCh* ATTR_AreaLightDirX;
   XMLCh* ATTR_AreaLightDirY;
   XMLCh* ATTR_AreaLightDirZ;
   XMLCh* ATTR_AreaLightHeight;
   XMLCh* ATTR_AreaLightWidth;
   XMLCh* ATTR_AreaLightGridHeight;
   XMLCh* ATTR_AreaLightGridWidth;
   XMLCh* ATTR_AreaLightSampleDensity;

   // Point light
   XMLCh* TAG_OmniLight;

   // Material Parameters
   XMLCh* TAG_Material;
   XMLCh* ATTR_MaterialColorR;
   XMLCh* ATTR_MaterialColorG;
   XMLCh* ATTR_MaterialColorB;
   XMLCh* ATTR_MaterialOpacy;
   XMLCh* ATTR_MaterialFilter;
   XMLCh* ATTR_MaterialInterior;
   XMLCh* ATTR_MaterialReflection;
   XMLCh* ATTR_MaterialAmbient;
   XMLCh* ATTR_MaterialDiffuse;
   XMLCh* ATTR_MaterialSpecular;
   XMLCh* ATTR_MaterialSpecHardness;

   // Primitive Parameters
   XMLCh* ATTR_PrimitivePosX;
   XMLCh* ATTR_PrimitivePosY;
   XMLCh* ATTR_PrimitivePosZ;
   XMLCh* ATTR_MaterialIndex;
   // Sphere
   XMLCh* TAG_Sphere;
   XMLCh* ATTR_SphereRadius;
   // Plane
   XMLCh* TAG_Plane;
   XMLCh* ATTR_PlaneNormalX;
   XMLCh* ATTR_PlaneNormalY;
   XMLCh* ATTR_PlaneNormalZ;


};

#endif /* PARSER_H_ */
