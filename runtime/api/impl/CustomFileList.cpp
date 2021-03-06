
#include "runtime/api/impl/CustomFileList.h"
#include "runtime/api/rapidxml-1.13/rapidxml.hpp"
#include "runtime/api/impl/SeedEnginePublic.h"

#if !DISABLE_FILE_AND_UI

void AddFileToSizeMap(const char* name, unsigned int val, DWORD data)
{
	FileArchive* pFileArch = (FileArchive*)(data); 
	if (pFileArch)
		pFileArch->AddFileToSizeMap(name, val);
}

	FileArchive::FileArchive(const String& name, const String& archType)
		: Archive(name, archType)
	{
		m_FileToSizeMap.clear();
	}

	FileArchive::~FileArchive()
	{
		unload();
	}

	void FileArchive::load()
	{
		if (g_init_params.pFileList)
		{
			g_init_params.pFileList->unload();
			g_init_params.pFileList->load(mName.c_str(), (DWORD)this);
		}
	}

	void FileArchive::unload()
	{
		OGRE_LOCK_AUTO_MUTEX;

		m_FileToSizeMap.clear();
	}

	Ogre::DataStreamPtr FileArchive::open(const String& filename, bool readOnly) const
	{
		OGRE_LOCK_AUTO_MUTEX;

		Ogre::String sName(mName);
		sName += filename;
		FileDataStream* pFileDataStream = NULL;
		
		IReadFile* pReadFile = NULL;
		if (g_init_params.pFileList)
			pReadFile = g_init_params.pFileList->OpenReadFile(sName.c_str(), 0, false, false);
		
		if (pReadFile)
		{
			pFileDataStream = ENGINE_NEW FileDataStream(filename, pReadFile);
			if (!pFileDataStream)
			{
				pReadFile->Close();
				pReadFile = NULL;
			}
		}

		if (!pFileDataStream)
		{
			ENGINE_LOG_ERROR("Ogre Open File Faild :" + sName );
		}

		return Ogre::DataStreamPtr(pFileDataStream);
	}

	Ogre::DataStreamPtr FileArchive::create(const String& filename) const
	{
		ENGINE_EXCEPTION("Modification Of File Archives Isn't Support");
	}

	void FileArchive::remove(const String& filename) const
	{

	}

	Ogre::StringVectorPtr FileArchive::list(bool recursive, bool dirs) C_CONST
	{
		OGRE_LOCK_AUTO_MUTEX;

		Ogre::StringVectorPtr ret = Ogre::StringVectorPtr(ENGINE_NEW_T(Ogre::StringVector, Ogre::MEMCATEGORY_GENERAL)(), Ogre::SPFM_DELETE_T);
		FileToSizeMap::const_iterator it = m_FileToSizeMap.begin();
		FileToSizeMap::const_iterator itEnd = m_FileToSizeMap.end();
		while (it != itEnd)
		{
			ret->push_back(it->first);
			++it;
		}

		return ret;
	}

	Ogre::FileInfoListPtr FileArchive::listFileInfo(bool recursive, bool dirs) C_CONST
	{
		OGRE_LOCK_AUTO_MUTEX;

		Ogre::FileInfoListPtr ret = Ogre::FileInfoListPtr(ENGINE_NEW_T(Ogre::FileInfoList, Ogre::MEMCATEGORY_GENERAL)(), Ogre::SPFM_DELETE_T);

		Ogre::FileInfo fileInfo;
		fileInfo.archive = this;
		FileToSizeMap::const_iterator it = m_FileToSizeMap.begin();
		FileToSizeMap::const_iterator itEnd = m_FileToSizeMap.end();
		while (it != itEnd)
		{
			fileInfo.basename = it->first;
			fileInfo.filename = it->first;
			fileInfo.compressedSize = it->second;
			fileInfo.uncompressedSize = it->second;
			ret->push_back(fileInfo);
			++it;
		}

		return ret;
	}

	Ogre::StringVectorPtr FileArchive::find(const String& pattern, bool recursive, bool dirs) C_CONST
	{
		OGRE_LOCK_AUTO_MUTEX;

		Ogre::StringVectorPtr ret = Ogre::StringVectorPtr(ENGINE_NEW_T(Ogre::StringVector, Ogre::MEMCATEGORY_GENERAL)(), Ogre::SPFM_DELETE_T);
		ret->push_back("");
		FileToSizeMap::const_iterator it = m_FileToSizeMap.begin();
		FileToSizeMap::const_iterator itEnd = m_FileToSizeMap.end();
		while (it != itEnd)
		{
			if (Ogre::StringUtil::match(it->first, pattern, false))
			{
				ret->push_back(it->first);
			}
			++it;
		}

		return ret;
	}

	Ogre::FileInfoListPtr FileArchive::findFileInfo(const String& pattern, bool recursive, bool dirs) C_CONST
	{
		OGRE_LOCK_AUTO_MUTEX;

		Ogre::FileInfoListPtr ret = Ogre::FileInfoListPtr(ENGINE_NEW_T(Ogre::FileInfoList, Ogre::MEMCATEGORY_GENERAL)(), Ogre::SPFM_DELETE_T);

		Ogre::FileInfo fileInfo;
		fileInfo.archive = this;

		FileToSizeMap::const_iterator it = m_FileToSizeMap.begin();
		FileToSizeMap::const_iterator itEnd = m_FileToSizeMap.end();
		while (it != itEnd)
		{
			if (Ogre::StringUtil::match(it->first, pattern, false))
			{
				fileInfo.basename = it->first;
				fileInfo.filename = it->first;
				fileInfo.compressedSize = it->second;
				fileInfo.uncompressedSize = it->second;
				ret->push_back(fileInfo);
			}
			++it;
		}

		return ret;
	}

	bool FileArchive::exists(const String& filename) C_CONST
	{
		OGRE_LOCK_AUTO_MUTEX;

		String sFileName = filename;
		Ogre::StringUtil::toLowerCase(sFileName);
		return m_FileToSizeMap.find(sFileName) != m_FileToSizeMap.end();
	}

	time_t FileArchive::getModifiedTime(const String& filename) C_CONST
	{
		return 0;
	}

	FileArchiveFactory::FileArchiveFactory()
	{

	}

	FileArchiveFactory::~FileArchiveFactory()
	{
	}

	const String& FileArchiveFactory::getType(void) const
	{
		static String name = "File";
		return name;
	}

	Ogre::Archive* FileArchiveFactory::createInstance(const String& name)
	{
		return ENGINE_NEW FileArchive(name, "File");
	}

	Ogre::Archive * FileArchiveFactory::createInstance(const String& name, bool readOnly)
	{
		return createInstance(name);
	}

	void FileArchiveFactory::destroyInstance(Ogre::Archive* arch)
	{
		FileArchive* pFileArch = (FileArchive*)arch;
		if (pFileArch)
		{
			ENGINE_DELETE(pFileArch); 
		}
	}

	FileDataStream::FileDataStream(IReadFile* pReadFile) : m_pReadFile(pReadFile)
	{
		assert(m_pReadFile);

		mSize = pReadFile->GetSize();

		g_profile.m_file_data_stream_object.numberCount++;
	}

	FileDataStream::FileDataStream(const String& name, IReadFile* pReadFile) : DataStream(name)
		, m_pReadFile(pReadFile)
	{
		assert(m_pReadFile);

		mSize = pReadFile->GetSize();

		g_profile.m_file_data_stream_object.numberCount++;
	}

	FileDataStream::~FileDataStream()
	{
		close();

		g_profile.m_file_data_stream_object.numberCount--;
	}

	size_t FileDataStream::read(void* buf, size_t count)
	{
		if (m_pReadFile)
		{
			unsigned int nLeft = m_pReadFile->GetSize() - m_pReadFile->Tell();
			if (count > nLeft)
			{
				count = nLeft;
			}
			if (m_pReadFile->Read(buf, count))
			{
				return count;
			}
		}
		return 0;
	}

	size_t FileDataStream::write(void* buf, size_t count)
	{
		return 0;
	}

	void FileDataStream::skip(long count)
	{
		if (m_pReadFile)
		{
			m_pReadFile->Seek(count, SEEK_CUR);
		}
	}

	void FileDataStream::seek(size_t pos)
	{
		if (m_pReadFile)
		{
			m_pReadFile->Seek(pos, SEEK_SET);
		}
	}

	size_t FileDataStream::tell(void) const
	{
		if (m_pReadFile)
		{
			return (size_t)m_pReadFile->Tell();
		}

		return 0;
	}

	bool FileDataStream::canClone(void) const
	{
		if (m_pReadFile && m_pReadFile->GetVersion() == 0x47504A45)
		{
			return false;
		}

		return true;
	}

	bool FileDataStream::eof(void) const
	{
		if (m_pReadFile && m_pReadFile->GetSize() == m_pReadFile->Tell())
		{
			return true;
		}

		return false;
	}

	void FileDataStream::close(void)
	{
		if (m_pReadFile)
		{
			m_pReadFile->Close();
			m_pReadFile = NULL;
		}
	}

	Ogre::uchar* FileDataStream::getPtr(void)
	{
		return (Ogre::uchar*)m_pReadFile->GetBuffer();
	}
//............................................................................................
//............................................................................................
SGOgreImageFile::SGOgreImageFile()
{

}

SGOgreImageFile::~SGOgreImageFile()
{
	if (m_pImage)
	{
		m_pImage = NULL;
	}
}

BOOL SGOgreImageFile::Init(Ogre::Image* pImage)
{
	assert(pImage);

	m_pImage = pImage;
	m_nCurrentReadPosition = 0;
	memcpy(m_uchBuffer, "OIMG", 4);
	memcpy(m_uchBuffer + 4, &m_pImage, sizeof(m_pImage));

	return true;
}

bool SGOgreImageFile::IsInPackage()
{
	return false;
}

bool SGOgreImageFile::IsBufferWrite()
{
	return false;
}

bool SGOgreImageFile::IsPreparing()
{
	return false;
}

bool SGOgreImageFile::IsPrepareSuccess()
{
	return true;
}

bool SGOgreImageFile::IsPrepareFailed()
{
	return false;
}

unsigned int SGOgreImageFile::GetVersion()
{
	return 0x47504A45;
}

int SGOgreImageFile::GetPrepareSize()
{
	return sizeof(m_uchBuffer);
}

int SGOgreImageFile::GetSize()
{
	return sizeof(m_uchBuffer);
}

void* SGOgreImageFile::GetBuffer()
{
	return m_uchBuffer;
}

bool SGOgreImageFile::Read(void* pBuffer, int nSize)
{
	if (pBuffer && (unsigned int)m_nCurrentReadPosition + nSize <= GetSize())
	{
		memcpy(pBuffer, m_uchBuffer + m_nCurrentReadPosition, nSize);
		m_nCurrentReadPosition += nSize;
		return true;
	}

	return false;
}

bool SGOgreImageFile::Seek(int nOffset, int nOrigin)
{
	int nFileSize = GetSize();
	int nPosition = 0;

	switch (nOrigin)
	{
	case SEEK_CUR:
		nPosition = m_nCurrentReadPosition + nOffset;
		if (nPosition >= 0 && nPosition <= nFileSize)
		{
			m_nCurrentReadPosition = nPosition;
			return true;
		}
		return false;
	case SEEK_END:
		nPosition = nFileSize + nOffset;
		if (nPosition >= 0 && nPosition <= nFileSize)
		{
			m_nCurrentReadPosition = nPosition;
			return true;
		}
		return false;
	case SEEK_SET:
		if (nOffset >= 0 && nOffset <= nFileSize)
		{
			m_nCurrentReadPosition = nOffset;
			return true;
		}
		return false;
	default:
		return false;
	}

	return false;
}

int SGOgreImageFile::Tell()
{
	return m_nCurrentReadPosition;
}

void SGOgreImageFile::Close()
{
	delete this;
}

#endif
