// * ||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
// * illegal driver
//2日
// * File: ICamera.h
// *
// * Author SeedEnginer
// * 
// * Desc:
// * ||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

#ifndef __ICAMERAL_H_INCLUDE__
#define __ICAMERAL_H_INCLUDE__

#include "runtime/api/IType.h"

namespace SeedEngine
{

	typedef bool(*CallBack_CameraTrackerEnd)(DWORD);

	class SeedExport ICamera
	{
	public:
		virtual VOID setLookAt(Real fX, Real fY, Real fZ) = 0;
		virtual VOID setLookAtAuto(Real fX, Real fY, Real fZ) = 0;
		virtual vector3 getLookAt() const = 0;

		virtual VOID setDistance(Real fDistance) = 0;
		virtual VOID setDistanceAuto(Real fDistance) = 0;
		virtual Real getDistance() const = 0;

		virtual VOID setPitch(Real fPitch) = 0;
		virtual VOID setPitchAuto(Real fPitch) = 0;
		virtual Real getPitch() const = 0;

		virtual VOID addOneVib(Real VibRange, Real StepLen, unsigned Times) = 0;
		virtual VOID setOneVibVisible(BOOL oneVib) = 0;
		virtual BOOL getOneVibVisible() const = 0;

		virtual VOID setDirection(Real fDirection) = 0;
		virtual VOID setDirectionAuto(Real fDirection) = 0;
		virtual Real getDirection() const = 0;
		
		virtual VOID playCameraAniTrack(LPCTSTR szAniTrackName, BOOL bLoop, const vector3& vPos, const vector3& vRotate, CallBack_CameraTrackerEnd pFunc, DWORD dwParam) = 0;
		virtual VOID stopCameraAniTrack() = 0;

		virtual VOID update(Real fElapsedTime) = 0;

		virtual VOID setFreeCamera(BOOL bFlag) = 0;


		virtual VOID handleMovement(int flag, bool bBeginOrEnd) = 0;
		virtual VOID handleMouseOver(vector2 point, int flag) = 0;
		virtual VOID hanldeMouseWheel(vector2 point, Real delta) = 0;
	};

}

#endif
