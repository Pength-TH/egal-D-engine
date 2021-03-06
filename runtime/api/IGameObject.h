// * ||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
// * illegal driver
//3日
// * File: IGameObject.h
// *
// * Author SeedEnginer
// * 
// * Desc:
// * ||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
#ifndef __IGAME_OBJECT_H_INCLUDE__
#define __IGAME_OBJECT_H_INCLUDE__

#include "runtime/api/IType.h"

typedef bool(*CallBack_On_Animation_End)(LPCTSTR, DWORD);
typedef bool(*CallBack_On_Animation_Shake)(LPCTSTR, DWORD, float, float, unsigned);

namespace SeedEngine
{
	class SeedExport IGameObject
	{
	public:
		IGameObject(){}
		virtual ~IGameObject(){}

		virtual BOOL GetDownload() = 0;

		virtual VOID Destroy(VOID) = 0;

		/** death effect
		*   will setEffectVisible(true)
		*   will setNormalShader();
		*/
		virtual VOID DeathEnterAnim(LPCTSTR szAniName, LPCTSTR szTagName, BOOL bLoop, Real fFuseParam = 0.3) = 0;

		/** base */
		virtual enum_game_object_type GetType(VOID) const = 0;
		virtual VOID SetData(uint32_t dwData) = 0;
		virtual uint32_t GetData(VOID) const = 0;

		/** 设置UI VisibleFlag */
		virtual VOID SetUIVisibleFlag(VOID) = 0;
		virtual VOID SetUIObjectVisible(BOOL bFlag) = 0;
		virtual VOID SetCreateRoleSpecialVisible(BOOL bFlag) = 0;
		virtual VOID SetVisible(BOOL bVisible, enum_dummy_location eInfoType = EDL_NONE) = 0;
		virtual VOID SetEffectVisible(BOOL bVisible, enum_dummy_location eInfoType = EDL_NONE) = 0;
		virtual BOOL GetVisible() const = 0;

		/** transform */
		virtual VOID SetPosition(const vector3& vPos) = 0;
		virtual VOID SetOrientation(const vector3& vRotate) = 0;
		virtual VOID SetEulerOrientation(const vector3& vRotate) = 0;
		
		/**
		* enum_dummy_location eInfoType :部件位置，默认是全部部件都设置
		* fTime ： 时间变换 多少时间内从当前值变化到目标值
		*/
		virtual VOID SetScale(const vector3& vScale, enum_dummy_location eInfoType = EDL_NONE, float fTime = 0.0) = 0;

		/** set other obj property */
		virtual VOID SetProperty(LPCTSTR szPropertyName, LPCTSTR szPropertyValue, enum_dummy_location eInfoType = EDL_NONE) = 0;
		virtual VOID GetProperty(LPCTSTR szName, char* pValue, size_t valueLength, enum_dummy_location eInfoType = EDL_NONE) = 0;

		/** set can select by mouse */
		virtual VOID SetRayQuery(BOOL bQuery) = 0;
		virtual BOOL GetRayQuery(VOID) const = 0;

		virtual VOID setRayQueryLevel(enum_ray_query_level nLevel) = 0;
		virtual uint32_t getRayQueryLevel(VOID) const = 0;

		//附加一个特效
		virtual uint32_t AddEffect(LPCTSTR effectName, LPCTSTR szBindName = NULL, enum_dummy_location eInfoType = EDL_NONE, BOOL bRot = TRUE) = 0;
		virtual VOID DelEffect(uint32_t effect, enum_remove_type removeEffectType = ERT_NOW, enum_dummy_location eInfoType = EDL_NONE) = 0;
		virtual VOID DelAllEffect(enum_dummy_location eInfoType = EDL_NONE) = 0;

		virtual VOID Attach_Object(IGameObject *pObject, LPCTSTR szAttachLocator) = 0;
		virtual VOID Detach_Object(IGameObject *pObject) = 0;

		/** 设置模型名字 */
		virtual VOID SetResourceFile(LPCTSTR szActorFile) = 0;
		/** 是否设置过模型名字 */
		virtual BOOL HasSetResourceFile() = 0;

		/** 播放动画以及标签 */
		virtual VOID EnterWeaponAnim(LPCTSTR szAniName, enum_dummy_location locLocation, BOOL bLoop, Real fFuseParam = 0.3) = 0;

		/** 播放标签，只播放标签，不播放动作 */
		virtual VOID PlayTag(LPCTSTR szfTagName, BOOL bLoop, BOOL bTimeControl) = 0;
		/** 播放动作，只播放动作  */
		virtual VOID PlayAnim(LPCTSTR szAniName, BOOL bLoop, Real fFuseParam = 0.3, Real fRadian = 0.0) = 0;
		/** 播放动作标签 */
		virtual VOID PlayAnimTag(LPCTSTR szAniName, LPCTSTR szTagName, BOOL bLoop, Real fFuseParam = 0.3, Real fRadian = 0.0) = 0;
		/** 播放动作双标签 */
		virtual VOID PlayAnimDoubleTag(LPCTSTR szAniName, LPCTSTR szfTagName, LPCTSTR szsTagName, BOOL bLoop, Real fFuseParam = 0.3, Real fRadian = 0.0) = 0;

		/** 取得任务模型上的某关键点位置 */
		virtual VOID GetLocator(LPCTSTR szName, vector3& fvPosition) = 0;
		
		/** 取得"头顶状态点"在屏幕上的位置,如果返回FALSE，表示在屏幕之外,或者没有该点 */
		virtual BOOL GetInfoBoardPos(vector3& ivPos, const vector3 *pvObjPos, const float fObligeHeight = -1) = 0;

		/** 切换动画的播放速度 fRate - 缩放比率 */
		virtual VOID ChangeActionRate(Real fRate, enum_dummy_location locLocation = EDL_NONE) = 0;

		/** 设置缺省动作 */
		virtual VOID SetDefaultAnim(LPCTSTR szAnimName) = 0;

		virtual VOID SetTransparent(Real fTransparency, Real fTime) = 0;

		/** 设置连线特效的目标点 */
		virtual VOID SetEffectExtraTransformInfos(uint32_t uEffect, vector3& fvPosition) = 0;

		/** 设置是否出于鼠标Hover状态 */
		virtual VOID SetMouseHover(BOOL bHover, uint32_t color = 0xFF0000FF, float crisperdingWidth = 0.04f) = 0;
		virtual VOID SetHitColour(const vector3& vColour, float fTime) = 0;

		/** set shader */
		virtual VOID ApplyShader(LPCTSTR szShaderName) = 0;
		virtual VOID NormalShader() = 0;

		/**  Event */
		virtual VOID SetAnimationEndEvent(CallBack_On_Animation_End pFunc, DWORD dwParam)= 0;
		virtual VOID SetAnimationShakeEvent(CallBack_On_Animation_Shake pFunc, DWORD dwParam) = 0;
	};

}
#endif
