#include "runtime/api/impl/EngineCamera.h"
#include "runtime/api/impl/Application.h"

namespace SeedEngine
{
	EngineCamera::EngineCamera(egal::Camera* pCamera)
		: m_pCamera(pCamera)
	{
		assert(m_pCamera);

		m_pCamera = pCamera;

		m_fCurBias = 0;
		m_fBiasStep = 0;
		m_bCurDirIsUp = TRUE;
		m_uAlreadyVibTimes = 0;

		m_fvLookatPos.x = 0.0f;
		m_fvLookatPos.y = 0.0f;
		m_fvLookatPos.z = 0.0f;
		m_fPitch = 0.0f;
		m_fDistance = 0.0f;
		m_fDirection = 0.0f;

		m_fLookatPosAuto.x = 0.0f;
		m_fLookatPosAuto.y = 0.0f;
		m_fLookatPosAuto.z = 0.0f;
		m_fPitchAuto = 0.0f;
		m_fDistanceAuto = 0.0f;
		m_fDirectionAuto = 0.0f;

		m_bOneVibVisible = true;

		m_uTrackID = 0;
		m_pFunc = NULL;
		m_dwPara = 0;

		m_status = NORMAL_STATUS;

		mFreeCamera = false;
	}

	EngineCamera::~EngineCamera()
	{
	}

	void EngineCamera::setLookAt(Real fX, Real fY, Real fZ)
	{
		vector3 fvLookAt;
		vector3 fvLooAtGFX;

		fvLookAt.x = fX;
		fvLookAt.y = fY;
		fvLookAt.z = fZ;

		g_SeedApplication->axis_trans(EXT_GAME, fvLookAt, EXT_ENGINE, fvLooAtGFX);

		m_fvLookatPos.x = (Real)fvLooAtGFX.x;
		m_fvLookatPos.y = (Real)fvLooAtGFX.y;
		m_fvLookatPos.z = (Real)fvLooAtGFX.z;

		Update();
	}

	void EngineCamera::setLookAtAuto(Real fX, Real fY, Real fZ)
	{
		vector3 fvLookAt;
		vector3 fvLooAtGFX;

		fvLookAt.x = fX;
		fvLookAt.y = fY;
		fvLookAt.z = fZ;

		g_SeedApplication->axis_trans(EXT_ENGINE, fvLookAt, EXT_GAME, fvLooAtGFX);

		m_fLookatPosAuto.x = (Real)fvLooAtGFX.x;
		m_fLookatPosAuto.y = (Real)fvLooAtGFX.y;
		m_fLookatPosAuto.z = (Real)fvLooAtGFX.z;
	}
	void EngineCamera::setDistance(Real fDistance)
	{
		m_fDistance = (float)fDistance;
		Update();
	}

	void EngineCamera::setDistanceAuto(Real fDistance)
	{
		m_fDistanceAuto = (float)fDistance;
	}

	void EngineCamera::setDirection(Real fDirection)
	{
		m_fDirection = (float)fDirection;
		Update();
	}

	void EngineCamera::setDirectionAuto(Real fDirection)
	{
		m_fDirectionAuto = (float)fDirection;
	}

	vector3 EngineCamera::getLookAt() const
	{
		vector3 fvLookAt;
		vector3 fvLooAtGFX;

		fvLooAtGFX.x = m_fvLookatPos.x;
		fvLooAtGFX.y = m_fvLookatPos.y;
		fvLooAtGFX.z = m_fvLookatPos.z;

		g_SeedApplication->axis_trans(EXT_ENGINE, fvLooAtGFX, EXT_GAME, fvLookAt);

		return fvLookAt;
	}

	void EngineCamera::setPitch(Real fPitch)
	{
		m_fPitch = (float)fPitch;
		Update();
	}

	void EngineCamera::setPitchAuto(Real fPitch)
	{
		m_fPitchAuto = (float)fPitch;
	}
	
	void EngineCamera::update(Real fElapsedTime)
	{
		Real fTime = (Real)fElapsedTime;
		if (fTime > 2.0)
			fTime = 2.0;

		ENGINE_ERROR(fElapsedTime > 0.0);


		m_fvLookatPos.x += m_fLookatPosAuto.x * fTime;
		m_fvLookatPos.y += m_fLookatPosAuto.y * fTime;
		m_fvLookatPos.z += m_fLookatPosAuto.z * fTime;

		m_fPitch += m_fPitchAuto * fTime;
		m_fDistance += m_fDistanceAuto * fTime;
		m_fDirection += m_fDirectionAuto * fTime;

		Update();
	Exit0:
		return;
	}

	void EngineCamera::Update()
	{
		if (m_status == FREEVIEW_STATUS)
			return;

		float y = m_fDistance * ::sin(-m_fPitch);
		float r_xy = m_fDistance * ::cos(-m_fPitch);
		float x = -r_xy * ::sin(m_fDirection);
		float z = -r_xy * ::cos(m_fDirection);

		egal::float3 vEyePos(m_fvLookatPos.x + x,
			m_fvLookatPos.y + y,
			m_fvLookatPos.z + z);

		_UpdateCurStatus(vEyePos, m_fvLookatPos);

		m_pCamera->setPosition(egal::float3(vEyePos.x, vEyePos.y, vEyePos.z));
		//m_pCamera->lookAt(egal::float3(m_fvLookatPos.x, m_fvLookatPos.y, m_fvLookatPos.z));
	}
	BOOL EngineCamera::_SetStatus(CAMERA_STATUS nStatus)
	{
		m_LastStatus = m_status;
		switch (nStatus)
		{
		case NORMAL_STATUS:
		{
			switch (m_status)
			{
			case VIBRATION_STATUS:
			{
				m_CurVibParam.fMaxBiasY = 0.0;
				m_CurVibParam.fMinBiasY = 0.0;
				m_CurVibParam.nTimes = 0;
				m_CurVibParam.fBiasStep = 0.0;

				m_fCurBias = 0.0;
				m_fBiasStep = 0.0;
				m_bCurDirIsUp = TRUE;
				m_uAlreadyVibTimes = 0;
			}
			break;
			case ANIMATION_STATUS:
			{
			}
			break;
			case FREEVIEW_STATUS:
				break;
			default:
				return FALSE;
			}
			m_status = nStatus;
			return TRUE;
		}
		break;
		case VIBRATION_STATUS:
		{
			switch (m_status)
			{
			case NORMAL_STATUS:
			{
				m_fCurBias = 0.0;
				m_fBiasStep = m_CurVibParam.fBiasStep;
				m_bCurDirIsUp = TRUE;
				m_uAlreadyVibTimes = 0;
			}
			break;
			case ANIMATION_STATUS:
			{
				return FALSE;
			}
			break;
			case FREEVIEW_STATUS:
			{
				return FALSE;
			}
			break;
			default:
				return FALSE;
			}
			m_status = nStatus;
			return TRUE;
		}
		break;
		case ANIMATION_STATUS:
		{

			switch (m_status)
			{
			case NORMAL_STATUS:
			{
			}
			break;
			case VIBRATION_STATUS:
			{
				m_CurVibParam.fMaxBiasY = 0.0;
				m_CurVibParam.fMinBiasY = 0.0;
				m_CurVibParam.nTimes = 0;
				m_CurVibParam.fBiasStep = 0.0;

				m_fCurBias = 0.0;
				m_fBiasStep = 0.0;
				m_bCurDirIsUp = TRUE;
				m_uAlreadyVibTimes = 0;
			}
			break;
			case FREEVIEW_STATUS:
			{
				return FALSE;
			}
			break;
			default:
				return FALSE;
			}
			m_status = nStatus;
			return TRUE;
		}
		break;
		case FREEVIEW_STATUS:
		{

			switch (m_status)
			{
			case NORMAL_STATUS:
			{

			}
			break;
			case VIBRATION_STATUS:
			{
				m_CurVibParam.fMaxBiasY = 0.0;
				m_CurVibParam.fMinBiasY = 0.0;
				m_CurVibParam.nTimes = 0;
				m_CurVibParam.fBiasStep = 0.0;

				m_fCurBias = 0.0;
				m_fBiasStep = 0.0;
				m_bCurDirIsUp = TRUE;
				m_uAlreadyVibTimes = 0;
			}
			break;
			}
			m_status = nStatus;
		}
		break;
		default:
			return FALSE;
		}
		return TRUE;
	}

	void EngineCamera::_UpdateCurStatus(egal::float3& vEyePos, egal::float3& vLookatPos)
	{
		CallBack_CameraTrackerEnd pFunc = NULL;
		DWORD dwPara = 0;

		switch (m_status)
		{
		case NORMAL_STATUS:
		{
			return;
		}
		break;
		case VIBRATION_STATUS:
		{
			if (m_uAlreadyVibTimes == m_CurVibParam.nTimes)
			{
				_SetStatus(NORMAL_STATUS);
				return;
			}
			m_fCurBias += m_fBiasStep;
			if (m_bCurDirIsUp == FALSE && m_fCurBias > 0)
			{
				m_uAlreadyVibTimes++;
				m_bCurDirIsUp = TRUE;
			}
			else if (m_bCurDirIsUp == TRUE && m_fCurBias < 0)
			{
				m_bCurDirIsUp = FALSE;
			}

			if (m_fCurBias > m_CurVibParam.fMaxBiasY)
			{
				m_fCurBias = m_CurVibParam.fMaxBiasY;
				m_fBiasStep = -m_CurVibParam.fBiasStep;
			}
			else if (m_fCurBias < m_CurVibParam.fMinBiasY)
			{
				m_fCurBias = m_CurVibParam.fMinBiasY;
				m_fBiasStep = m_CurVibParam.fBiasStep;
			}
			vLookatPos.y += m_fCurBias;
			vEyePos.y += m_fCurBias;
			return;
		}
		break;
		case ANIMATION_STATUS:
		{
			//if (g_EngineManager)
			//{
			//	BOOL bFinish = TRUE;
			//	NodeTrackerManager* pAniTrackMgr = g_EngineManager->getNodeTrackerManager();
			//	if (pAniTrackMgr)
			//	{
			//		TransformInfo transformInfo;
			//		Ogre::String strBoneName = "default";
			//		bFinish = pAniTrackMgr->getTrackerInfo(m_uTrackID, strBoneName, transformInfo);

			//		vEyePos = transformInfo.mPosition;
			//		vLookatPos = vEyePos + transformInfo.mRotation * Ogre::Vector3::UNIT_X;
			//		if (bFinish)
			//		{
			//			if (m_pFunc)
			//			{
			//				pFunc = m_pFunc;
			//				dwPara = m_dwPara;
			//			}

			//			stopCameraAniTrack();

			//			if (pFunc)
			//				pFunc(dwPara);
			//		}

			//	}
			//}
			return;
		}
		default:
			return;
		}
		return;
	}

	void EngineCamera::playCameraAniTrack(LPCTSTR szAniTrackName, BOOL bLoop, const vector3& vPos, const vector3& vRotate, CallBack_CameraTrackerEnd pFunc, DWORD dwParam)
	{
		if (m_uTrackID != 0)
			stopCameraAniTrack();
		m_uTrackID = g_SeedApplication->createNodeTrack(szAniTrackName, bLoop, vPos, vRotate);
		m_pFunc = pFunc;
		m_dwPara = dwParam;
		_SetStatus(ANIMATION_STATUS);
	}
	void EngineCamera::stopCameraAniTrack()
	{
		if (m_uTrackID != 0)
		{
			g_SeedApplication->destroyNodeTrack(m_uTrackID);
			m_uTrackID = 0;
			m_pFunc = NULL;
			m_dwPara = 0;
			_SetStatus(NORMAL_STATUS);
		}
	}
	//key down or release
	VOID EngineCamera::handleMovement(int flag, bool bBeginOrEnd)
	{
	}
	//mouse move
	VOID EngineCamera::handleMouseOver(vector2 point, int flag)
	{
	}
	VOID EngineCamera::hanldeMouseWheel(vector2 point, float delta)
	{
	}

	void EngineCamera::addOneVib(float VibRange, float StepLen, unsigned Times)
	{
		if (!m_bOneVibVisible)
			return;

		if (VibRange <= 0 || Times <= 0 || StepLen <= 0)
			return;

		if (m_status == VIBRATION_STATUS)
		{
			return;
		}

		m_CurVibParam.fMaxBiasY = VibRange;
		m_CurVibParam.fMinBiasY = -VibRange;
		m_CurVibParam.nTimes = Times;
		m_CurVibParam.fBiasStep = StepLen;

		_SetStatus(VIBRATION_STATUS);
	}

}