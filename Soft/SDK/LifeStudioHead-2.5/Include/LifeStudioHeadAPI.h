#ifndef _LIFESTUDIOHEADAPI_H_
#define _LIFESTUDIOHEADAPI_H_
/*
####################################################################
##                                                                ##
##                    LifeStudio: Head API                        ##
##                    Main Interface Header                       ##
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

#ifndef LIFESTUDIOHEADAPICALL
#define LIFESTUDIOHEADAPICALL __stdcall
#endif

namespace LifeStudioHeadAPI
{
//MacroItem types
enum ITEMTYPE
{
  ITEM_MUSCLE,
  ITEM_BONE_YAW,
  ITEM_BONE_PITCH,
  ITEM_BONE_ROLL,
  ITEM_MACROMUSCLE,
  ITEM_USER,
  ITEM_RESERVED,
};

//Matrix types
enum MATRIXTYPE
{
  MATRIXTYPE_FULL = 0,     // 16 float values (full 3D-graphics matrix)
  MATRIXTYPE_NOSCALE,  // 12 float values (full matrix without last column)
  MATRIXTYPE_ANGLES,   //  9 float values (rotation part of graphics king matrix)
};

//User item ID type
typedef int UserID;

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//               Object & muscle phycics interfaces
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

struct IMuscle
{
  //Get muscle name
  virtual const char *LIFESTUDIOHEADAPICALL Name() const = 0;

  //Set/Get muscle tension
  virtual void LIFESTUDIOHEADAPICALL Squeeze(float value) = 0;
  virtual float LIFESTUDIOHEADAPICALL Squeeze() const = 0;
};


//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//                    Bone physics interface
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

struct IBone
{
  //Get bone name
  virtual const char *LIFESTUDIOHEADAPICALL Name() const = 0;

  //Set/get relative bone matrix (rotation part matrix of any kind is only used)
  virtual void LIFESTUDIOHEADAPICALL BoneMatrix(float *matrix, MATRIXTYPE matrixType) = 0;
  virtual const float *LIFESTUDIOHEADAPICALL BoneMatrix() const = 0; //returned matrix is in MATRIXTYPE_FULL format

  //Get initial & result bone matrix in world coordinates without hierarchy trasformations
  //(used rotation and transform parts of matrix)
  virtual const float *LIFESTUDIOHEADAPICALL InitialBoneMatrix() const = 0; //returned matrix is in MATRIXTYPE_FULL format
  virtual const float *LIFESTUDIOHEADAPICALL ResultBoneMatrix() const = 0;  //returned matrix is in MATRIXTYPE_FULL format

  //Get initial & result bone matrix in world coordinates with hierarchy trasformations
  //Valid only after IAnimator::ComputeBonesHierarchy()/ComputePhysics()/Process() call
  virtual const float *LIFESTUDIOHEADAPICALL InitialBoneMatrixTr() const = 0; //returned matrix is in MATRIXTYPE_FULL format
  virtual const float *LIFESTUDIOHEADAPICALL ResultBoneMatrixTr() const = 0;  //returned matrix is in MATRIXTYPE_FULL format

  //Get parent bone
  virtual IBone *LIFESTUDIOHEADAPICALL Parent() const = 0;

  //Set/get angles to/from relative matrix
  //If relative matrix was set by BoneMatrix(), then Yaw(), Pitch(), Roll() returns are invalid
  virtual void LIFESTUDIOHEADAPICALL Angles(float yaw, float pitch, float roll) = 0;
  virtual float LIFESTUDIOHEADAPICALL Yaw() const = 0;
  virtual float LIFESTUDIOHEADAPICALL Pitch() const = 0;
  virtual float LIFESTUDIOHEADAPICALL Roll() const = 0;

  //Get flags
  virtual unsigned long LIFESTUDIOHEADAPICALL Type() const = 0;
};

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//                     Macro muscle interface
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

struct IMacroMuscle
{
  //Get macro muscle name
  virtual const char *LIFESTUDIOHEADAPICALL Name() const = 0;

  //Enable/disable mixing
  virtual void LIFESTUDIOHEADAPICALL Enable (bool enable) = 0;
  virtual bool LIFESTUDIOHEADAPICALL Enabled() const = 0;
};


//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//                      Animator interface
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

struct IAnimator
{
  //Load animator from buffer
  virtual bool LIFESTUDIOHEADAPICALL Load(const char *buffer, int sizeOfBuffer) = 0;

  //Save animator data
  virtual int LIFESTUDIOHEADAPICALL SaveBufferSize() = 0;
  virtual bool LIFESTUDIOHEADAPICALL Save(char *buffer) = 0;

  //Get muscle info
  virtual IMuscle *LIFESTUDIOHEADAPICALL MuscleByName(const char *name) = 0;
  virtual IMuscle *LIFESTUDIOHEADAPICALL Muscle(int number) = 0;
  virtual int LIFESTUDIOHEADAPICALL MusclesCount() const = 0;

  //Get bone info
  virtual IBone *LIFESTUDIOHEADAPICALL BoneByName(const char *name) = 0;
  virtual IBone *LIFESTUDIOHEADAPICALL Bone(int number) = 0;
  virtual IBone *LIFESTUDIOHEADAPICALL BoneByType(unsigned long type, IBone *prev = 0) = 0;
  virtual int LIFESTUDIOHEADAPICALL BonesCount() const = 0;

  //Process mode
  virtual void LIFESTUDIOHEADAPICALL FillUnused(bool fill) = 0;
  virtual bool LIFESTUDIOHEADAPICALL FillUnused() const = 0;

  //Process mesh
  virtual bool LIFESTUDIOHEADAPICALL Process(float *vertexArray, int step) = 0;           //step in 4 bytes
  
  //Vertices count info
  virtual int LIFESTUDIOHEADAPICALL VerticesCount() const = 0;
  
  //Macro muscles support
  virtual void LIFESTUDIOHEADAPICALL ClearAllMacroMuscles() = 0;
  virtual void LIFESTUDIOHEADAPICALL AddMacroMuscle(IMacroMuscle *muscle, float expression) = 0;
  virtual void LIFESTUDIOHEADAPICALL MultMacroMuscle(IMacroMuscle *muscle, float expression) = 0;
  virtual void LIFESTUDIOHEADAPICALL ComputePhysics() = 0;

  //Register macro muscle. You must once register macro muscle before call AddMacroMuscle().
  virtual void LIFESTUDIOHEADAPICALL RegisterMacroMuscle(IMacroMuscle *muscle) = 0;
  //Unregister macro muscle.
  virtual void LIFESTUDIOHEADAPICALL UnregisterMacroMuscle(IMacroMuscle *muscle) = 0;
  //Unregister all macro muscles
  virtual void LIFESTUDIOHEADAPICALL ClearAllRegistration() = 0;

  //User items support
  //  User item collection mode support
  virtual void LIFESTUDIOHEADAPICALL CollectUserItems(bool use) = 0;
  virtual bool LIFESTUDIOHEADAPICALL CollectUserItems() const = 0;
  //  Get user item values
  virtual UserID LIFESTUDIOHEADAPICALL UserItem(const char *itemName) = 0;//return -1 if item not found
  virtual int LIFESTUDIOHEADAPICALL UserValuesCount(UserID id) = 0;//return -1 if id is invalid
  virtual float LIFESTUDIOHEADAPICALL UserValue(UserID id, int number) = 0;//return value is in [-1,1]
                                                     //if not then id or number are invalid
  //Clear user item IDs
  virtual void LIFESTUDIOHEADAPICALL ClearUserItems() = 0;

  //Bones hierachy support
  virtual void LIFESTUDIOHEADAPICALL ComputeBonesHierarchy() = 0;

  //Neck support
  virtual bool LIFESTUDIOHEADAPICALL HasNeck() const = 0;

  //Additional neck processing mode
  //If additional mode is enabled muscles can affect the neck area
  virtual void LIFESTUDIOHEADAPICALL NeckProcessing2(bool enable) = 0;
  virtual bool LIFESTUDIOHEADAPICALL NeckProcessing2() const = 0;

  //Flags (see LIFESTUDIOHEADAPI_ANIMATORFLAG_*)
  virtual unsigned int LIFESTUDIOHEADAPICALL Flags() = 0;

  //Destroy Animator object
  virtual void LIFESTUDIOHEADAPICALL Destroy() = 0;

  //Create Animator object
  static LIFESTUDIOHEADAPI_API IAnimator *LIFESTUDIOHEADAPICALL Create();
};

};

#endif
