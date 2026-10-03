#ifndef _LIFESTUDIOHEADAPITRANSFORM_H_
#define _LIFESTUDIOHEADAPITRANSFORM_H_
/*
####################################################################
##                                                                ##
##                    LifeStudio: Head API                        ##
##                   Transformations Interface                    ##
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

#include "LifeStudioHeadAPI.h"

namespace LifeStudioHeadAPI
{

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//               Transformation input data interface
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

struct ITransformerInput
{
  virtual int LIFESTUDIOHEADAPICALL Size(const char *name) = 0;         //-1 if error (not found)
  virtual bool LIFESTUDIOHEADAPICALL Get(const char *name, char * buffer) = 0;
};


//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//               Transformation interface
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

struct ITransformer: public IAnimator
{
  //Load transformer
  virtual bool LIFESTUDIOHEADAPICALL Load(ITransformerInput *input) = 0;

  //Set/get output animator
  virtual void LIFESTUDIOHEADAPICALL OutputAnimator(IAnimator *animator) = 0;
  virtual IAnimator *LIFESTUDIOHEADAPICALL OutputAnimator() const = 0;

  //Fill output animator
  virtual void LIFESTUDIOHEADAPICALL Generate() = 0;

  //Create transformer object
  static LIFESTUDIOHEADAPI_API ITransformer *LIFESTUDIOHEADAPICALL Create();
};

};

#endif
