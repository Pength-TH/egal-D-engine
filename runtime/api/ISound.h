#ifndef _ISOUND_H_
#define _ISOUND_H_

#include <assert.h>

#ifdef _WIN32
#include <Windows.h>
#endif

#include <math.h>
#include <string>
#include <vector>
#include <map>

namespace SeedSoundEngine
{

	typedef unsigned int S_Uint32;

	struct vector3
	{
		float fX;
		float fY;
		float fZ;
	};

	enum FILE_OPEN_TYPE
	{
		OFT_BEGIN,
		OFT_NORMAL = OFT_BEGIN,
		OFT_PACKAGE_FIRST,
		OFT_UNPACKAGE_FIRST,
		OFT_PACKAGE,
		OFT_UNPACKAGE,
		OFT_FULL_PATH,
		OFT_END,
	};

	struct SOUND_SOUND_TYPE_INFO
	{
		BOOL bEnable;
		float fVolume;
		BOOL bReplaceMode;
		unsigned int uMaxSameFileCount;
		unsigned int uMaxCount;
	};

	class ISound
	{
	public:
		virtual BOOL SetVelocity(const vector3 * pcVelocity) = 0;
		virtual BOOL GetVelocity(vector3 * pVelocity) = 0;
		virtual BOOL SetConeOutsideVolume(float fVolume) = 0;
		virtual float GetConeOutsideVolume() = 0;

		virtual BOOL SetMinDistance(float fMinDistance) = 0;
		virtual float GetMinDistance() = 0;

		virtual BOOL SetMaxDistance(float fMaxDistance) = 0;
		virtual float GetMaxDistance() = 0;

		virtual void Release() = 0;

		virtual BOOL SetVolume(float fVolume) = 0;
		virtual float GetVolume() = 0;

		virtual BOOL SetLoop(BOOL bLoop) = 0;
		virtual BOOL IsLoop() = 0;

		virtual BOOL SetLoopCount(int nLoopCount) = 0;
		virtual int GetLoopCount() = 0;

		virtual BOOL Play() = 0;
		virtual BOOL Stop(BOOL bImmediately) = 0;
		virtual BOOL IsPlaying() = 0;

		virtual BOOL Is2DSound() = 0;
		virtual BOOL SetConeAngle(float fInsideAngle, float fOutsideAngle) = 0;
		virtual BOOL GetConeAngle(float * pfInsideAngle, float * pfOutsideAngle) = 0;

		virtual BOOL SetConeOrientation(const vector3 * pcOrientation) = 0;
		virtual BOOL GetConeOrientation(vector3 * pOrientation) = 0;


		virtual BOOL Is3DSound() = 0;
		virtual BOOL IsThreePartSound() = 0;

		virtual const char * GetSoundType() = 0;

		virtual const char * GetFile() = 0;
		virtual const char * GetFileBegin() = 0;
		virtual const char * GetFileLoop() = 0;
		virtual const char * GetFileEnd() = 0;
		virtual FILE_OPEN_TYPE GetOpenFileType() = 0;

		virtual BOOL SetPosition(const vector3 * pcPosition) = 0;
		virtual BOOL GetPosition(vector3 * pPosition) = 0;

		virtual BOOL Pause() = 0;
		virtual BOOL Continue() = 0;
		virtual BOOL IsPaused() = 0;

		virtual BOOL Rewind() = 0;

		virtual BOOL FadeIn(unsigned int uTime) = 0;
		virtual BOOL IsFadeIn() = 0;

		virtual BOOL FadeOut(unsigned int uTime) = 0;
		virtual BOOL IsFadeOut() = 0;

		virtual BOOL SetPan(int nPan) = 0;
		virtual int GetPan() = 0;
	};


	class ISoundReadFile
	{
	public:
		virtual int GetSize() = 0;
		virtual void* GetMemoryBuffer() = 0;
		virtual unsigned int GetUserFlag() = 0;
		virtual BOOL Read(void* pBuffer, int nSize) = 0;
		virtual BOOL Seek(int nOffset, int nOrigin) = 0;
		virtual int Tell() = 0;
		virtual void Close() = 0;
	};

	class ISoundFileOpt
	{
	public:
		virtual ISoundReadFile* OpenFile(const char* pcszFile, FILE_OPEN_TYPE eType) = 0;
		virtual size_t FormatFilePath(const char* pcszSrcFile, size_t uSrcFileLength, char* pszDstFile, size_t uDstFileSize) = 0;
	};

	enum SOUND_LOG_TYPE
	{
		SLT_BEGIN,
		SLT_INFOMATION = SLT_BEGIN,
		SLT_WARNING,
		SLT_ERROR,
		SLT_FATAL_ERROR,
		SLT_END,
	};

	class ISoundManager
	{
	public:
		virtual BOOL PlayThreePart2DSound(
			const char * pcszSoundType, float fPriority,
			const char * pcszFileBegin, const char * pcszFileLoop, const char * pcszFileEnd, FILE_OPEN_TYPE eOpenFileType,
			BOOL bLoop, int nLoopCount,
			BOOL bFadeIn, unsigned int uFadeInTime,
			float fVolume,
			int nPan,
			ISound ** ppSound
		) = 0;

		virtual BOOL Play3DSound(
			const char * pcszSoundType, float fPriority,
			const char * pcszFile, FILE_OPEN_TYPE eOpenFileType,
			BOOL bLoop, int nLoopCount,
			BOOL bFadeIn, unsigned int uFadeInTime,
			float fVolume,
			const vector3 * pcPosition, const vector3 * pcVelocity,
			float fConeInsideAngle, float fConeOutsideAngle, const vector3 * pcConeOrientation, float fConeOutsideVolume,
			float fMinDistance, float fMaxDistance,
			ISound ** ppSound
		) = 0;

		virtual BOOL PlayThreePart3DSound(
			const char * pcszSoundType, float fPriority,
			const char * pcszFileBegin, const char * pcszFileLoop, const char * pcszFileEnd, FILE_OPEN_TYPE eOpenFileType,
			BOOL bLoop, int nLoopCount,
			BOOL bFadeIn, unsigned int uFadeInTime,
			float fVolume,
			const vector3 * pcPosition, const vector3 * pcVelocity,
			float fConeInsideAngle, float fConeOutsideAngle, const vector3 * pcConeOrientation, float fConeOutsideVolume,
			float fMinDistance, float fMaxDistance,
			ISound ** ppSound
		) = 0;

		virtual void Activate() = 0;
		virtual void Release() = 0;

		virtual BOOL ClearSound() = 0;
		virtual BOOL ClearSoundTypeSound(const char * pcszSoundType) = 0;

		virtual BOOL Enable(BOOL bEnable) = 0;
		virtual BOOL IsEnabled() = 0;

		virtual BOOL SetVolume(float fVolume) = 0;
		virtual float GetVolume() = 0;

		virtual BOOL SetListenerDopplerFactor(float fDopplerFactor) = 0;
		virtual float GetListenerDopplerFactor() = 0;

		virtual BOOL SetListenerRolloffFactor(float fRolloffFactor) = 0;
		virtual float GetListenerRolloffFactor() = 0;

		virtual BOOL SetListenerDistanceFactor(float fDistanceFactor) = 0;
		virtual float GetListenerDistanceFactor() = 0;

		virtual BOOL SetSoundTypeInfo(const char * pcszSoundType, const SOUND_SOUND_TYPE_INFO * pcInfo) = 0;
		virtual BOOL GetSoundTypeInfo(const char * pcszSoundType, SOUND_SOUND_TYPE_INFO * pInfo) = 0;

		virtual BOOL SetListenerPosition(const vector3 * pcPosition) = 0;
		virtual BOOL GetListenerPosition(vector3 * pPosition) = 0;

		virtual BOOL SetListenerVelocity(const vector3 * pcVelocity) = 0;
		virtual BOOL GetListenerVelocity(vector3 * pVelocity) = 0;

		virtual BOOL SetListenerOrientation(const vector3 * pcFrontOrientation, const vector3 * pcTopOrientation) = 0;
		virtual BOOL GetListenerOrientation(vector3 * pFrontOrientation, vector3 * pTopOrientation) = 0;


		virtual BOOL Play2DSound(
			const char * pcszSoundType, float fPriority,
			const char * pcszFile, FILE_OPEN_TYPE eOpenFileType,
			BOOL bLoop, int nLoopCount,
			BOOL bFadeIn, unsigned int uFadeInTime,
			float fVolume,
			int nPan,
			ISound ** ppSound
		) = 0;


	};

	struct SOUND_CREATE_PARAM
	{
		HWND hWnd;
		BOOL bAvailableWhenOnlyHaveFocus;
		ISoundFileOpt * pFileSystem;
		size_t uParamSize;
		unsigned int uParamVersion;
	};

	extern "C" ISoundManager * CreateSoundManager(const SOUND_CREATE_PARAM * pcParam);

}

#endif