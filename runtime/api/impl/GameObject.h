// * ||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
// * illegal driver
//7日
// * File: GameObject.h
// *
// * Author SeedEnginer
// * 
// * Desc:
// * ||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
#pragma once
#include "runtime/api/IType.h"
#include "runtime/api/IGameObject.h"
#include "runtime/api/impl/SeedEnginePublic.h"

namespace SeedEngine
{
	class GameObject : public IGameObject
	{
	public:
		GameObject();
		virtual ~GameObject();

	public:
		virtual BOOL GetDownload()
		{
			return FALSE;
		};
		/** death effect */
		virtual VOID DeathEnterAnim(LPCTSTR szAniName, LPCTSTR szTagName, BOOL bLoop, FLOAT fFuseParam = 0.3) { };

		/** base */
		virtual enum_game_object_type GetType(VOID) const
		{
			return m_eenum_game_object_type;
		};
		virtual VOID SetData(uint32_t dwData)
		{
			m_Data = dwData;
		};
		virtual uint32_t GetData(VOID) const
		{
			return m_Data;
		};
		virtual VOID Destroy(VOID);
		virtual VOID SetUIObjectVisible(BOOL bFlag) { };
		virtual VOID SetCreateRoleSpecialVisible(BOOL bFlag) { };
		virtual VOID SetVisible(BOOL bVisible, enum_dummy_location eInfoType = EDL_NONE) { };
		virtual VOID SetEffectVisible(BOOL bVisible, enum_dummy_location eInfoType = EDL_NONE) { };
		virtual BOOL GetVisible() const
		{
			return FALSE;
		};

		/** transform */
		virtual VOID SetPosition(const vector3& vPos) { };
		virtual VOID SetOrientation(const vector3& vRotate) { };
		virtual VOID SetEulerOrientation(const vector3& vRotate) { };
		virtual VOID SetScale(const vector3& vScale, enum_dummy_location eInfoType = EDL_NONE, float fTime = 0.0) {};

		/** set other obj property */
		virtual VOID SetProperty(LPCTSTR szPropertyName, LPCTSTR szPropertyValue, enum_dummy_location eInfoType = EDL_NONE) { };
		virtual VOID GetProperty(LPCTSTR szName, char*  szValue, size_t valueLength, enum_dummy_location eInfoType = EDL_NONE) { };

		/** set can select by mouse */
		virtual VOID SetRayQuery(BOOL bQuery)
		{
			m_bRayQuery = bQuery;
		};
		virtual BOOL GetRayQuery(VOID) const
		{
			return m_bRayQuery;
		};

		virtual VOID setRayQueryLevel(enum_ray_query_level nLevel)
		{
			m_nRayQueryLevel = nLevel;
		};
		virtual uint32_t getRayQueryLevel(VOID) const
		{
			return m_nRayQueryLevel;
		};

		//附加一个特效
		virtual uint32_t AddEffect(LPCTSTR effectName, LPCTSTR szBindName, enum_dummy_location eInfoType = EDL_NONE, BOOL bRot = FALSE)
		{
			return 0;
		};
		virtual VOID DelEffect(uint32_t effect, enum_remove_type removeEffectType = ERT_NOW, enum_dummy_location eInfoType = EDL_NONE) { };
		virtual VOID DelAllEffect(enum_dummy_location eInfoType = EDL_NONE) { };

		virtual VOID Attach_Object(IGameObject *pObject, LPCTSTR szAttachLocator) { };
		virtual VOID Detach_Object(IGameObject *pObject) { };

		/** 设置模型名字 */
		virtual VOID SetResourceFile(LPCTSTR szActorFile) { };
		virtual BOOL HasSetResourceFile()
		{
			return FALSE;
		}

		/** 播放动画以及标签 */
		virtual VOID EnterWeaponAnim(LPCTSTR szAniName, enum_dummy_location locLocation, BOOL bLoop, FLOAT fFuseParam = 0.3) { };
		/** 播放标签，只播放标签，不播放动作 */
		virtual VOID PlayTag(LPCTSTR szfTagName, BOOL bLoop, BOOL bTimeControl) {};
		/** 播放动作，只播放动作  */
		virtual VOID PlayAnim(LPCTSTR szAniName, BOOL bLoop, FLOAT fFuseParam = 0.3, FLOAT fRadian = 0.0) {};
		/** 播放动作标签 */
		virtual VOID PlayAnimTag(LPCTSTR szAniName, LPCTSTR szTagName, BOOL bLoop, FLOAT fFuseParam = 0.3, FLOAT fRadian = 0.0) {};
		/** 播放动作双标签 */
		virtual VOID PlayAnimDoubleTag(LPCTSTR szAniName, LPCTSTR szfTagName, LPCTSTR szsTagName, BOOL bLoop, FLOAT fFuseParam = 0.3, FLOAT fRadian = 0.0) {};

		/** 取得任务模型上的某关键点位置 */
		virtual VOID GetLocator(LPCTSTR szName, vector3& fvPosition) { };
		/** 取得"头顶状态点"在屏幕上的位置,如果返回FALSE，表示在屏幕之外,或者没有该点 */
		virtual BOOL GetInfoBoardPos(vector3& ivPos, const vector3 *pvObjPos = NULL, const float fObligeHeight = -1)
		{
			return FALSE;
		};

		/** 切换动画的播放速度 fRate - 缩放比率 */
		virtual VOID ChangeActionRate(FLOAT fRate, enum_dummy_location locLocation = EDL_NONE) { };

		/** 设置缺省动作 */
		virtual VOID SetDefaultAnim(LPCTSTR szAnimName) { };

		/** 设置UI VisibleFlag */
		virtual VOID SetUIVisibleFlag(VOID) { };

		virtual VOID SetTransparent(FLOAT fTransparency, FLOAT fTime) { };

		/** 设置连线特效的目标点 */
		virtual VOID SetEffectExtraTransformInfos(uint32_t uEffect, vector3& fvPosition) { };

		/** 设置是否出于鼠标Hover状态 */
		virtual VOID SetMouseHover(BOOL bHover, uint32_t color = 0xFF0000FF, float crisperdingWidth = 0.04f) { };
		virtual VOID SetHitColour(const vector3& vColour, float fTime) { };

		/** set shader */
		virtual VOID ApplyShader(LPCTSTR szShaderName) { };
		virtual VOID NormalShader() { };

		virtual VOID SetAnimationEndEvent(CallBack_On_Animation_End pFunc, DWORD dwParam){}
		virtual VOID SetAnimationShakeEvent(CallBack_On_Animation_Shake pFunc, DWORD dwParam){}
	protected:
		uint32_t					m_Data;

		enum_ray_query_level	m_nRayQueryLevel;
		enum_game_object_type   m_eenum_game_object_type;
		BOOL				    m_bRayQuery;
	};
}
