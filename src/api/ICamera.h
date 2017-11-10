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

#include "IType.h"

namespace SeedEngine
{

	typedef bool(*CallBack_CameraTrackerEnd)(DWORD);

	class SeedExport ICamera
	{
	public:
		virtual VOID set_look_at(Real fX, Real fY, Real fZ) = 0;
		virtual VOID set_look_at_auto(Real fX, Real fY, Real fZ) = 0;
		virtual vector3 get_look_at() const = 0;

		virtual VOID set_distance(Real fDistance) = 0;
		virtual VOID set_distanceAuto(Real fDistance) = 0;
		virtual Real get_distance() const = 0;

		virtual VOID set_pitch(Real fPitch) = 0;
		virtual VOID set_pitch_auto(Real fPitch) = 0;
		virtual Real get_pitch() const = 0;

		virtual VOID add_one_vib(Real VibRange, Real StepLen, unsigned Times) = 0;
		virtual VOID set_one_vib_visible(BOOL oneVib) = 0;
		virtual BOOL get_one_vib_visible() const = 0;

		virtual VOID set_direction(Real fDirection) = 0;
		virtual VOID set_direction_auto(Real fDirection) = 0;
		virtual Real get_direction() const = 0;
		
		virtual VOID play_camera_ani_track(LPCTSTR szAniTrackName, BOOL bLoop, const vector3& vPos, const vector3& vRotate, CallBack_CameraTrackerEnd pFunc, DWORD dwParam) = 0;
		virtual VOID stop_camera_ani_track() = 0;

		virtual VOID update(Real fElapsedTime) = 0;

		virtual VOID set_free_camera(BOOL bFlag) = 0;


		virtual VOID handle_movement(int flag, bool bBeginOrEnd) = 0;
		virtual VOID handle_mouse_over(vector2 point, int flag) = 0;
		virtual VOID hanlde_mouse_wheel(vector2 point, Real delta) = 0;
	};

}

#endif
