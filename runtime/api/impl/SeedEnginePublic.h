// * ||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
// * illegal driver
//1日
// * File: SeedEnginePublic.h
// *
// * Author SeedEnginer
// *
// * Desc:
// * ||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

#ifndef _SEED_ENGINE_PUBLIC_H_INCLUDE
#define _SEED_ENGINE_PUBLIC_H_INCLUDE

#include "runtime/api/IType.h"

#pragma region WIN32

#ifndef _WIN32_WINNT
#define _WIN32_WINNT 0x0501
#endif

#ifndef ASSERT
#include <assert.h>
#define ASSERT(x)	assert(x)
#endif

#ifdef WIN32
#   define WIN32_LEAN_AND_MEAN
#   include <windows.h>
#endif

#pragma warning(disable:4819)
#pragma warning(disable:4996)

#include <stddef.h>

#ifdef WIN32
#define FUNCTION __FUNCTION__
#else
#define FUNCTION __PRETTY_FUNCTION__
#endif

#ifdef _MSC_VER
#define TEMP_DISABLE_WARNING(warningCode, expression)   \
			__pragma(warning(push))                             \
			__pragma(warning(disable:warningCode))              \
			expression                                          \
			__pragma(warning(pop))
#else
#define TEMP_DISABLE_WARNING(warningCode, expression)   \
			expression
#endif

#pragma endregion

#pragma region ENGINE_DEFINE

#if _MSC_VER == 1900
#define ENGINE_BLANKSTRING Ogre::BLANKSTRING
#else
#define ENGINE_BLANKSTRING ""
#endif

#define ENGINE_ERROR(Condition) { if (!(Condition)) goto Exit0; }
#define ENGINE_SUCCESS(Condition) { if (Condition)  goto Exit1; }

#define ENGINE_RELEASE(pInterface)  \
{									\
	if (pInterface)                 \
	{                               \
		(pInterface)->Release();    \
		(pInterface) = NULL;        \
	}								\
}

#define ENGINE_DELETE_ARRAY(pArray) \
    {								\
        if (pArray)                 \
        {                           \
            delete [](pArray);      \
            (pArray) = NULL;        \
        }                           \
    }


#define ENGINE_DELETE(p)			\
    {								\
        if (p)						\
        {							\
            delete (p);				\
            (p) = NULL;				\
        }							\
    }

#define ENGINE_UNINIT_DELETE(pInterface) \
	{									 \
		if (pInterface)                  \
		{                                \
			(pInterface)->UnInit();      \
			delete (pInterface);         \
			(pInterface) = NULL;         \
		}                                \
	}

#pragma endregion

#pragma region Define_Type

typedef unsigned char	BYTE;
typedef unsigned short	WORD;
typedef unsigned long	DWORD;

typedef unsigned long	ULONG;
typedef unsigned short	USHORT;
typedef unsigned char	UCHAR;

#if _MSC_VER >= 1200        // >= VS 6.0

#if  _MSC_VER < 1800
typedef __int8              int8_t;
#endif

typedef __int16             int16_t;
typedef __int32             int32_t;
typedef __int64             int64_t;

typedef unsigned __int8     uint8_t;
typedef unsigned __int16    uint16_t;
typedef unsigned __int32    uint32_t;
typedef unsigned __int64    uint64_t;

#if defined(_WIN64)

typedef __int64             intptr_t;
typedef unsigned __int64    uint32_tptr_t;

#else

#if !defined(_W64)
#if !defined(__midl) && (defined(_X86_) || defined(_M_IX86)) && _MSC_VER >= 1300
#define _W64 __w64
#else
#define _W64
#endif
#endif

typedef _W64 __int32             intptr_t;
typedef _W64 unsigned __int32    uint32_tptr_t;
#endif

#endif //_MSC_VER

#ifndef _MSC_VER
typedef unsigned long long    uint32_t64_t;
#endif

#if _MSC_VER >= 1200            // >= VS 6.0
#   define LITTLE_ENDIAN	1234
#   define BIG_ENDIAN	    4321
#   define PDP_ENDIAN	    3412
#   define BYTE_ORDER        LITTLE_ENDIAN
#else
#   include <endian.h>
#   define LITTLE_ENDIAN     __LITTLE_ENDIAN
#   define BIG_ENDIAN        __BIG_ENDIAN
#   define PDP_ENDIAN        __PDP_ENDIAN
#   define BYTE_ORDER        __BYTE_ORDER
#endif

#ifndef MAX_PATH
#   define MAX_PATH 260
#endif

#ifndef NULL
#   define	NULL	0
#endif

#ifndef VOID
#   define	VOID	void
#endif

#ifndef TRUE
#   define TRUE	    1
#endif

#ifndef FALSE
#   define FALSE	0
#endif

#pragma endregion

#pragma region Shader

enum ShaderType
{
	ST_DEFAULT = 0,
	ST_FLOAT = 1,
	ST_FLOAT3 = 3,
	ST_FLOAT4 = 4,
	ST_TEXTURE = 5,
	ST_INT = 6,
};

struct NeedUpdateParam
{
	std::string mParamName;
	std::string mStartValue;
	std::string mEndValue;
	unsigned int mParamType;
public:
	NeedUpdateParam()
	{
		mParamName = "";
		mParamType = 1;
	}
};

typedef std::vector<NeedUpdateParam>  ShaderNeedUpdateParamVector;

struct ShaderParam
{
public:
	ShaderParam()
	{
		valueName = "";
		valueStr = "";
		valueType = ST_DEFAULT;
		valueIndex = 0;
	}
	~ShaderParam()
	{
		valueName = "";
		valueStr = "";
		valueType = ST_DEFAULT;
		valueIndex = 0;
	}
	std::string valueName;
	std::string valueStr;
	ShaderType valueType;
	int        valueIndex;

};

typedef std::vector<ShaderParam>  ShaderParamVector;
struct SHADER_PARAM_MODEL
{
public:
	SHADER_PARAM_MODEL()
	{
		mfTime = 0.0;	   //s
		mName = "";
		sVertexProgramName = "";
		sFragmentProgramName = "";
		vParamVector.clear();
		fParamVector.clear();
		fParamTextureVector.clear();
		mLoaded = false;
		mDelayRemove = false;
		mRenderGroup = 50;
		mOldRenderGroup = 50;
		mApplyStr = "";
		mUpdateTime = 0.0f;
	}

	~SHADER_PARAM_MODEL()
	{
		clear();
	}
	void clear()
	{
		mLoaded = false;
		mfTime = 0.0;	   //s
		mName = "";
		sVertexProgramName = "";
		sFragmentProgramName = "";
		vParamVector.clear();
		fParamVector.clear();
		fParamTextureVector.clear();
		mDelayRemove = false;
		mCastshadow = true;
		mRenderGroup = 50;
		mOldRenderGroup = 50;
		mApplyStr = "";
		mUpdateTime = 0.0f;
	}
	bool mCastshadow;
	bool  mDelayRemove;
	bool mLoaded;
	int                  nId;
	std::string          mName;
	std::string          sVertexProgramName;
	std::string          sFragmentProgramName;
	float                mfTime;
	unsigned int         mRenderGroup;
	unsigned int         mOldRenderGroup;
	float                mUpdateTime;
	std::string          mApplyStr;

	ShaderParamVector    vParamVector;  //vertex_program
	ShaderParamVector    fParamVector;  //fragment_program
	ShaderParamVector    fParamTextureVector; //fragment_program texture
	ShaderNeedUpdateParamVector mUpdaeParam;
};
#pragma endregion

enum LightMaskFlags
{
	LMF_DEFAULT		= 0xFFFFFFFF,
	LMF_SCENE		= 1 << 0,
	LMF_DYNMODEL	= 1 << 1,
	LMF_OTHERFILTER	= 1 << 2,
};

enum VisibilityFlags
{
	VF_POSTFILTER = 1 << 0,
	VF_INDICATORS = 1 << 1,
	VF_GUI_ELEMENTS = 1 << 2,
	VF_DEFAULT = ~0 << 3,
};

enum QueryFlags
{
	EQF_DECAL_TARGET = 1 << 0,
	EQF_COLLIDEABLE = 1 << 1,
	EQF_BLOCKHIDE = 1 << 2,
	EQF_DEFAULT = ~0 << 3,
};

struct EngineConfigOption
{
	EngineConfigOption();
public: //coifig
	uint32_t bEngineConfigId;

	BOOL bEnableDownload;
	BOOL bAnimation;
	BOOL bStramEnable;
	BOOL bEnableShadow;
	BOOL bShakeCamera;

	uint32_t bGressDensity;
	Real fParticleLevel;
	uint32_t fDynamicObjectCount;

	uint32_t uTextureFilter;
	uint32_t uTerrainLevel;
	uint32_t uWaterLevel;

	BOOL   bShowUI;
	BOOL   bSound;

	BOOL bStopRibbon;

	BOOL bUpdateEngineOnce;
public:
	BOOL bNoLightMap;
	BOOL bLightTextureDownloaded;
	BOOL bEnableFog;
	BOOL bShowParticle;
	BOOL bRendering;
	
	BOOL bProfile;

	BOOL bResourceLoaded;
	Real fResourceLoadPercentage;

	Real fFarClipDistance;

	BOOL bGlobalCreateDynObject;
	BOOL bGlobalCreateEffect;
	BOOL bGlobalCreateSkill;

	BOOL bShowAnimationLog;
	BOOL bShowDownloadlog;

	std::string sFileSystemName;
};

extern "C" EngineConfigOption g_EngineOption;

#include "ISeed.h"

extern "C" seed_init_params   g_init_params;

#endif // _SEED_ENGINE_PUBLIC_H_INCLUDE
