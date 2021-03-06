// * ||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
// * illegal driver
//2日
// * File: Application.h
// *
// * Author SeedEnginer
// * 
// * Desc:
// * ||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
#ifndef __APPLICATION_H_INCLUDE__
#define __APPLICATION_H_INCLUDE__
#include "common/egal-d.h"
//.................................................................................................
//.................................................................................................
//.................................................................................................
#include "runtime/api/IApplication.h"
#include "runtime/api/impl/Level.h"

struct HideObjectInfo
{
	egal::GameObject* ptrObj;
	bool bCheck;
	float fInitAlpha;
	float fCurrentAlpha;
};
typedef std::list<egal::GameObject*>				HideObjectList;
typedef std::map<egal::GameObject*, HideObjectInfo> HideGameObjectMap;

typedef std::map<uint32_t, SeedEngine::IGameObject* > GameObjectMap;

namespace SeedEngine
{
	struct RaySceneQuery
	{

	};

	class Application : public IApplication
	{
	public:
		Application();
		virtual ~Application();

		virtual VOID update(Real fElapsedTime) ;

		virtual BOOL resource_load_progress(Real& precent);

		virtual VOID run() ;
		virtual VOID pause() ;
		virtual VOID resume() ;

		virtual VOID resize(int32_t nWidth, int32_t nHeight) ;

		virtual VOID onDeath(BOOL bFlag);

		virtual VOID draw_to_dc(VOID* hDc, int32_t nleft, int32_t nTop, int32_t nWidth, int32_t nHeight) ;

		virtual VOID set_axis_define(uint32_t tileLength = 32, uint32_t tile_count_per_merer = 2, uint32_t zero_z_point_in_gfx = 2048) ;
		virtual BOOL axis_trans(axis_type typeSource, const vector3& fvSource, axis_type typeTar, vector3& fvTarget) ;
		virtual BOOL axis_check_valid(axis_type typeSource, const vector3& fvAxis);

		virtual VOID apply_post_effect(LPCTSTR szEffectName, BOOL bEnable) ;

		virtual SeedSoundEngine::ISoundManager* get_sound_manager();
		virtual VOID set_listener_pos(vector3& vPos, vector3 vOrienation) ;

		virtual IGameObject* create_game_object(enum_game_object_type type) ;
		virtual VOID destroy_game_object(IGameObject* pNode) ;

		virtual ILevel* getLevel();
		virtual ICamera* getCamera();

		virtual uint32_t createNodeTrack(LPCTSTR szAniTrackName, BOOL bLoop, const vector3& vPos, const vector3& vRotate);
		virtual BOOL updateNodeTrack(uint32_t trackID, IGameObject* pNode);
		virtual VOID destroyNodeTrack(uint32_t trackID);

		virtual VOID create_ui_viewport(uint32_t sceneId, IGameObject* pGameObj, LPCTSTR szCameraName);
		virtual VOID update_ui_viewport(uint32_t sceneId, Real fAspectRatio, Real fDegree = 45.0f);
		virtual VOID update_ui_viewport(uint32_t sceneId, BOOL bFlag, Real fLeft, Real fTop, Real fWidth, Real fHeight);
		virtual VOID destory_ui_game_object(uint32_t sceneId, IGameObject* pGameObj);
		virtual VOID destory_ui_viewport(uint32_t sceneId);

		virtual IGameObject* get_hit_object(uint32_t x, uint32_t y);

		VOID clear_hide_object();
	private:
		VOID transparentHideObject(Real fTime);

		uint32_t			m_tileLength;
		uint32_t			m_tile_count_per_merer;
		uint32_t			m_zero_z_point_in_gfx;

		Level*				mLevel;

		RaySceneQuery*		m_pRaySceneQuery;

		HideGameObjectMap	mHideObjectSet;
		HideObjectList		m_HideObjectList;
		Real				m_sfAlphaSpeed;
		Real				m_sfDesAlpha;

	};
}

#endif