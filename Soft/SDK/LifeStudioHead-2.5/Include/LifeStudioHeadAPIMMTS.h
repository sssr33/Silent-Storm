#ifndef _LIFESTUDIOHEADAPIMMTS_H_
#define _LIFESTUDIOHEADAPIMMTS_H_
/*
####################################################################
##                                                                ##
##                    LifeStudio: Head API                        ##
##                 MMTree & Sequencer Interfaces                  ##
##                                                                ##
##           (c) 2001..2003, LifeMode Interactive, Corp.          ##
##                    Author: Alexander Strakhov                  ##
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
//                 Macro muscle tree interface
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

struct IMMTree
{
  //Load macro muscle library
  virtual bool LIFESTUDIOHEADAPICALL	Load(const char *file_name) = 0;
  virtual bool LIFESTUDIOHEADAPICALL	   Load(const char *buffer, int size) = 0;

  //Get root macro muscle
  virtual IMacroMuscle *	LIFESTUDIOHEADAPICALL	RootMacroMuscle() const = 0;
  //Find macro muscle by name
  virtual IMacroMuscle *	LIFESTUDIOHEADAPICALL	FindMacroMuscle(const char *name) = 0;

  //Destroy object
  virtual void LIFESTUDIOHEADAPICALL	Destroy() = 0;

  //Create object
  static LIFESTUDIOHEADAPI_API IMMTree * LIFESTUDIOHEADAPICALL	Create();
};

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//                    Sequencer interface
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

struct ISequencer;

//Item flags
#define SEQ_ITEM_SELECTED_FLAG	  	0x01
#define SEQ_ITEM_PREEMPTIVE_FLAG	  0x10

//Enumeration callbacks
//Return NULL to continue enumeration, stop otherwise
typedef void * 	(LIFESTUDIOHEADAPICALL	*MUSCLE_CB)(ISequencer *sa, IMacroMuscle *muscle, int flags, void *user_data);
typedef void *	(LIFESTUDIOHEADAPICALL	*MUSCLE_EXPR_CB)(ISequencer *sa, IMacroMuscle *muscle, float expression, int track, int flags, void *user_data);

typedef void *	(LIFESTUDIOHEADAPICALL	*MUSCLE_NAME_CB)(ISequencer *sa, const char *clip, int flags, void *user_data);
typedef void *	(LIFESTUDIOHEADAPICALL	*MUSCLE_NAME_EXPR_CB)(ISequencer *sa, const char *clip, float expression, int track, int flags, void *user_data);

typedef void *	(LIFESTUDIOHEADAPICALL	*SOUND_CB)(ISequencer *sa, const char *file_name, int start_time, int flags, void *user_data);
typedef void *	(LIFESTUDIOHEADAPICALL	*SOUND_TIME_CB)(ISequencer *sa, const char *file_name, int start_time, int time_offset, int track, int flags, void *user_data);

struct ISequencer
{
  //Load sequence
  virtual bool LIFESTUDIOHEADAPICALL	Load(const char *file_name) = 0;
  virtual bool LIFESTUDIOHEADAPICALL	Load(const char *buffer, int size) = 0;

  //Set macro muscle tree for sequence, return previous
  virtual IMMTree *	LIFESTUDIOHEADAPICALL	RegisterMMTree(IMMTree *tree=0) = 0;

  //Get sequence duration (milliseconds)
  virtual int LIFESTUDIOHEADAPICALL	SequenceTime() const = 0;

  //Get number of tracks in sequence
  virtual int LIFESTUDIOHEADAPICALL	ChannelsCount() const = 0;

  //Enumerate all macro muscles in sequence, return count
  virtual int LIFESTUDIOHEADAPICALL	EnumerateMacroMuscles(MUSCLE_CB cb, void *user_data=0) = 0;

  //Enumerate sequence macro muscles at given time, return count
  virtual int LIFESTUDIOHEADAPICALL	EnumerateMacroMuscles(int time, MUSCLE_EXPR_CB cb, void *user_data=0) = 0;

  //Enumerate all macro muscles in sequence, return count
  virtual int LIFESTUDIOHEADAPICALL	EnumerateMacroMuscles(MUSCLE_NAME_CB cb, void *user_data=0) = 0;

  //Enumerate sequence macro muscles at given time, return count
  virtual int LIFESTUDIOHEADAPICALL	EnumerateMacroMuscles(int time, MUSCLE_NAME_EXPR_CB cb, void *user_data=0) = 0;

  //Enumerate all sounds in sequence, return count
  virtual int LIFESTUDIOHEADAPICALL	EnumerateSounds(SOUND_CB cb, void *user_data=0) = 0;

  //Enumerate sequence sounds at given time, return count
  virtual int LIFESTUDIOHEADAPICALL	EnumerateSounds(int time, SOUND_TIME_CB cb, void *user_data=0) = 0;

  //Render macro muscles at given time
  virtual void LIFESTUDIOHEADAPICALL	RenderMacroMuscles(IAnimator *animator, int time) = 0;

  //Destroy object
  virtual void  LIFESTUDIOHEADAPICALL	Destroy() = 0;

  //Create object
  static LIFESTUDIOHEADAPI_API ISequencer *	LIFESTUDIOHEADAPICALL Create();
};

};

#endif
