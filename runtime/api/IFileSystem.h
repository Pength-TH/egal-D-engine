// * ||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
// * illegal driver
//14日
// * File: IFileSystem.h
// *
// * Author SeedEnginer
// * 
// * Desc:
// * ||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

#ifndef __IFILE_SYSTEM_H_INCLUDE__
#define __IFILE_SYSTEM_H_INCLUDE__

#pragma once

#include "runtime/api/IType.h"
#include <string>

#ifndef DISABLE_FILE_AND_UI
#define DISABLE_FILE_AND_UI 1
#else
#define DISABLE_FILE_AND_UI 1
#endif

#if !DISABLE_FILE_AND_UI

typedef bool(*CallBack_FileOnDownloaded)(LPCTSTR, int);

#include "isgui.h"

class IReadFile
{
public:
	virtual bool IsInPackage() = 0;
	virtual bool IsBufferWrite() = 0;
	virtual bool IsPreparing() = 0;
	virtual bool IsPrepareSuccess() = 0;
	virtual bool IsPrepareFailed() = 0;
	virtual unsigned int GetVersion() = 0;
	virtual int GetPrepareSize() = 0;
	virtual int GetSize() = 0;
	virtual void* GetBuffer() = 0;
	virtual bool Read(void* pBuffer, int nSize) = 0;
	virtual bool Seek(int nOffset, int nOrigin) = 0;
	virtual int Tell() = 0;
	virtual void Close() = 0;
};

class SeedExport IFileList
{
public:
	virtual void load(const char* szDir, DWORD pData) = 0;
	virtual void unload() = 0;
	virtual IReadFile* OpenReadFile(const std::string& filename,uint32_t readType, BOOL bEnableRunTimeUpdate, BOOL bAsynchronismRead) = 0;
};

class SeedExport IFileDownload
{
public:
	virtual BOOL CheckDownloaded(LPCTSTR nameChar, bool bFileType = false, bool bThreadLoad = false) = 0;
	virtual BOOL AddToDownloadQueue(LPCTSTR namePriority, LPCTSTR nameChar, int resIndex, bool bFileType = false, bool bThreadLoad = false) = 0;
	virtual VOID SetOnDownloadedFunction(CallBack_FileOnDownloaded pFunc) = 0;
};

extern "C" SeedExport void AddFileToSizeMap(const char*, unsigned int, DWORD data);

extern "C" SeedExport void SeedLoadLua(ISGUI* pUi);
extern "C" SeedExport ISGUIGraphicProcess* getUiGraphicProcess();

#endif

#endif
