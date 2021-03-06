// * ||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
// * illegal driver
//2日
// * File: SGRICamera.h
// *
// * Author SeedEnginer
// * 
// * Desc:
// * ||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
#ifndef __CAMERA_H_INCLUDE
#define __CAMERA_H_INCLUDE

#include "runtime/api/IType.h"
#include "runtime/api/ICamera.h"
#include "runtime/EngineFramework/camera.h"
namespace SeedEngine
{
	class EngineCamera : public ICamera
	{
	public:
		virtual VOID setLookAt(Real fX, Real fY, Real fZ);
		virtual VOID setLookAtAuto(Real fX, Real fY, Real fZ);
		virtual vector3	getLookAt() const;

		virtual VOID setPitch(Real fPitch);
		virtual VOID setPitchAuto(Real fPitch);
		virtual Real getPitch() const {	return m_fPitch; }

		virtual VOID setDistance(Real fDistance);
		virtual VOID setDistanceAuto(Real fDistance);
		virtual Real getDistance() const {	return m_fDistance;	}

		virtual VOID setDirection(Real fDirection);
		virtual VOID setDirectionAuto(Real fDirection);
		virtual Real getDirection() const { return m_fDirection; }

		virtual VOID update(Real fElapsedTime);

		egal::Camera* GetCamera() {	return m_pCamera; }
		virtual VOID Update();

		EngineCamera(egal::Camera* pOgreCamera);
		virtual ~EngineCamera();

		virtual VOID addOneVib(float VibRange, float StepLen, unsigned Times);
		virtual VOID setOneVibVisible(BOOL oneVib) { m_bOneVibVisible = oneVib;	}
		virtual BOOL getOneVibVisible() const { return m_bOneVibVisible; }

		virtual VOID playCameraAniTrack(LPCTSTR szAniTrackName, BOOL bLoop, const vector3& vPos, const vector3& vRotate, CallBack_CameraTrackerEnd pFunc, DWORD dwParam);
		virtual VOID stopCameraAniTrack();

		inline VOID setFreeCamera(BOOL bFlag)
		{
			mFreeCamera = bFlag;
			bFlag ? _SetStatus(FREEVIEW_STATUS) : _SetStatus(NORMAL_STATUS);
		}
		inline bool getFreeCamera()	{ return mFreeCamera; }

		virtual VOID handleMovement(int flag, bool bBeginOrEnd);
		virtual VOID handleMouseOver(vector2 point, int flag);
		virtual VOID hanldeMouseWheel(vector2 point, float delta);
	protected:
		enum CAMERA_STATUS
		{
			NORMAL_STATUS = 0,	
			VIBRATION_STATUS,	
			ANIMATION_STATUS,
			FREEVIEW_STATUS,
		};

		struct VibParam_t
		{
			float	fMaxBiasY;	
			float	fMinBiasY;	
			float	fBiasStep;
			unsigned	nTimes;		
			VibParam_t()
			{
				fMaxBiasY = 0.0;
				fMinBiasY = 0.0;
				fBiasStep = 0.0;
				nTimes = 0;
			}
		};

		CAMERA_STATUS		m_status;
		CAMERA_STATUS		m_LastStatus;

		VibParam_t			m_CurVibParam;

		egal::float3	    m_fvLookatPos;
		float				m_fPitch;
		float				m_fDistance;
		float				m_fDirection;

		float				m_fCurBias;
		float				m_fBiasStep;
		BOOL				m_bCurDirIsUp;
		unsigned			m_uAlreadyVibTimes;

		egal::Camera*		m_pCamera;
		egal::float3        m_fLookatPosAuto;

		float               m_fPitchAuto;
		float               m_fDistanceAuto;
		float               m_fDirectionAuto;
		BOOL                m_bOneVibVisible;

		CallBack_CameraTrackerEnd m_pFunc;
		DWORD m_dwPara;
		uint32_t m_uTrackID;

		BOOL    mFreeCamera;

	protected:
		BOOL _SetStatus(CAMERA_STATUS nStatus);
		VOID _UpdateCurStatus(egal::float3& vEyePos, egal::float3& vLookatPos);
	};
}
#endif
