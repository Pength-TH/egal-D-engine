#include "runtime/api/impl/Level.h"

namespace SeedEngine
{
	Level::Level()
	{
		mObjectList.clear();
		mWalkPlaneMgr = new CollisionManager();

		mSceneFileName = "";
		mSceneName = "";
		mbPageObjectsLoadEnd = false;
		mLoadScene = true;
		mTotalNum = 0;
		mCurrentNum = 0;
		mUseLoadFrame = true;
		mLoadLightMap = false;
		fReloadSceneTime = -1;
		m_loaded = false;
	}
	Level::~Level()
	{
		unload();

		//mPageManager->destroyRenderInstance();
		//ENGINE_DELETE(mPageManager);
		//ENGINE_DELETE(mTerrainGroup);
		ENGINE_DELETE(mWalkPlaneMgr);
		//ENGINE_DELETE(mEnvMgr);
		//ENGINE_DELETE(mSkeletonEntityManager);
		mObjectList.clear();
		mLoadLightMap = false;
	}

	BOOL Level::play_scene_anim(LPCTSTR szAniName, BOOL bLoop, Real fTime)
	{
		return TRUE;
	}

	BOOL Level::play_screen_effect(LPCTSTR effectName)
	{
		return TRUE;
	}

	BOOL Level::play_3dsound(LPCTSTR effectName, BOOL bLoop /*= FALSE*/)
	{
		return TRUE;
	}

	BOOL Level::change_bgm(LPCTSTR effectName, BOOL bLoop /*= FALSE*/)
	{
		return TRUE;
	}

	void Level::RefreshPageObject()
	{
		return;
	}

	void Level::CreateAllNoPageObject()
	{
		ObjectList::iterator itObject = mObjectList.begin();
		while (itObject != mObjectList.end())
		{
			//IResourceObject* objPtr = *itObject;
			//objPtr->loadInstance();
			itObject++;
		}

		return;
	}
	void Level::DestoryAllNoPageobject()
	{
		ObjectList::iterator itObject = mObjectList.begin();
		while (itObject != mObjectList.end())
		{
			//IResourceObject* objPtr = *itObject;
			//g_EngineManager->getResourceManager()->destory_resource_object(objPtr);

			itObject++;
		}
		mObjectList.clear();

		itObject = mALLObjectList.begin();
		while (itObject != mALLObjectList.end())
		{
			//IResourceObject* objPtr = *itObject;
			//PackageManager::getSingletonPtr()->removeObject(objPtr);
			//g_EngineManager->getResourceManager()->destory_resource_object(objPtr);
			itObject++;
		}
		mALLObjectList.clear();
		return;
	}

	egal::GameObject* Level::ParseObjectNode(rapidxml::xml_node<>* pObjectElement)
	{
		rapidxml::xml_attribute<> *pTempAttribute = NULL;
		/*IResourceObject* objectPtr = NULL;
		pTempAttribute = pObjectElement->first_attribute("type");
		const std::string& type = pTempAttribute->value();

		objectPtr = g_EngineManager->getResourceManager()->create_resource_object(type);
		if (!objectPtr)
		{
			assert(!objectPtr);
			return NULL;
		}

		pTempAttribute = pObjectElement->first_attribute("name");
		if (pTempAttribute)
		{
			const String& name = pTempAttribute->value();
			objectPtr->setPropertyAsString("name", name);
		}

		rapidxml::xml_node<>* pPropertyElement = pObjectElement->first_node("Property");
		while (pPropertyElement)
		{
			pTempAttribute = pPropertyElement->first_attribute("name");
			if (pTempAttribute)
			{
				const String& name = pTempAttribute->value();

				pTempAttribute = pPropertyElement->first_attribute("value");
				if (pTempAttribute)
				{
					const String& value = pTempAttribute->value();
					objectPtr->setPropertyAsString(name, value);
				}
			}

			pPropertyElement = pPropertyElement->next_sibling("Property");
		}

		if (g_EngineOption.bEnableDownload)
		{
			PackageManager::getSingletonPtr()->addSceneGroupPak(objectPtr->getName(), "", objectPtr->getResourceType());

			rapidxml::xml_node<>* pResourceInfoElement = pObjectElement->first_node("Resource");
			while (pResourceInfoElement)
			{
				pTempAttribute = pResourceInfoElement->first_attribute("type");
				if (pTempAttribute)
				{
					const String& type = pTempAttribute->value();
					pTempAttribute = pResourceInfoElement->first_attribute("name");
					if (pTempAttribute)
					{
						const String& value = pTempAttribute->value();
						if (!value.empty())
						{
							ResourceType eType = (ResourceType)(Ogre::StringConverter::parseUnsignedInt(type));

						}
					}
				}

				pResourceInfoElement = pResourceInfoElement->next_sibling("Resource");
			}
		}*/

		return NULL;
	}

	void Level::load_level(LPCTSTR filename)
	{
		if (m_loaded)
		{
			unload();
		}

		mLoadTime = 0.0;

		mSceneFileName = filename;
		g_EngineOption.bLightTextureDownloaded = FALSE;

		AddSceneResourceLocation();
		
		parseLevel();

		std::string strWalkPlaneMgr = mSceneName.substr(0, mSceneName.length() - 5) + "collision";
		mWalkPlaneMgr->load_collision_file(strWalkPlaneMgr.c_str());


		m_loaded = true;
	}

	void Level::ApplyMainPos(vector3& vecPos)
	{
		//mEnvMgr->ApplyMainPos(vecPos);
	}

	void Level::parseLevel()
	{
		mUseLoadFrame = true;
		mPagesObjectList.clear();
		mALLObjectList.clear();
		mbPageObjectsLoadEnd = false;
		mTotalNum = 0;
		mCurrentNum = 0;

		rapidxml::xml_document<> xmldoc;
		rapidxml::xml_node<> *pRootElement = NULL;

		rapidxml::xml_attribute<> *pTempAttribute = NULL;
		{
			//tTime tt;
			//getLocalTime(&tt);
			//uint16_t hour = tt.wHour;
			//Ogre::StringVector nameAndExt = Ogre::StringUtil::split(mSceneName, ".");
			//Ogre::String name = nameAndExt.at(0);
			//for (Ogre::uint16 i = hour - 1; i <= hour + 1; ++i)
			//{
			//	Ogre::String targetSceneName = name + "#" + Ogre::StringConverter::toString(i) + ".level";

			//	if (Ogre::ResourceGroupManager::getSingleton().resourceExists("General", targetSceneName))
			//	{
			//		stream = Ogre::ResourceGroupManager::getSingleton().openResource(targetSceneName, "General");
			//		break;
			//	}
			//}
		}
		
		egal::FS::IFile* stream = egal::g_file_system->open(egal::g_file_system->getDiskDevice(), "", egal::FS::Mode::READ);

		char* xml_copy = (char*)egal::g_allocator->allocate(stream->size() + 1);
		stream->read(xml_copy, stream->size());
		xml_copy[stream->size()] = '\0';

		xmldoc.parse<0>(&xml_copy[0]);
		int nLightNum = 0;
		pRootElement = xmldoc.first_node("scene");
		if (pRootElement)
		{
			//loadLightmap
			rapidxml::xml_node<>* pEnvironmentElement = pRootElement->first_node("environment");
			if (pEnvironmentElement)
			{
				rapidxml::xml_node<>* pLightmapElement = pEnvironmentElement->first_node("LightmapResourceInfo");
				if (pLightmapElement)
				{
					rapidxml::xml_attribute<> *pTempAttribute = NULL;

					//添加光照图到资源列表
					//1、创建资源组
					//PackageManager::getSingletonPtr()->addSceneGroupPak(mSceneName + "_LightMap", "", RT_SCENE_LIGHTMAP);

					rapidxml::xml_node<>* pResourceElement = pLightmapElement->first_node("Resource");
					while (pResourceElement)
					{
						pTempAttribute = pResourceElement->first_attribute("name");
						if (pTempAttribute)
						{
							const std::string& strValue = pTempAttribute->value();
							
							//PackageManager::getSingletonPtr()->addSceneGroupPak(mSceneName + "_LightMap", strValue, RT_TEXTURE);

						}
						pResourceElement = pResourceElement->next_sibling("Resource");
					}
				}
			}

			//创建角色阴影  和烘焙图的主光源一直
			rapidxml::xml_node<>* pShadowLightElement = pRootElement->first_node("shadowLight");
			if (pShadowLightElement)
			{
				pTempAttribute = pShadowLightElement->first_attribute("direction");
				if (pTempAttribute)
				{
					const std::string& strDirValue = pTempAttribute->value();

					//IResourceObject* objectPtr = g_EngineManager->getResourceManager()->create_resource_object("Light");
					//assert(objectPtr);
					//if (objectPtr)
					//{
					//	objectPtr->setPropertyAsString("name", "shadowLight");
					//	objectPtr->setPropertyAsString("type", "1");
					//	objectPtr->setPropertyAsString("cast shadows", "true");
					//	objectPtr->setPropertyAsString("direction", strDirValue);
					//	objectPtr->setPropertyAsString("power", "0");
					//	objectPtr->setPropertyAsString("diffuse", "0 0 0");
					//	mObjectList.push_back(objectPtr);
					//}
				}
			}

			//rapidxml::xml_node<>* pTerrainGroupElement = pRootElement->first_node("terrain");
			//if (pTerrainGroupElement)
			//{
			//	mTerrainGroup->setTerrainLightNum(0);
			//	mTerrainGroup->parseTerrain(pTerrainGroupElement, mPageManager);
			//}

			rapidxml::xml_node<>* pNodeElementPart = pRootElement->first_node("nodes");
			if (pNodeElementPart)
			{
				rapidxml::xml_node<>* pObjectElement = pNodeElementPart->first_node("node");
				while (pObjectElement)
				{
					//IResourceObject* objectPtr = ParseObjectNode(pObjectElement);

					//if (!objectPtr)
					//{
					//	assert(objectPtr);
					//	pObjectElement = pObjectElement->next_sibling("node");
					//	continue;
					//}

					//if (objectPtr->getType() == "Enviroment")
					//{
					//	mEnvMgr->AddEnvObj(objectPtr);
					//	mObjectList.push_back(objectPtr);
					//}
					//else
					//{
					//	//这里是特殊处理，这是必须首先加载出来的物件
					//	if (objectPtr->getLoadOrder() == 999 || objectPtr->getResourceType() == RT_SCENE_PLANE_WATER)
					//	{
					//		mPageManager->addObject(objectPtr);
					//		mALLObjectList.push_back(objectPtr);
					//	}
					//	else if (objectPtr->determinCreate())
					//	{
					//		mPagesObjectList.push_back(objectPtr);
					//		mALLObjectList.push_back(objectPtr);
					//	}
					//	else
					//	{
					//		g_EngineManager->getResourceManager()->destory_resource_object(objectPtr);
					//		objectPtr = NULL;
					//	}
					//}
					pObjectElement = pObjectElement->next_sibling("node");
				}
			}
		}

		if (mUseLoadFrame)
		{
			mTotalNum = mPagesObjectList.size();
		}
		CreateAllNoPageObject();

		stream->close();

		egal::g_allocator->deallocate(xml_copy);
		xml_copy = 0;

		fReloadSceneTime = 0;
	}

	void Level::unload()
	{
		if (!m_loaded)
			return;
	
		m_loaded = false;

		DestoryAllNoPageobject();
		mWalkPlaneMgr->clear_data();

		RemoveSceneResourceLocation();

		mPagesObjectList.clear();

		SetProcessWorkingSetSize(GetCurrentProcess(), -1, -1);

		mTotalNum = 0;
		mCurrentNum = 0;
		mSceneFileName = "";
	}

	VOID Level::refreshPageObject()
	{

	}

	Real Level::getHeight(float x, float z)
	{
		float fHeight, fWalkHeight;
		//fHeight = mTerrainGroup->getHeightAt(x, z);
		if (mWalkPlaneMgr->get_hight_at_pos(x, z, fWalkHeight))
		{
			if (fHeight < fWalkHeight)
				fHeight = fWalkHeight;
		}
		return fHeight;
	}

	void Level::AddSceneResourceLocation()
	{
	
	}
	void Level::RemoveSceneResourceLocation()
	{
	
	}

	void Level::LoadLightMapOnDownloaded(bool flag)
	{
		
		mLoadLightMap = true;
	}

	bool Level::getIntersects(Real winx, Real winy, vector3& position, vector3* normal, bool allowOutside) const
	{
		const float c_Maxdistance = 100.0f;
		bool bIntersects = false;
		vector3 vecCollisionPos;

		//Ray ray = g_EngineManager->getCamera()->GetOgreCamera()->getCameraToViewportRay(
		//	(winx - g_EngineManager->getViewport()->getActualLeft()) / g_EngineManager->getViewport()->getActualWidth(),
		//	(winy - g_EngineManager->getViewport()->getActualTop()) / g_EngineManager->getViewport()->getActualHeight());

		//bIntersects = mTerrainGroup->getIntersects(ray, position, normal, allowOutside);
		if (mWalkPlaneMgr)
		{
			//if (bIntersects)
			//{
			//	bool bCollsion = false;
			//	bCollsion = mWalkPlaneMgr->build_collision(ray.getOrigin().x, ray.getOrigin().y, ray.getOrigin().z, ray.getDirection().x, ray.getDirection().y, ray.getDirection().z,
			//		position.x, position.y, position.z, vecCollisionPos.x, vecCollisionPos.y, vecCollisionPos.z);
			//	if (bCollsion && position.y < vecCollisionPos.y)
			//		position = vecCollisionPos;
			//}
			//else
			{
				//vector3 vecMaxDisPoint = ray.getOrigin() + c_Maxdistance * ray.getDirection();
				//bool bCollsion = false;
				//bCollsion = mWalkPlaneMgr->build_collision(ray.getOrigin().x, ray.getOrigin().y, ray.getOrigin().z, ray.getDirection().x, ray.getDirection().y, ray.getDirection().z,
				//	vecMaxDisPoint.x, vecMaxDisPoint.y, vecMaxDisPoint.z, vecCollisionPos.x, vecCollisionPos.y, vecCollisionPos.z);
				//if (bCollsion)
				//{
				//	position = vecCollisionPos;
				//	bIntersects = true;
				//}
			}
		}
		return bIntersects;
	}

	bool Level::getPercentage(float& percentage)
	{
		if (!mUseLoadFrame)
		{
			percentage = 1;
			return true;
		}

		if (mTotalNum == 0)
		{
			mTotalNum = 1;
		}
		percentage = mCurrentNum * 1.0 / mTotalNum;

		return mbPageObjectsLoadEnd;
	}

	bool Level::update(Real  fElapsedTime)
	{
	
		return mbPageObjectsLoadEnd;
	}
}