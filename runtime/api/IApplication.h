// * ||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
// * illegal driver
//2日
// * File: IApplication.h
// *
// * Author SeedEnginer
// * 
// * Desc:
// * ||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

#ifndef SEED_IAPPLICATION_H_INCLUDE
#define SEED_IAPPLICATION_H_INCLUDE

#include "runtime/api/IType.h"
#include "runtime/api/IGameObject.h"
#include "runtime/api/IScene.h"
#include "runtime/api/ICamera.h"
#include "runtime/api/ISound.h"

namespace SeedEngine
{
	class SeedExport IApplication
	{
	public:
		virtual VOID update(Real fElapsedTime)  = 0;
		
		virtual BOOL resource_load_progress(Real& precent) = 0;

		virtual VOID run()						= 0;
		virtual VOID pause()					= 0;
		virtual VOID resume()					= 0;

		virtual VOID onDeath(BOOL bFlag) = 0; 

		virtual VOID resize(int32_t nWidth, int32_t nHeight) = 0;

		virtual VOID draw_to_dc(VOID* hDc, int32_t nleft, int32_t nTop, int32_t nWidth, int32_t nHeight) = 0;

		virtual VOID set_axis_define(uint32_t tileLength = 32, uint32_t tile_count_per_merer = 2, uint32_t zero_z_point_in_gfx = 2048) = 0;
		virtual BOOL axis_trans(axis_type typeSource, const vector3& fvSource, axis_type typeTar, vector3& fvTarget) = 0;
		virtual BOOL axis_check_valid(axis_type typeSource, const vector3& fvAxis) = 0;

		virtual IGameObject* create_game_object(enum_game_object_type type) = 0;
		virtual VOID destroy_game_object(IGameObject* pNode) = 0;

		virtual ILevel* getLevel() = 0;
		virtual ICamera* getCamera() = 0;

		virtual uint32_t createNodeTrack(LPCTSTR szAniTrackName, BOOL bLoop, const vector3& vPos, const vector3& vRotate) = 0;
		virtual BOOL updateNodeTrack(uint32_t trackID, IGameObject* pNode) = 0;
		virtual VOID destroyNodeTrack(uint32_t trackID) = 0;

		virtual VOID apply_post_effect(LPCTSTR szEffectName, BOOL bEnable) = 0;

		virtual SeedSoundEngine::ISoundManager* get_sound_manager() = 0;
		virtual VOID set_listener_pos(vector3& vPos, vector3 vOrienation) = 0;

		virtual VOID create_ui_viewport(uint32_t sceneId, IGameObject* pGameObj, LPCTSTR szCameraName) = 0;
		virtual VOID update_ui_viewport(uint32_t sceneId, Real fAspectRatio, Real fDegree = 45.0f) = 0;
		virtual VOID update_ui_viewport(uint32_t sceneId, BOOL bFlag, Real fLeft, Real fTop, Real fWidth, Real fHeight) = 0;
		virtual VOID destory_ui_game_object(uint32_t sceneId, IGameObject* pGameObj) = 0;
		virtual VOID destory_ui_viewport(uint32_t sceneId) = 0;

		virtual IGameObject* get_hit_object(uint32_t x, uint32_t y) = 0;
		
	};
}

#endif

