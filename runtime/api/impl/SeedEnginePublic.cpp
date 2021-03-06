// * ||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
// * illegal driver
//3日
// * File: SeedEngine.cpp
// *
// * Author SeedEnginer
// * 
// * Desc:
// * ||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

#include <iomanip>
#include <iostream>

#include "runtime/api/impl/Application.h"
#include "runtime/api/ISound.h"

SeedEngine::IApplication*	g_SeedApplication   = NULL;
seed_init_params			g_init_params;
EngineConfigOption			g_EngineOption;

#if !DISABLE_FILE_AND_UI /** 是否启用UI */
#include "LuaSeed.h"
#include "LuaSound.h"

#include "SGUIGraphicProcess.h"

SGUIGraphicProcess*			uiGraphicSystem = NULL;

void SeedLoadLua(ISGUI* pUi)
{
	LoadSeed(pUi->GetLuaState());
	LoadSound(pUi->GetLuaState());
}

ISGUIGraphicProcess* getUiGraphicProcess()
{
	if (!uiGraphicSystem)
		uiGraphicSystem = ENGINE_NEW SGUIGraphicProcess();

	return uiGraphicSystem;
}

#endif //---------------------------------------------

BOOL CreateSeedEngine(const seed_init_params & initParam)
{
	BOOL bResult = FALSE;
	g_init_params = initParam;
#if !DISABLE_FILE_AND_UI
	if (initParam.pFileList)
		g_EngineOption.sFileSystemName = "File";
#endif
	g_SeedApplication = new SeedEngine::Application();
	ENGINE_ERROR(g_SeedApplication);

	g_EngineOption.bShowUI = initParam.bOnlyEngine;
	if (initParam.bOnlyEngine)
		g_EngineOption.bEnableDownload = true;
	else
		g_EngineOption.bEnableDownload = true;
	
	bResult = TRUE;
Exit0:
	if (!bResult)
	{
		ENGINE_DELETE(g_SeedApplication);
	}

	return bResult;
}

BOOL DestorySeedEngine()
{
#if !DISABLE_FILE_AND_UI
	if (uiGraphicSystem)
		uiGraphicSystem = NULL;
#endif

	ENGINE_DELETE(g_SeedApplication);
	return TRUE;
}

//**********************************************************************************************************
//**********************************************************************************************************
EngineConfigOption::EngineConfigOption() :
bLightTextureDownloaded(FALSE)
, bShowUI(FALSE)
, bRendering(TRUE)
, bStramEnable(FALSE) //外网必须为False
, bEnableDownload(TRUE) //外网必须为true
, bAnimation(FALSE)
, bShowParticle(TRUE)
, bEngineConfigId(5)
, bGressDensity(1)
, fParticleLevel(1.0f)
, uTextureFilter(1)
, uTerrainLevel(1)
, uWaterLevel(1)
, bShakeCamera(TRUE)
, bProfile(FALSE)
, fResourceLoadPercentage(0)
, bResourceLoaded(FALSE)
, bEnableFog(FALSE)
, fDynamicObjectCount(0)
, sFileSystemName("FileSystem")
, fFarClipDistance(200.0f)
, bNoLightMap(FALSE)
, bGlobalCreateDynObject(TRUE)
, bGlobalCreateEffect(TRUE)
, bGlobalCreateSkill(TRUE)
, bShowAnimationLog(FALSE)
, bShowDownloadlog(FALSE)
, bStopRibbon(FALSE)
, bSound(TRUE)
, bUpdateEngineOnce(TRUE)
{
}
