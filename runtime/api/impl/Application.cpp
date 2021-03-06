#include "runtime/api/impl/Application.h"
#include "runtime/api/ISound.h"
#include "runtime/api/impl/SeedEnginePublic.h"
#include "runtime/EngineFramework/camera.h"

namespace SeedEngine
{
	Application::Application()
	{
		m_tileLength = 1 << 5;
		m_tile_count_per_merer = 2;
		m_zero_z_point_in_gfx = 1 << 11;

		mLevel = NULL;
		m_pRaySceneQuery = NULL;

		m_sfAlphaSpeed = 0.3f;
		m_sfDesAlpha = 0.3f;
	}

	Application::~Application()
	{
		clear_hide_object();
		SAFE_DELETE(m_pRaySceneQuery);
	}

	VOID Application::update(Real fElapsedTime)
	{
		transparentHideObject(fElapsedTime);


	}

	BOOL Application::resource_load_progress(Real& precent)
	{
		precent = g_EngineOption.fResourceLoadPercentage;

		return g_EngineOption.bResourceLoaded;
	}

	VOID Application::run()
	{

	}

	VOID Application::pause()
	{
		/**
		* 暂停渲染，其实只是暂停 动作的刷新，特效的发射，声音的播放，等
		* 系统的渲染还是继续
		*/
		g_EngineOption.bRendering = FALSE;
	}

	VOID Application::resume()
	{
		g_EngineOption.bRendering = TRUE;
	}

	VOID Application::resize(int32_t nWidth, int32_t nHeight)
	{


	}

	VOID Application::draw_to_dc(VOID* hDc, int32_t nleft, int32_t nTop, int32_t nWidth, int32_t nHeight)
	{
		//g_EngineManager->draw_to_dc(hDc, nleft, nTop, nWidth, nHeight);
	}

	VOID Application::set_axis_define(uint32_t tileLength /*= 32*/, uint32_t tile_count_per_merer /*= 2*/, uint32_t zero_z_point_in_gfx /*= 2048*/)
	{
		m_tileLength = tileLength;
		m_tile_count_per_merer = tile_count_per_merer;
		m_zero_z_point_in_gfx = m_zero_z_point_in_gfx;
	}

	BOOL Application::axis_trans(axis_type typeSource, const vector3& fvSource, axis_type typeTar, vector3& fvTarget)
	{
		if (!mLevel)
			return false;
	
		BOOL bResult = false;
		int nRetCode = false;
		
		ENGINE_SUCCESS(typeSource == typeTar);

		nRetCode = axis_check_valid(typeSource, fvSource);
		ENGINE_ERROR(nRetCode);
		
		switch (typeSource)
		{
		case EXT_GAME:
		{
			if (typeTar == EXT_GAME_FLAT)
			{
				fvTarget = fvSource;
			}
			else if (typeTar == EXT_ENGINE)
			{
				int nOffsetPages = 0;// (pTerrainGroup ? pTerrainGroup->GetWorldSize() / 2 : 0);
				fvTarget.x = fvSource.x / m_tileLength / m_tile_count_per_merer - nOffsetPages;
				fvTarget.y = (fvSource.z - m_zero_z_point_in_gfx) / m_tileLength / m_tile_count_per_merer;
				fvTarget.z = -fvSource.y / m_tileLength / m_tile_count_per_merer + nOffsetPages;
			}
			else
			{
				ENGINE_ERROR(false);
			}
		}
		break;
		case EXT_GAME_FLAT:
		{
			int nOffsetPages = 0;// (pTerrainGroup ? pTerrainGroup->GetWorldSize() / 2 : 0);
			FLOAT fGfxX = fvSource.x / m_tileLength / m_tile_count_per_merer - nOffsetPages;
			FLOAT fGfxZ = -fvSource.y / m_tileLength / m_tile_count_per_merer + nOffsetPages;

			if (typeTar == EXT_GAME)
			{
				fvTarget.x = fvSource.x;
				fvTarget.y = fvSource.y;
				fvTarget.z = mLevel->getHeight(fGfxX, fGfxZ) * m_tileLength * m_tile_count_per_merer + m_zero_z_point_in_gfx;

				if (fvTarget.z < 0.0f)
					fvTarget.z = 0.0f;

				return TRUE;
			}
			else if (typeTar == EXT_ENGINE)
			{
				fvTarget.x = fGfxX;
				fvTarget.z = fGfxZ;
				fvTarget.y = mLevel->getHeight(fGfxX, fGfxZ);
				return TRUE;
			}
			else
			{
				ENGINE_ERROR(false);
			}
		}
		break;

		case EXT_ENGINE:
		{
			int nOffsetPages = 0;// (pTerrainGroup ? pTerrainGroup->GetWorldSize() / 2 : 0);
			if (typeTar == EXT_GAME || typeTar == EXT_GAME_FLAT)
			{
				fvTarget.x = (fvSource.x + nOffsetPages) * m_tileLength * m_tile_count_per_merer;
				fvTarget.y = -(fvSource.z - nOffsetPages) * m_tileLength * m_tile_count_per_merer;
				fvTarget.z = fvSource.y * m_tileLength * m_tile_count_per_merer + m_zero_z_point_in_gfx;

				if (fvTarget.z < 0.0f)
					fvTarget.z = 0.0f;
			}
			else if (typeTar == EXT_SCREEN)
			{
				egal::Camera* pCamera = NULL;
				egal::float3 vRet;

				int nWidth = 0;
				int nHeight = 0;
				//pCamera = g_EngineManager->getCamera()->GetOgreCamera();
				ENGINE_ERROR(pCamera);

				nRetCode = 1;// pCamera->isVisible(Ogre::Vector3(fvSource.x, fvSource.y, fvSource.z));
				ENGINE_ERROR(nRetCode);

				// x:[-1w, 1w]	z:[-1h, 1h]
				//vRet = pOgreCamera->getProjectionMatrix() * pOgreCamera->getViewMatrix() * Ogre::Vector3(fvSource.x, fvSource.y, fvSource.z);

				//nWidth = g_EngineManager->getViewport()->getActualWidth();
				//nHeight = g_EngineManager->getViewport()->getActualHeight();

				fvTarget.x = ((vRet.x + 1.0f) * nWidth / 2.0f);
				fvTarget.y = ((-vRet.y + 1.0f) * nHeight / 2.0f);
				fvTarget.z = vRet.z;
			}
		}
		break;

		case EXT_SCREEN:
		{
			vector3 vRenderPos;
			nRetCode = mLevel->getIntersects(fvSource.x, fvSource.y, vRenderPos);
			ENGINE_ERROR(nRetCode);

			if (typeTar == EXT_GAME || typeTar == EXT_GAME_FLAT)
			{
				nRetCode = axis_trans(EXT_ENGINE, vector3(vRenderPos.x, vRenderPos.y, vRenderPos.z), EXT_GAME, fvTarget);
				ENGINE_ERROR(nRetCode);
			}
			else if (EXT_ENGINE == typeTar)
			{
				fvTarget = vector3(vRenderPos.x, vRenderPos.y, vRenderPos.z);
			}
		}
		break;
		}

	Exit1:
		bResult = TRUE;
	Exit0:
		return bResult;
	}

	BOOL Application::axis_check_valid(axis_type typeSource, const vector3& fvAxis)
	{
		switch (typeSource)
		{
		case EXT_GAME:
			return TRUE;
			break;
		case EXT_GAME_FLAT:
			return TRUE;
		break;
		case EXT_ENGINE:
			return TRUE;
		break;
		case EXT_SCREEN:
		{
			RECT rect;
			GetClientRect(g_init_params.hWnd, &rect);

			POINT pt;
			pt.x = (INT)fvAxis.x;
			pt.y = (INT)fvAxis.y;

			return (PtInRect(&rect, pt) == TRUE);
		}
		break;
		default:
			return FALSE;
		}

		return FALSE;
	}

	VOID Application::apply_post_effect(LPCTSTR szEffectName, BOOL bEnable)
	{

	}

	SeedSoundEngine::ISoundManager* Application::get_sound_manager()
	{
		return nullptr;// g_EngineManager->getSoundManager();
	}

	VOID Application::set_listener_pos(vector3& vPos, vector3 vOrienation)
	{
		//g_EngineManager->setCharactorPosition(Ogre::Vector3(vPos.x, vPos.y, vPos.z));

		SeedSoundEngine::vector3 vListener, vListenerUp, vListenerFront;
		vListener.fX = vPos.x;
		vListener.fY = vPos.y;
		vListener.fZ = vPos.z;

		vListenerUp.fX = 0.0f;
		vListenerUp.fY = 1.0f;
		vListenerUp.fZ = 0.0f;
		vListenerFront.fX = vOrienation.x;
		vListenerFront.fY = vOrienation.y;
		vListenerFront.fZ = vOrienation.z;

		//if (g_EngineManager->getSoundManager())
		//{
		//	g_EngineManager->getSoundManager()->SetListenerPosition(&vListener);
		//	g_EngineManager->getSoundManager()->SetListenerOrientation(&vListenerFront, &vListenerUp);
		//}
	}

	IGameObject* Application::create_game_object(enum_game_object_type type)
	{
		/*switch (type)
		{
		case EGOT_DEFAULT:
			return new GameObject();
			break;
		case EGOT_DUMMY:
			return ENGINE_NEW_T(GameObject_Dummy, Mem_Scene_Object)();
			break;
		case EGOT_STATIC_MESH:
			break;
		case EGOT_DYNAMIC_MESH:
			break;
		case EGOT_CHARACTOR:
			return ENGINE_NEW_T(GameObject_Actor, Mem_Scene_Object)();
			break;
		case EGOT_EFFECT:
			break;
		case EGOT_SCENE:
			break;
		case EGOT_CAMERA:
			break;
		case EGOT_TRACKER:
			break;
		case EGOT_END:
			break;
		default:
			break;
		}*/
		return NULL;
	}

	VOID Application::destroy_game_object(IGameObject* pNode)
	{
		if (!pNode)
			return;

		assert(pNode);

#if enable_check
		
		delete pNode;

#else
		switch (pNode->GetType())
		{
		/*case EGOT_DEFAULT:
			new ((GameObject*)pNode, GameObject, Mem_Scene_Object);
			break;
		case EGOT_DUMMY:
			new ((GameObject_Dummy*)pNode, GameObject_Dummy, Mem_Scene_Object);
			break;
		case EGOT_STATIC_MESH:
			break;
		case EGOT_DYNAMIC_MESH:
			break;
		case EGOT_CHARACTOR:
			new ((GameObject_Actor*)pNode, GameObject_Actor, Mem_Scene_Object);
			break;
		case EGOT_EFFECT:
			break;
		case EGOT_SCENE:
			break;
		case EGOT_CAMERA:
			break;
		case EGOT_TRACKER:
			break;
		case EGOT_END:
			break;*/
		default:
			break;
		}
#endif
		
		pNode = NULL;
	}

	ILevel* Application::getLevel()
	{
		if (!mLevel)
			mLevel = new Level();

		//g_EngineManager->setCurrentLevel(mLevel);

		return mLevel;
	}
	
	SeedEngine::ICamera* Application::getCamera()
	{
		return NULL;// g_EngineManager->getCamera();
	}

	uint32_t Application::createNodeTrack(LPCTSTR szAniTrackName, BOOL bLoop, const vector3& vPos, const vector3& vRotate)
	{
		uint32_t nodeTrackId = 0;
		egal::Transform transformInfo;
		vector3 fvGfxPos;
		axis_trans(EXT_GAME, vPos, EXT_ENGINE, fvGfxPos);
		transformInfo.pos = egal::float3(fvGfxPos.x, fvGfxPos.y, fvGfxPos.z);
		//transformInfo.rot = egal::Quaternion::(egal::Math::C_RadianToDegree * vRotate.y, egal::float3(0,1,0));
		//nodeTrackId = g_EngineManager->getNodeTrackerManager()->createTracker(szAniTrackName, !!bLoop, transformInfo);
		return nodeTrackId;
	}

	BOOL Application::updateNodeTrack(uint32_t trackID, IGameObject* pNode)
	{
		BOOL bFinish = TRUE;
		egal::Transform transformInfo;
		std::string strBoneName = "default";
		//bFinish = g_EngineManager->getNodeTrackerManager()->getTrackerInfo(trackID, strBoneName, transformInfo);
		//if (pNode != NULL && pNode->GetType() == EGOT_CHARACTOR)
		//{
		//	GameObject_Actor *pRFObject = (GameObject_Actor*)pNode;
		//	DynamicModelObject* pActorImpl = pRFObject->GetWxModel();
		//	if (pActorImpl != NULL)
		//	{
		//		pActorImpl->setPosition(transformInfo.mPosition);
		//		pActorImpl->setOrientation(transformInfo.mRotation);
		//	}
		//}
		return bFinish;
	}

	VOID Application::destroyNodeTrack(uint32_t trackID)
	{
		//g_EngineManager->getNodeTrackerManager()->destoryTracker(trackID);
	}

	VOID Application::create_ui_viewport(uint32_t sceneId, IGameObject* pGameObj, LPCTSTR szCameraName)
	{
		//g_EngineManager->getUiViewportManager()->create_ui_object(sceneId, pGameObj, szCameraName);
	}

	VOID Application::update_ui_viewport(uint32_t sceneId, Real fAspectRatio, Real fDegree)
	{
		//g_EngineManager->getUiViewportManager()->set_camera_param(sceneId, fAspectRatio, fDegree);
	}

	VOID Application::update_ui_viewport(uint32_t sceneId, BOOL bFlag, Real fLeft, Real fTop, Real fWidth, Real fHeight)
	{
		//g_EngineManager->getUiViewportManager()->update_ui(sceneId, bFlag, fLeft, fTop, fWidth, fHeight);
	}

	VOID Application::destory_ui_game_object(uint32_t sceneId, IGameObject* pGameObj)
	{
		//g_EngineManager->getUiViewportManager()->destory_ui_object(sceneId, pGameObj);
	}

	VOID Application::destory_ui_viewport(uint32_t sceneId)
	{
		//g_EngineManager->getUiViewportManager()->destory_ui(sceneId);
	}

	SeedEngine::IGameObject* Application::get_hit_object(uint32_t x, uint32_t y)
	{
		//Ogre::Ray ray = g_EngineManager->getRayFromViewport(x, y);

		//if (!m_pRaySceneQuery)
		//{
		//	m_pRaySceneQuery = g_EngineManager->getSceneManager()->createRayQuery(ray);
		//	m_pRaySceneQuery->setSortByDistance(TRUE);
		//}

		//m_pRaySceneQuery->setRay(ray);
		//m_pRaySceneQuery->setQueryMask(EQF_DEFAULT);

		//GameObjectMap mapRayQuery;
		//const Ogre::RaySceneQueryResult& queryResult = m_pRaySceneQuery->execute();
		//for (Ogre::RaySceneQueryResult::const_iterator it = queryResult.begin(); it != queryResult.end(); ++it)
		//{
		//	Ogre::MovableObject* pMovable = it->movable;
		//	IResourceObject* object = getObjectFromMovable(pMovable);
		//	if (object && object->getData())
		//	{
		//		IGameObject* pEntityNode = (IGameObject*)(object->getData());
		//		if (!pEntityNode) 
		//			continue;
		//		if (!(pEntityNode->GetRayQuery())) 
		//			continue;

		//		if (pEntityNode->GetType() == EGOT_CHARACTOR) // object->getType() == "LogicModel")
		//		{
		//			DynamicModelResourceObject* dynModelObject = static_cast<DynamicModelResourceObject*>(object);

		//			assert(dynModelObject);

		//			DynamicModelObject* model = dynModelObject->getLogicModel();

		//			assert(model);

		//			if (model->isUseExternalBoundingBox() && !(model->rayIntersect(ray)))
		//				continue;
		//		}
		//		if (mapRayQuery.find(pEntityNode->getRayQueryLevel()) != mapRayQuery.end()) 
		//			continue;

		//		mapRayQuery.insert(std::make_pair(pEntityNode->getRayQueryLevel(), pEntityNode));
		//	}
		//}

		//if (!mapRayQuery.empty())
		//	return mapRayQuery.begin()->second;

		return NULL;
	}


	VOID Application::transparentHideObject(Real fTime)
	{
		/*if (!g_EngineManager->getCamera())
			return;

		HideGameObjectMap::iterator itHideObject = mHideObjectSet.begin();
		while (itHideObject != mHideObjectSet.end())
		{
			itHideObject->second.bCheck = false;
			itHideObject++;
		}
		Ogre::Camera* pCamera = g_EngineManager->getCamera()->GetOgreCamera();

		Ogre::Vector3 vecLookAt = g_EngineManager->getCharactorPosition();
		Ogre::Ray ray(vecLookAt, -(pCamera->getDirection()));

		if (!m_pRaySceneQuery)
		{
			m_pRaySceneQuery = g_EngineManager->getSceneManager()->createRayQuery(ray);
			m_pRaySceneQuery->setSortByDistance(TRUE);
		}

		m_pRaySceneQuery->setRay(ray);
		m_pRaySceneQuery->setQueryMask(EQF_BLOCKHIDE);

		Ogre::Vector3 eyepos = pCamera->getPosition();

		GameObjectMap mapRayQuery;
		Ogre::Vector3 vecObjPos;
		const Ogre::RaySceneQueryResult& queryResult = m_pRaySceneQuery->execute();
		for (Ogre::RaySceneQueryResult::const_iterator it = queryResult.begin(); it != queryResult.end(); ++it)
		{
			Ogre::MovableObject* pMovable = it->movable;
			IResourceObject* object = getObjectFromMovable(pMovable);
			if (object)
			{
				object->page_pos(vecObjPos);

				Ogre::Real len = (eyepos - vecLookAt).length() - (eyepos - vecObjPos).length();

				if (object->getCategory() == "StaticObject" && len > 0)
				{

					HideGameObjectMap::iterator itHideObject = mHideObjectSet.find(object);
					if (itHideObject != mHideObjectSet.end())
					{
						itHideObject->second.bCheck = true;
						if (itHideObject->second.fCurrentAlpha > m_sfDesAlpha)
							itHideObject->second.fCurrentAlpha = std::max(itHideObject->second.fCurrentAlpha - fTime * m_sfAlphaSpeed, m_sfDesAlpha);
					}
					else
					{
						HideObjectInfo infoHideObject;
						infoHideObject.bCheck = true;
						infoHideObject.fInitAlpha = Ogre::any_cast<Real>(object->getProperty("transparency"));
						infoHideObject.fCurrentAlpha = std::max(infoHideObject.fInitAlpha + fTime * m_sfAlphaSpeed, m_sfDesAlpha);
						infoHideObject.ptrObj = object;
						mHideObjectSet.insert(std::pair<IResourceObject*, HideObjectInfo>(object, infoHideObject));
					}
				}
			}
		}
		itHideObject = mHideObjectSet.begin();
		while (itHideObject != mHideObjectSet.end())
		{
			if (itHideObject->second.bCheck)
			{
				itHideObject->second.ptrObj->setProperty("transparency", Ogre::Any(itHideObject->second.fCurrentAlpha));
				itHideObject++;
			}
			else
			{
				itHideObject->second.fCurrentAlpha = std::min(itHideObject->second.fCurrentAlpha + fTime * m_sfAlphaSpeed, itHideObject->second.fInitAlpha);
				if (itHideObject->second.fCurrentAlpha >= itHideObject->second.fInitAlpha)
				{
					itHideObject->second.ptrObj->setProperty("transparency", Ogre::Any(itHideObject->second.fInitAlpha));
					itHideObject = mHideObjectSet.erase(itHideObject);
				}
				else
				{
					itHideObject->second.ptrObj->setProperty("transparency", Ogre::Any(itHideObject->second.fCurrentAlpha));
					itHideObject++;
				}
			}
		}*/
	}

	VOID Application::onDeath( BOOL bFlag )
	{
		//g_EngineManager->onDeath(bFlag);
	}

	//when one object translate,and then unload level.whill shut down
	VOID Application::clear_hide_object()
	{
		HideGameObjectMap::iterator itHideObject = mHideObjectSet.begin();
		while (itHideObject != mHideObjectSet.end())
		{
			mHideObjectSet.erase(itHideObject++);
		}
		mHideObjectSet.clear();
	}
}
