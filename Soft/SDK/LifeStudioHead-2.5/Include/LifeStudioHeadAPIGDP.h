#ifndef _LIFESTUDIOHEADAPIGDP_H_
#define _LIFESTUDIOHEADAPIGDP_H_

/*
####################################################################
##                                                                ##
##                    LifeStudio: Head API                        ##
##                 GDP File Interface Header                      ##
##                                                                ##
##           (c) 2001..2003, LifeMode Interactive, Corp.          ##
##                    Author: Yuri Golubev                        ##
##                                                                ##
####################################################################
*/

#ifdef LIFESTUDIOHEADAPI_EXPORTS_LIB
#define LIFESTUDIOHEADAPI_API
#else
#ifdef LIFESTUDIOHEADAPI_EXPORTS
#define LIFESTUDIOHEADAPI_API __declspec(dllexport)
#else
#define LIFESTUDIOHEADAPI_API __declspec(dllimport)
#endif
#endif

#include "LifeStudioHeadAPITransform.h"

namespace LifeStudioHeadAPI
{

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//                    Material structure
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

#define LIFESTUDIOHEADAPI_MATERIAL_WRAPU       0x0001
#define LIFESTUDIOHEADAPI_MATERIAL_WRAPV       0x0002
#define LIFESTUDIOHEADAPI_MATERIAL_UVCHG       0x0004
#define LIFESTUDIOHEADAPI_MATERIAL_BLEND       0x0008
#define LIFESTUDIOHEADAPI_MATERIAL_TRANPARENT  0x0010
#define LIFESTUDIOHEADAPI_MATERIAL_DOUBLESIDED 0x0020

struct ObjectMaterial
{
  char name[32];
  float diffuse[4]; 
  float ambient[4]; 
  float specular[4]; 
  float emission[4];
  float shininess;
  unsigned int flags;
  char textureName[32]; // == "" if material has no texture
};

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//                    Graphics object interface
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
struct IGDPObject: public ITransformerInput
{
  //Get graphics data
  virtual int LIFESTUDIOHEADAPICALL MaterialsCount() const = 0;//return -1 if failed
  virtual bool LIFESTUDIOHEADAPICALL Material(int materialNumber, ObjectMaterial& material) const = 0;
  virtual int LIFESTUDIOHEADAPICALL TrianglesCount(int materialNumber) const = 0;//return -1 if failed
  virtual const unsigned short *LIFESTUDIOHEADAPICALL Triangulation(int materialNumber) const = 0;

  virtual int LIFESTUDIOHEADAPICALL VerticesCount() const = 0;//return -1 if failed
  virtual const float *LIFESTUDIOHEADAPICALL UV() const = 0;
  virtual const float *LIFESTUDIOHEADAPICALL UVNoChg() const = 0;

  //Extended UV information
  virtual bool LIFESTUDIOHEADAPICALL HasExtenedUVInfo() const = 0;
  virtual int LIFESTUDIOHEADAPICALL UVCount() const = 0;//return -1 if failed
  virtual int LIFESTUDIOHEADAPICALL BaseTrianglesCount() const = 0;//return -1 if failed
  virtual const unsigned short *LIFESTUDIOHEADAPICALL BaseTriangulation() const = 0;
  virtual const unsigned short *LIFESTUDIOHEADAPICALL UV2VMap() const = 0;

  //Extended normals data
  virtual int LIFESTUDIOHEADAPICALL AdditionalNormalsDataSize() const = 0;//return -1 if no additional data
  virtual bool LIFESTUDIOHEADAPICALL AdditionalNormalsData(char * buffer) = 0;
  
  //Textures
  virtual int LIFESTUDIOHEADAPICALL PNGTextureSize(const char *textureName) const = 0;//return -1 if failed
  virtual bool LIFESTUDIOHEADAPICALL PNGTexture(const char *textureName, char * buffer) = 0;

  //GDP object mode
  virtual bool LIFESTUDIOHEADAPICALL IsTransformable() const = 0;

  //Get data name list for ITransformer interface
  virtual int LIFESTUDIOHEADAPICALL DataListSize() const = 0;//return -1 if failed
  virtual const char *LIFESTUDIOHEADAPICALL DataListItem(int itemNumber) const = 0;

  //Default animator data access
  virtual int LIFESTUDIOHEADAPICALL DefaultAnimatorDataSize() const = 0;//return -1 if failed
  virtual bool LIFESTUDIOHEADAPICALL DefaultAnimatorData(char * buffer) = 0;

  //Get sub objects
  virtual int LIFESTUDIOHEADAPICALL SubObjectsCount() const = 0; //return -1 if failed
  virtual const char *LIFESTUDIOHEADAPICALL SubObjectName(int number) const = 0;
  virtual const char *LIFESTUDIOHEADAPICALL SubObjectType(int number) const = 0;
  virtual IGDPObject *LIFESTUDIOHEADAPICALL SubObject(int number) = 0; //return NULL if failed

  //Destroy GDPObject
  virtual void LIFESTUDIOHEADAPICALL Destroy() = 0;
};

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//                       GDP file interface
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
struct IGDPFile
{
  //Get objects
  virtual int LIFESTUDIOHEADAPICALL ObjectsCount() const = 0; //return -1 if failed
  virtual const char *LIFESTUDIOHEADAPICALL ObjectName(int number) const = 0;
  virtual IGDPObject *LIFESTUDIOHEADAPICALL Object(int number) = 0; //return NULL if failed

  //Destroy GDPFile object and close GDP file
  virtual void LIFESTUDIOHEADAPICALL Destroy() = 0;

  //Create GDPFile object
  static LIFESTUDIOHEADAPI_API IGDPFile *LIFESTUDIOHEADAPICALL Create(const char *filename);
};

};



#endif