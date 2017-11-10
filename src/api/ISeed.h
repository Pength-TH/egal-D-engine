// * ||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
// * illegal driver
//2日
// * File: ISeed.h
// *
// * Author SeedEnginer
// * 
// * Desc:
// * ||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
#ifndef SEED_ISEED_H_INCLUDE
#define SEED_ISEED_H_INCLUDE

#include "IType.h"
#include "IScene.h"
#include "IGameObject.h"
#include "ICamera.h"
#include "IApplication.h"
#include "IFileSystem.h"
#include "ISound.h"

typedef void (* CallBack_On_Resource_Loaded)();

struct seed_init_params
{
public:
	seed_init_params() :
	  hWnd(0)
	, bWebPage(FALSE)
	, bOnlyEngine(FALSE)
	, szLogFileName(NULL)
	, szResourceLoadFile(NULL)

#if !DISABLE_FILE_AND_UI
	, pFileDownload(NULL)
	, pFileList(NULL)
	, pSoundFileOpt(NULL)
#endif
	{
	}

public:
	HWND hWnd;
	BOOL bWebPage;
	BOOL bD3d9_IE;
	BOOL bOnlyEngine;

	LPCTSTR szLogFileName;
	LPCTSTR szResourceLoadFile;
	LPCTSTR szConfigFile;
	
#if !DISABLE_FILE_AND_UI
	IFileDownload*  pFileDownload;
	IFileList*		pFileList;
	SeedSoundEngine::ISoundFileOpt*  pSoundFileOpt;
#endif

	CallBack_On_Resource_Loaded cbo_res_loaded; 
};

extern "C" SeedEngine::IApplication* g_SeedApplication;

extern "C" SeedExport BOOL CreateSeedEngine(const seed_init_params & initParam);
extern "C" SeedExport BOOL DestorySeedEngine();

#endif
