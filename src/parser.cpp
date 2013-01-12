/*
 * File: parser.cpp
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

#include <string>
#include <iostream>
#include <sstream>
#include <stdexcept>
#include <list>

#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <errno.h>

#include "parser.h"

using namespace xercesc;
using namespace std;

/**
 *  Constructor initializes xerces-C libraries.
 *  The XML tags and attributes which we seek are defined.
 *  The xerces-C DOM parser infrastructure is initialized.
 */

SceneParser::SceneParser() {
  try
  {
    XMLPlatformUtils::Initialize();  // Initialize Xerces infrastructure
  }
  catch( XMLException& e )
  {
    char* message = XMLString::transcode( e.getMessage() );
    cerr << "XML toolkit initialization error: " << message << endl;
    XMLString::release( &message );
    // throw exception here to return ERROR_XERCES_INIT
  }

  // Tags and attributes used in XML file.
  // Can't call transcode till after Xerces Initialize()
  TAG_Scene = XMLString::transcode("Scene");
  TAG_RenderSettings = XMLString::transcode("RenderSettings");
  ATTR_image_height = XMLString::transcode("Height");
  ATTR_image_width = XMLString::transcode("Width");
  ATTR_cameraPosX = XMLString::transcode("CameraPosX");
  ATTR_cameraPosY = XMLString::transcode("CameraPosY");
  ATTR_cameraPosZ = XMLString::transcode("CameraPosZ");
  ATTR_cameraLookAtX = XMLString::transcode("CameraLookAtX");
  ATTR_cameraLookAtY = XMLString::transcode("CameraLookAtY");
  ATTR_cameraLookAtZ = XMLString::transcode("CameraLookAtZ");
  ATTR_cameraRollAngle = XMLString::transcode("CameraRollAngle");
  ATTR_focalLength = XMLString::transcode("FocalLength");
  ATTR_focusDistance = XMLString::transcode("FocusDistance");
  ATTR_saveZBuffer = XMLString::transcode("SaveZBuffer");
  ATTR_zBufferMaxDepth = XMLString::transcode("ZBufferMaxDepth");
  ATTR_samplesPerPixel = XMLString::transcode("SamplesPerPixel");
  ATTR_secondaryRaysDepth = XMLString::transcode("SecondaryRaysDepth");
  ATTR_shadows = XMLString::transcode("Shadows");

  ATTR_LightPosX = XMLString::transcode("LightPosX");
  ATTR_LightPosY = XMLString::transcode("LightPosY");
  ATTR_LightPosZ = XMLString::transcode("LightPosZ");
  ATTR_LightColorR = XMLString::transcode("LightColorR");
  ATTR_LightColorG = XMLString::transcode("LightColorG");
  ATTR_LightColorB = XMLString::transcode("LightColorB");
  ATTR_LightIntensity = XMLString::transcode("LightIntensity");
  ATTR_LightDamping = XMLString::transcode("LightDamping");

  // Area light
  TAG_AreaLight = XMLString::transcode("AreaLight");
  ATTR_AreaLightDirX = XMLString::transcode("AreaLightDirX");
  ATTR_AreaLightDirY = XMLString::transcode("AreaLightDirY");
  ATTR_AreaLightDirZ = XMLString::transcode("AreaLightDirZ");
  ATTR_AreaLightHeight = XMLString::transcode("AreaLightHeight");
  ATTR_AreaLightWidth = XMLString::transcode("AreaLightWidth");
  ATTR_AreaLightGridHeight = XMLString::transcode("AreaLightGridHeight");
  ATTR_AreaLightGridWidth = XMLString::transcode("AreaLightGridWidth");
  ATTR_AreaLightSampleDensity = XMLString::transcode("AreaLightSampleDensity");

  // Point light
  TAG_OmniLight = XMLString::transcode("PointLight");

  // Material Parameters
  TAG_Material = XMLString::transcode("Material");
  ATTR_MaterialColorR = XMLString::transcode("MaterialColorR");
  ATTR_MaterialColorG = XMLString::transcode("MaterialColorG");
  ATTR_MaterialColorB = XMLString::transcode("MaterialColorB");
  ATTR_MaterialOpacy = XMLString::transcode("MaterialOpacy");
  ATTR_MaterialFilter = XMLString::transcode("MaterialFilter");
  ATTR_MaterialInterior = XMLString::transcode("MaterialInterior");
  ATTR_MaterialReflection = XMLString::transcode("MaterialReflection");
  ATTR_MaterialAmbient = XMLString::transcode("MaterialAmbient");
  ATTR_MaterialDiffuse = XMLString::transcode("MaterialDiffuse");
  ATTR_MaterialSpecular = XMLString::transcode("MaterialSpecular");
  ATTR_MaterialSpecHardness = XMLString::transcode("MaterialSpecHardness");

  // Primitive Parameters
  ATTR_PrimitivePosX = XMLString::transcode("PrimitivePosX");
  ATTR_PrimitivePosY = XMLString::transcode("PrimitivePosY");
  ATTR_PrimitivePosZ = XMLString::transcode("PrimitivePosZ");
  // Sphere
  TAG_Sphere = XMLString::transcode("Sphere");
  ATTR_SphereRadius = XMLString::transcode("SphereRadius");
  // Plane
  TAG_Plane = XMLString::transcode("Plane");
  ATTR_PlaneNormalX = XMLString::transcode("PlaneNormalX");
  ATTR_PlaneNormalY = XMLString::transcode("PlaneNormalY");
  ATTR_PlaneNormalZ = XMLString::transcode("PlaneNormalZ");

   m_SceneFileParser = new XercesDOMParser;
}

/**
 *  Class destructor frees memory used to hold the XML tag and
 *  attribute definitions. It als terminates use of the xerces-C
 *  framework.
 */

SceneParser::~SceneParser()
{
   try
   {
      XMLPlatformUtils::Terminate();  // Terminate Xerces
   }
   catch( xercesc::XMLException& e )
   {
      char* message = xercesc::XMLString::transcode( e.getMessage() );

      cerr << "XML toolkit teardown error: " << message << endl;
      XMLString::release( &message );
   }

   try
   {
      XMLString::release( &TAG_Scene );
      XMLString::release( &TAG_RenderSettings );
      XMLString::release( &ATTR_cameraPosX );
      XMLString::release( &ATTR_cameraPosY );
      XMLString::release( &ATTR_cameraPosZ );
      XMLString::release( &ATTR_cameraLookAtX );
      XMLString::release( &ATTR_cameraLookAtY );
      XMLString::release( &ATTR_cameraLookAtZ );
      XMLString::release( &ATTR_cameraLookAtZ );
      XMLString::release( &ATTR_cameraRollAngle );
      XMLString::release( &ATTR_focalLength );
      XMLString::release( &ATTR_focusDistance );
      XMLString::release( &ATTR_saveZBuffer );
      XMLString::release( &ATTR_zBufferMaxDepth );
      XMLString::release( &ATTR_samplesPerPixel );
      XMLString::release( &ATTR_secondaryRaysDepth );
      XMLString::release( &ATTR_shadows );

      XMLString::release( &ATTR_LightColorR );
      XMLString::release( &ATTR_LightColorG );
      XMLString::release( &ATTR_LightColorB );
      XMLString::release( &ATTR_LightPosX );
      XMLString::release( &ATTR_LightPosY );
      XMLString::release( &ATTR_LightPosZ );
      XMLString::release( &ATTR_LightIntensity );
      XMLString::release( &ATTR_LightDamping );

      XMLString::release( &TAG_AreaLight );
      XMLString::release( &ATTR_AreaLightDirX );
      XMLString::release( &ATTR_AreaLightDirY );
      XMLString::release( &ATTR_AreaLightDirZ );
      XMLString::release( &ATTR_AreaLightSampleDensity );
      XMLString::release( &ATTR_AreaLightHeight );
      XMLString::release( &ATTR_AreaLightWidth );
      XMLString::release( &ATTR_AreaLightGridHeight );
      XMLString::release( &ATTR_AreaLightGridWidth );

      XMLString::release( &TAG_OmniLight );

      XMLString::release( &TAG_Material );
      XMLString::release( &ATTR_MaterialColorR );
      XMLString::release( &ATTR_MaterialColorG );
      XMLString::release( &ATTR_MaterialColorB );
      XMLString::release( &ATTR_MaterialOpacy );
      XMLString::release( &ATTR_MaterialFilter );
      XMLString::release( &ATTR_MaterialReflection );
      XMLString::release( &ATTR_MaterialAmbient );
      XMLString::release( &ATTR_MaterialDiffuse );
      XMLString::release( &ATTR_MaterialSpecular );
      XMLString::release( &ATTR_MaterialSpecHardness );
      XMLString::release( &ATTR_MaterialInterior );

      XMLString::release( &ATTR_PrimitivePosX );
      XMLString::release( &ATTR_PrimitivePosY );
      XMLString::release( &ATTR_PrimitivePosZ );

      XMLString::release( &TAG_Sphere );
      XMLString::release( &ATTR_SphereRadius );

      XMLString::release( &TAG_Plane );
      XMLString::release( &ATTR_PlaneNormalX );
      XMLString::release( &ATTR_PlaneNormalY );
      XMLString::release( &ATTR_PlaneNormalZ );
   }
   catch( ... )
   {
      cerr << "Unknown exception encountered in TagNamesdtor" << endl;
   }
}

/**
 *  This function:
 *  - Tests the access and availability of the XML configuration file.
 *  - Configures the xerces-c DOM parser.
 *  - Reads and extracts the pertinent information from the XML config file.
 *
 *  @param in configFile The text string name of the HLA configuration file.
 */

void SceneParser::readSceneFile(string& configFile, Scene *scene)
        throw( std::runtime_error )
{

   struct stat fileStatus;

   int iretStat = stat(configFile.c_str(), &fileStatus);
   if( iretStat == ENOENT )
      throw ( std::runtime_error("Path file_name does not exist, or path is an empty string.") );
   else if( iretStat == ENOTDIR )
      throw ( std::runtime_error("A component of the path is not a directory."));
   else if( iretStat == ELOOP )
      throw ( std::runtime_error("Too many symbolic links encountered while traversing the path."));
   else if( iretStat == EACCES )
      throw ( std::runtime_error("Permission denied."));
   else if( iretStat == ENAMETOOLONG )
      throw ( std::runtime_error("File can not be read\n"));

   // Configure DOM parser.

   m_SceneFileParser->setValidationScheme( XercesDOMParser::Val_Always );
   m_SceneFileParser->setDoNamespaces( true );
   m_SceneFileParser->setDoSchema( true );
   m_SceneFileParser->setLoadExternalDTD( true );

   try
   {
      m_SceneFileParser->parse( configFile.c_str() );

      // no need to free this pointer - owned by the parent parser object
      DOMDocument* xmlDoc = m_SceneFileParser->getDocument();
      if( !xmlDoc ) throw(std::runtime_error( "Error opening XML file" ));

      // Get the top-level element: NAme is "root". No attributes for "root"
      DOMElement* elementRoot = xmlDoc->getDocumentElement();
      if( !elementRoot ) throw(std::runtime_error( "Empty XML document" ));

      // Look one level nested within "root". (child of root)

      DOMNodeList* children = elementRoot->getChildNodes();
      const  XMLSize_t nodeCount = children->getLength();

      // For all nodes, children of "root" in the XML tree.

      for( XMLSize_t xx = 0; xx < nodeCount; ++xx )
      {
         DOMNode* currentNode = children->item(xx);
         if( currentNode->getNodeType() &&  // true is not NULL
             currentNode->getNodeType() == DOMNode::ELEMENT_NODE ) // is element
         {
            // Found node which is an Element. Re-cast node as element
            DOMElement* currentElement
                        = dynamic_cast< xercesc::DOMElement* >( currentNode );
            if( XMLString::equals(currentElement->getTagName(), TAG_RenderSettings))
            {

              const XMLCh* xmlch_image_width
                    = currentElement->getAttribute(ATTR_image_width);
              scene->image_width = atoi(XMLString::transcode(xmlch_image_width));
              const XMLCh* xmlch_image_height
                    = currentElement->getAttribute(ATTR_image_height);
              scene->image_height = atoi(XMLString::transcode(xmlch_image_height));

              const XMLCh* xmlch_cameraPosX
                    = currentElement->getAttribute(ATTR_cameraPosX);
              scene->cameraPos[0] = atof(XMLString::transcode(xmlch_cameraPosX));

              const XMLCh* xmlch_cameraPosY
                    = currentElement->getAttribute(ATTR_cameraPosY);
              scene->cameraPos[1] = atof(XMLString::transcode(xmlch_cameraPosY));

              const XMLCh* xmlch_cameraPosZ
                    = currentElement->getAttribute(ATTR_cameraPosZ);
              scene->cameraPos[2] = atof(XMLString::transcode(xmlch_cameraPosZ));

               const XMLCh* xmlch_cameraLookAtX
                     = currentElement->getAttribute(ATTR_cameraLookAtX);
               scene->cameraLookAt[0] = atof(XMLString::transcode(xmlch_cameraLookAtX));

               const XMLCh* xmlch_cameraLookAtY
                     = currentElement->getAttribute(ATTR_cameraLookAtY);
               scene->cameraLookAt[1] = atof(XMLString::transcode(xmlch_cameraLookAtY));

               const XMLCh* xmlch_cameraLookAtZ
                     = currentElement->getAttribute(ATTR_cameraLookAtZ);
               scene->cameraLookAt[2] = atof(XMLString::transcode(xmlch_cameraLookAtZ));

               const XMLCh* xmlch_cameraRollAngle
                     = currentElement->getAttribute(ATTR_cameraRollAngle);
               scene->cameraRollAngle = atof(XMLString::transcode(xmlch_cameraRollAngle));

               const XMLCh* xmlch_focalLength
                     = currentElement->getAttribute(ATTR_focalLength);
               scene->focalLength = atof(XMLString::transcode(xmlch_focalLength))/10;

               const XMLCh* xmlch_focusDistance
                     = currentElement->getAttribute(ATTR_focusDistance);
               scene->focusDistance = atof(XMLString::transcode(xmlch_focusDistance));

               const XMLCh* xmlch_zBufferMaxDepth
                     = currentElement->getAttribute(ATTR_zBufferMaxDepth);
               scene->zBufferMaxDepth = atof(XMLString::transcode(xmlch_zBufferMaxDepth));

               const XMLCh* xmlch_samplesPerPixel
                     = currentElement->getAttribute(ATTR_samplesPerPixel);
               scene->sqrtSamplesPerPixel = atof(XMLString::transcode(xmlch_samplesPerPixel));

               const XMLCh* xmlch_saveZBuffer
                     = currentElement->getAttribute(ATTR_saveZBuffer);
               scene->saveZBuffer = atoi(XMLString::transcode(xmlch_saveZBuffer));

               const XMLCh* xmlch_secondaryRaysDepth
                     = currentElement->getAttribute(ATTR_secondaryRaysDepth);
               scene->secondaryRaysDepth = atoi(XMLString::transcode(xmlch_secondaryRaysDepth));

               const XMLCh* xmlch_shadows
                     = currentElement->getAttribute(ATTR_shadows);
               scene->shadows = atoi(XMLString::transcode(xmlch_shadows));
            }

            if( XMLString::equals(currentElement->getTagName(), TAG_AreaLight))
            {
              AreaLight *alight = new AreaLight();

              const XMLCh* xmlch_LightPosX
                    = currentElement->getAttribute(ATTR_LightPosX);
              alight->pos[0] = atof(XMLString::transcode(xmlch_LightPosX));

              const XMLCh* xmlch_LightPosY
                    = currentElement->getAttribute(ATTR_LightPosY);
              alight->pos[1] = atof(XMLString::transcode(xmlch_LightPosY));

              const XMLCh* xmlch_LightPosZ
                    = currentElement->getAttribute(ATTR_LightPosZ);
              alight->pos[2] = atof(XMLString::transcode(xmlch_LightPosZ));

              const XMLCh* xmlch_LightColorR
                    = currentElement->getAttribute(ATTR_LightColorR);
              alight->color[0] = atof(XMLString::transcode(xmlch_LightColorR));

              const XMLCh* xmlch_LightColorG
                    = currentElement->getAttribute(ATTR_LightColorG);
              alight->color[1] = atof(XMLString::transcode(xmlch_LightColorG));

              const XMLCh* xmlch_LightColorB
                    = currentElement->getAttribute(ATTR_LightColorB);
              alight->color[2] = atof(XMLString::transcode(xmlch_LightColorB));

              const XMLCh* xmlch_AreaLightDirX
                    = currentElement->getAttribute(ATTR_AreaLightDirX);
              alight->dir[0] = atof(XMLString::transcode(xmlch_AreaLightDirX));

              const XMLCh* xmlch_AreaLightDirY
                    = currentElement->getAttribute(ATTR_AreaLightDirY);
              alight->dir[1] = atof(XMLString::transcode(xmlch_AreaLightDirY));

              const XMLCh* xmlch_AreaLightDirZ
                    = currentElement->getAttribute(ATTR_AreaLightDirZ);
              alight->dir[2] = atof(XMLString::transcode(xmlch_AreaLightDirZ));

	      const XMLCh* xmlch_AreaLightHeight
                    = currentElement->getAttribute(ATTR_AreaLightHeight);
              alight->height = atoi(XMLString::transcode(xmlch_AreaLightHeight));

              const XMLCh* xmlch_AreaLightWidth
                    = currentElement->getAttribute(ATTR_AreaLightWidth);
              alight->width = atoi(XMLString::transcode(xmlch_AreaLightWidth));

              const XMLCh* xmlch_AreaLightGridHeight
                    = currentElement->getAttribute(ATTR_AreaLightGridHeight);
              alight->gridHeight = atoi(XMLString::transcode(xmlch_AreaLightGridHeight));

              const XMLCh* xmlch_AreaLightGridWidth
                    = currentElement->getAttribute(ATTR_AreaLightGridWidth);
              alight->gridWidth = atoi(XMLString::transcode(xmlch_AreaLightGridWidth));

              const XMLCh* xmlch_AreaLightSampleDensity
                    = currentElement->getAttribute(ATTR_AreaLightSampleDensity);
              alight->sampleDensity = atoi(XMLString::transcode(xmlch_AreaLightSampleDensity));

	      alight->samples = (alight->gridHeight * alight->gridWidth * alight->sampleDensity);

              const XMLCh* xmlch_LightIntensity
                    = currentElement->getAttribute(ATTR_LightIntensity);
              alight->intensity = atof(XMLString::transcode(xmlch_LightIntensity));
	      alight->intensity /= alight->samples;

              const XMLCh* xmlch_LightDamping
                    = currentElement->getAttribute(ATTR_LightDamping);
              alight->damping = atof(XMLString::transcode(xmlch_LightDamping));

              scene->sceneLights.push_back(alight);
            }

            if( XMLString::equals(currentElement->getTagName(), TAG_OmniLight))
            {
              Omnidirectional *olight = new Omnidirectional();

              const XMLCh* xmlch_LightPosX
                    = currentElement->getAttribute(ATTR_LightPosX);
              olight->pos[0] = atof(XMLString::transcode(xmlch_LightPosX));

              const XMLCh* xmlch_LightPosY
                    = currentElement->getAttribute(ATTR_LightPosY);
              olight->pos[1] = atof(XMLString::transcode(xmlch_LightPosY));

              const XMLCh* xmlch_LightPosZ
                    = currentElement->getAttribute(ATTR_LightPosZ);
              olight->pos[2] = atof(XMLString::transcode(xmlch_LightPosZ));

              const XMLCh* xmlch_LightColorR
                    = currentElement->getAttribute(ATTR_LightColorR);
              olight->color[0] = atof(XMLString::transcode(xmlch_LightColorR));

              const XMLCh* xmlch_LightColorG
                    = currentElement->getAttribute(ATTR_LightColorG);
              olight->color[1] = atof(XMLString::transcode(xmlch_LightColorG));

              const XMLCh* xmlch_LightColorB
                    = currentElement->getAttribute(ATTR_LightColorB);
              olight->color[2] = atof(XMLString::transcode(xmlch_LightColorB));

              const XMLCh* xmlch_LightDamping
                    = currentElement->getAttribute(ATTR_LightDamping);
              olight->damping = atof(XMLString::transcode(xmlch_LightDamping));

              const XMLCh* xmlch_LightIntensity
                    = currentElement->getAttribute(ATTR_LightIntensity);
              olight->intensity = atof(XMLString::transcode(xmlch_LightIntensity));

              scene->sceneLights.push_back(olight);
            }
            if( XMLString::equals(currentElement->getTagName(), TAG_Sphere))
            {
              Sphere *s = new Sphere();
              const XMLCh* xmlch_PrimitivePosX
                    = currentElement->getAttribute(ATTR_PrimitivePosX);
              s->pos[0] = atof(XMLString::transcode(xmlch_PrimitivePosX));

              const XMLCh* xmlch_PrimitivePosY
                    = currentElement->getAttribute(ATTR_PrimitivePosY);
              s->pos[1] = atof(XMLString::transcode(xmlch_PrimitivePosY));

              const XMLCh* xmlch_PrimitivePosZ
                    = currentElement->getAttribute(ATTR_PrimitivePosZ);
              s->pos[2] = atof(XMLString::transcode(xmlch_PrimitivePosZ));

              const XMLCh* xmlch_SphereRadius
                    = currentElement->getAttribute(ATTR_SphereRadius);
              s->radius = atof(XMLString::transcode(xmlch_SphereRadius));
              s->sqrRadius = s->radius*s->radius;

              Material *m = new Material();

              const XMLCh* xmlch_MaterialColorR
                    = currentElement->getAttribute(ATTR_MaterialColorR);
              m->color[0] = atof(XMLString::transcode(xmlch_MaterialColorR));

              const XMLCh* xmlch_MaterialColorG
                    = currentElement->getAttribute(ATTR_MaterialColorG);
              m->color[1] = atof(XMLString::transcode(xmlch_MaterialColorG));

              const XMLCh* xmlch_MaterialColorB
                    = currentElement->getAttribute(ATTR_MaterialColorB);
              m->color[2] = atof(XMLString::transcode(xmlch_MaterialColorB));

              const XMLCh* xmlch_MaterialOpacy
                    = currentElement->getAttribute(ATTR_MaterialOpacy);
              m->opacy = atof(XMLString::transcode(xmlch_MaterialOpacy));

              const XMLCh* xmlch_MaterialFilter
                    = currentElement->getAttribute(ATTR_MaterialFilter);
              m->filter = atof(XMLString::transcode(xmlch_MaterialFilter));

              const XMLCh* xmlch_MaterialAmbient
                    = currentElement->getAttribute(ATTR_MaterialAmbient);
              m->ambient = atof(XMLString::transcode(xmlch_MaterialAmbient));

              const XMLCh* xmlch_MaterialDiffuse
                    = currentElement->getAttribute(ATTR_MaterialDiffuse);
              m->diffuse = atof(XMLString::transcode(xmlch_MaterialDiffuse));

              const XMLCh* xmlch_MaterialSpecular
                    = currentElement->getAttribute(ATTR_MaterialSpecular);
              m->specular = atof(XMLString::transcode(xmlch_MaterialSpecular));

              const XMLCh* xmlch_MaterialSpecHardness
                    = currentElement->getAttribute(ATTR_MaterialSpecHardness);
              m->specular_Hardness = (atof(XMLString::transcode(xmlch_MaterialSpecHardness)))*8;

              const XMLCh* xmlch_MaterialReflection
                    = currentElement->getAttribute(ATTR_MaterialReflection);
              m->reflection = atof(XMLString::transcode(xmlch_MaterialReflection));

              const XMLCh* xmlch_MaterialInterior
                    = currentElement->getAttribute(ATTR_MaterialInterior);
              m->interior = atof(XMLString::transcode(xmlch_MaterialInterior));
              s->material = m;

              scene->sceneMaterials.push_back(m);
              scene->sceneObjects.push_back(s);
            }

            if( XMLString::equals(currentElement->getTagName(), TAG_Plane))
            {
              Plane *p = new Plane();
              const XMLCh* xmlch_PrimitivePosX
                    = currentElement->getAttribute(ATTR_PrimitivePosX);
              p->pos[0] = atof(XMLString::transcode(xmlch_PrimitivePosX));
              const XMLCh* xmlch_PrimitivePosY
                    = currentElement->getAttribute(ATTR_PrimitivePosY);
              p->pos[1] = atof(XMLString::transcode(xmlch_PrimitivePosY));
              const XMLCh* xmlch_PrimitivePosZ
                    = currentElement->getAttribute(ATTR_PrimitivePosZ);
              p->pos[2] = atof(XMLString::transcode(xmlch_PrimitivePosZ));
              const XMLCh* xmlch_PlaneNormalX
                    = currentElement->getAttribute(ATTR_PlaneNormalX);
              p->planeN[0] = atof(XMLString::transcode(xmlch_PlaneNormalX));
              const XMLCh* xmlch_PlaneNormalY
                    = currentElement->getAttribute(ATTR_PlaneNormalY);
              p->planeN[1] = atof(XMLString::transcode(xmlch_PlaneNormalY));
              const XMLCh* xmlch_PlaneNormalZ
                    = currentElement->getAttribute(ATTR_PlaneNormalZ);
              p->planeN[2] = atof(XMLString::transcode(xmlch_PlaneNormalZ));

              Material *m = new Material();

              const XMLCh* xmlch_MaterialColorR
                    = currentElement->getAttribute(ATTR_MaterialColorR);
              m->color[0] = atof(XMLString::transcode(xmlch_MaterialColorR));

              const XMLCh* xmlch_MaterialColorG
                    = currentElement->getAttribute(ATTR_MaterialColorG);
              m->color[1] = atof(XMLString::transcode(xmlch_MaterialColorG));

              const XMLCh* xmlch_MaterialColorB
                    = currentElement->getAttribute(ATTR_MaterialColorB);
              m->color[2] = atof(XMLString::transcode(xmlch_MaterialColorB));

              const XMLCh* xmlch_MaterialOpacy
                    = currentElement->getAttribute(ATTR_MaterialOpacy);
              m->opacy = atof(XMLString::transcode(xmlch_MaterialOpacy));

              const XMLCh* xmlch_MaterialFilter
                    = currentElement->getAttribute(ATTR_MaterialFilter);
              m->filter = atof(XMLString::transcode(xmlch_MaterialFilter));

              const XMLCh* xmlch_MaterialAmbient
                    = currentElement->getAttribute(ATTR_MaterialAmbient);
              m->ambient = atof(XMLString::transcode(xmlch_MaterialAmbient));

              const XMLCh* xmlch_MaterialDiffuse
                    = currentElement->getAttribute(ATTR_MaterialDiffuse);
              m->diffuse = atof(XMLString::transcode(xmlch_MaterialDiffuse));

              const XMLCh* xmlch_MaterialSpecular
                    = currentElement->getAttribute(ATTR_MaterialSpecular);
              m->specular = atof(XMLString::transcode(xmlch_MaterialSpecular));

              const XMLCh* xmlch_MaterialSpecHardness
                    = currentElement->getAttribute(ATTR_MaterialSpecHardness);
              m->specular_Hardness = atof(XMLString::transcode(xmlch_MaterialSpecHardness));

              const XMLCh* xmlch_MaterialReflection
                    = currentElement->getAttribute(ATTR_MaterialReflection);
              m->reflection = atof(XMLString::transcode(xmlch_MaterialReflection));

              const XMLCh* xmlch_MaterialInterior
                    = currentElement->getAttribute(ATTR_MaterialInterior);
              m->interior = atof(XMLString::transcode(xmlch_MaterialInterior));
              p->material = m;

              scene->sceneMaterials.push_back(m);
              scene->sceneObjects.push_back(p);
            }
         }
      }
   }
   catch( xercesc::XMLException& e )
   {
    char* message = xercesc::XMLString::transcode( e.getMessage() );
    ostringstream errBuf;
    errBuf << "Error parsing file: " << message << flush;
    XMLString::release( &message );
   }
}
