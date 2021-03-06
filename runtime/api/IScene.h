#ifndef SEED_ILEVEL_INCLUDE
#define SEED_ILEVEL_INCLUDE

#include "runtime/api/IType.h"
namespace SeedEngine
{
	class SeedExport ILevel
	{
	public:
		virtual BOOL play_screen_effect(LPCTSTR effectName) = 0;
		virtual BOOL play_3dsound(LPCTSTR effectName, BOOL bLoop = FALSE) = 0;
		virtual BOOL change_bgm(LPCTSTR effectName, BOOL bLoop = FALSE) = 0;
		virtual BOOL play_scene_anim(LPCTSTR szAniName, BOOL bLoop, Real fTime) = 0;

		virtual VOID load_level(LPCTSTR level_file) = 0;
		virtual VOID unload() = 0;

		virtual VOID refreshPageObject() = 0;

		virtual Real getHeight(Real x, Real z) = 0;
		// TODO:
	};
}

#endif
