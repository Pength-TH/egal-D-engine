/*
-----------------------------------------------------------------------------------------------
This source file is part of the Particle Universe product.

Copyright (c) 2012 Henry van Merode

Usage of this program is licensed under the terms of the Particle Universe Commercial License.
You can find a copy of the Commercial License in the Particle Universe package.
-----------------------------------------------------------------------------------------------
*/

#include "PCH.h"

#ifndef PARTICLE_UNIVERSE_EXPORTS
#define PARTICLE_UNIVERSE_EXPORTS
#endif

#include "ScriptCompiler.h"
#include "OgreRoot.h"
#include "OgreHighLevelGpuProgramManager.h"
#include "OgreMaterialManager.h"
#include "OgreHardwarePixelBuffer.h"
#include "OgreOverlayManager.h"
#include "OgreOverlayContainer.h"


#include "seed_memory_leak_check.h" //the last include head file,no include after this line
template<> ParticleUniverse::ParticleSystemManager* ParticleUniverse::Singleton<ParticleUniverse::ParticleSystemManager>::msSingleton = 0;
namespace egal
{
	ParticleSystemManager::ParticleSystemManager (void) :
		mDepthTextureName(StringUtil::BLANK),
		mDepthMaterialName(StringUtil::BLANK),
		mDepthVertexName(StringUtil::BLANK),
		mDepthFragmentName(StringUtil::BLANK),
		mLastCreatedParticleSystemTemplateName(StringUtil::BLANK),
		mDepthMap(0),
		mDepthTechnique(0),
		mDepthPass(0),
		mDepthMapExtern(false),
		mDebugPanel(0),
		mDebugOverlay(0),
		mAutoLoadMaterials(false),
		mDepthScale(1.0f)
	{
		// Initialise script deserialisation
		ScriptCompilerManager::getSingleton().addScriptPattern("*.pua");
		ScriptCompilerManager::getSingleton().addScriptPattern("*.pu");
		mBuiltinScriptTranslatorManager = PU_NEW BuiltinScriptTranslatorManager();
		ScriptCompilerManager::getSingleton().addTranslatorManager(mBuiltinScriptTranslatorManager);

		// Initialise script serialisation
		mParticleScriptSerializer = PU_NEW ParticleScriptSerializer();

		// Other
		mParticleSystemFactory = PU_NEW ParticleSystemFactory();
		Ogre::Root::getSingleton().addMovableObjectFactory(mParticleSystemFactory);

		mBoxSetFactory = PU_NEW BoxSetFactory();
		Ogre::Root::getSingleton().addMovableObjectFactory(mBoxSetFactory);

		mSphereSetFactory = PU_NEW SphereSetFactory();
		Ogre::Root::getSingleton().addMovableObjectFactory(mSphereSetFactory);
	}
	//-----------------------------------------------------------------------
	ParticleSystemManager::~ParticleSystemManager (void)
	{
		// Remove all template + aliasses
		destroyAllParticleSystemTemplates();

		/** Clear the aliasmap and destroy the objects in it 
		*/ 
		destroyAllAliasses(); 

		// Delete the Serializer
		if (mParticleScriptSerializer)
		{
			PU_DELETE mParticleScriptSerializer;
			mParticleScriptSerializer = 0;
		}

		if (mBuiltinScriptTranslatorManager)
		{
			PU_DELETE mBuiltinScriptTranslatorManager;
			mBuiltinScriptTranslatorManager = 0;
		}

		// Delete the factories
		if (mParticleSystemFactory)
		{
			Ogre::Root::getSingleton().removeMovableObjectFactory(mParticleSystemFactory);
			PU_DELETE mParticleSystemFactory;
			mParticleSystemFactory = 0;
		}

		if (mBoxSetFactory)
		{
			Ogre::Root::getSingleton().removeMovableObjectFactory(mBoxSetFactory);
			PU_DELETE mBoxSetFactory;
			mBoxSetFactory = 0;
		}

		if (mSphereSetFactory)
		{
			Ogre::Root::getSingleton().removeMovableObjectFactory(mSphereSetFactory);
			PU_DELETE mSphereSetFactory;
			mSphereSetFactory = 0;
		}
	}
	//-----------------------------------------------------------------------
	void ParticleSystemManager::removeAndDestroyDanglingSceneNodes(Ogre::SceneNode* sceneNode)
	{
		// Only destroy the SceneNodes that start with ""
		if (sceneNode)
		{
			Ogre::SceneNode::ChildNodeIterator iterator = sceneNode->getChildIterator();
			while(iterator.hasMoreElements())
			{
				Ogre::SceneNode* childSceneNode = static_cast<Ogre::SceneNode*>(iterator.getNext());
				String name = childSceneNode->getName();
				if(name.compare(0, 16, "") == 0)
				{
					sceneNode->removeAndDestroyChild(name);
				}
			}
		}
	}
	//-----------------------------------------------------------------------
	void ParticleSystemManager::destroyAllParticleSystemTemplates() 
	{ 
		// Delete all Particle System Templates 
		ParticleSystemTemplateMap::iterator t;
		for (t = mParticleSystemTemplates.begin(); t != mParticleSystemTemplates.end(); ++t)
		{
			PU_DELETE t->second;
		}
		mParticleSystemTemplates.clear();
	}
	//-----------------------------------------------------------------------
	BoxSet* ParticleSystemManager::createBoxSet(const String& name,
		Ogre::SceneManager* sceneManager,
		const uint poolSize)
	{
		Ogre::NameValuePairList params;
		params["poolSize"] = StringConverter::toString(poolSize);
	
		return static_cast<BoxSet*>(
			sceneManager->createMovableObject(name, BoxSetFactory::PU_FACTORY_TYPE_NAME,
			&params));
	}
	//-----------------------------------------------------------------------
	void ParticleSystemManager::destroyBoxSet(BoxSet* boxSet, Ogre::SceneManager* sceneManager)
	{
		sceneManager->destroyMovableObject(boxSet);
	}
	//-----------------------------------------------------------------------
	SphereSet* ParticleSystemManager::createSphereSet(const String& name,
		Ogre::SceneManager* sceneManager,
		const uint poolSize)
	{
		Ogre::NameValuePairList params;
		params["poolSize"] = StringConverter::toString(poolSize);
	
		return static_cast<SphereSet*>(
			sceneManager->createMovableObject(name, SphereSetFactory::PU_FACTORY_TYPE_NAME,
			&params));
	}
	//-----------------------------------------------------------------------
	void ParticleSystemManager::destroySphereSet(SphereSet* sphereSet, Ogre::SceneManager* sceneManager)
	{
		sceneManager->destroyMovableObject(sphereSet);
	}
	//-----------------------------------------------------------------------
	void ParticleSystemManager::registerAttachable(Attachable* attachable, Ogre::SceneManager* sceneManager)
	{
		sceneManager->injectMovableObject(attachable);
	}
	//-----------------------------------------------------------------------
	void ParticleSystemManager::unregisterAttachable(Attachable* attachable, Ogre::SceneManager* sceneManager)
	{
		// Note, that the Attachable is not destroyed
		sceneManager->extractMovableObject(attachable);
	}
	//-----------------------------------------------------------------------
	void ParticleSystemManager::addEmitterFactory(ParticleEmitterFactory* factory)
	{
		String type = factory->getEmitterType();
		mEmitterFactories[HashCodeString(type)] = factory;
		//LogManager::getSingleton().logMessage(": Particle Emitter Type '" + type + "' registered");
	}
	//-----------------------------------------------------------------------
	ParticleEmitterFactory* ParticleSystemManager::getEmitterFactory(const String& emitterType)
	{
		EmitterFactoryMap::iterator it = mEmitterFactories.find(HashCodeString(emitterType));
		if (it != mEmitterFactories.end())
		{
			return it->second;
		}
		return 0;
	}
	//-----------------------------------------------------------------------
	void ParticleSystemManager::removeEmitterFactory(const String& emitterType)
	{
		EmitterFactoryMap::iterator it = mEmitterFactories.find(HashCodeString(emitterType));
		if (it == mEmitterFactories.end())
		{
			OGRE_LOG("PU: Cannot find emitter factory to remove. ParticleSystemManager::removeEmitterFactory");
		}

		mEmitterFactories.erase(it);
	}
	//-----------------------------------------------------------------------
	void ParticleSystemManager::destroyEmitterFactory(const String& emitterType)
	{
		EmitterFactoryMap::iterator it = mEmitterFactories.find(HashCodeString(emitterType));
		if (it == mEmitterFactories.end())
		{
			OGRE_LOG("PU: Cannot find emitter factory to destroy. ParticleSystemManager::destroyEmitterFactory");
		}

		PU_DELETE it->second;
		mEmitterFactories.erase(it);
	}
	//-----------------------------------------------------------------------
	ParticleEmitter* ParticleSystemManager::createEmitter(const String& emitterType)
	{
		// Locate emitter type
		EmitterFactoryMap::iterator it = mEmitterFactories.find(HashCodeString(emitterType));

		if (it == mEmitterFactories.end())
		{
			OGRE_LOG("PU: Cannot find requested emitter type. ParticleSystemManager::createEmitter");
			return NULL;
		}

		return it->second->createEmitter();
	}
	//-----------------------------------------------------------------------
	ParticleEmitter* ParticleSystemManager::cloneEmitter(ParticleEmitter* emitter)
	{
		if (!emitter)
			return 0;

		ParticleEmitter* clonedEmitter = createEmitter(emitter->getEmitterType());
		emitter->copyAttributesTo(clonedEmitter);
		return clonedEmitter;
	}
	//-----------------------------------------------------------------------
	void ParticleSystemManager::destroyEmitter(ParticleEmitter* emitter)
	{
		// Locate emitter type
		EmitterFactoryMap::iterator it = mEmitterFactories.find(HashCodeString(emitter->getEmitterType()));

		if (it == mEmitterFactories.end())
		{
			OGRE_LOG("PU: Cannot find requested emitter type. ParticleSystemManager::destroyEmitter");
		}

		it->second->destroyEmitter(emitter);
	}
	//-----------------------------------------------------------------------
	ParticleTechnique* ParticleSystemManager::createTechnique(void)
	{
		ParticleTechnique* technique = PU_NEW_T(ParticleTechnique, MEMCATEGORY_SCENE_OBJECTS)();
		technique->setRenderer(createRenderer("Billboard")); // Set default renderer
		return technique;
	}
	//-----------------------------------------------------------------------
	ParticleTechnique* ParticleSystemManager::cloneTechnique(ParticleTechnique* technique)
	{
		if (!technique)
			return 0;

		ParticleTechnique* clonedTechnique = createTechnique();
		technique->copyAttributesTo(clonedTechnique);
		return clonedTechnique;
	}
	//-----------------------------------------------------------------------
	void ParticleSystemManager::destroyTechnique(ParticleTechnique* technique)
	{
		// Deletion must be done by the same object that also created it
		PU_DELETE_T(technique, ParticleTechnique, MEMCATEGORY_SCENE_OBJECTS);
	}
	//-----------------------------------------------------------------------
	void ParticleSystemManager::addAffectorFactory(ParticleAffectorFactory* factory)
	{
		String type = factory->getAffectorType();
		mAffectorFactories[HashCodeString(type)] = factory;
		//LogManager::getSingleton().logMessage(": Particle Affector Type '" + type + "' registered");
	}
	//-----------------------------------------------------------------------
	ParticleAffectorFactory* ParticleSystemManager::getAffectorFactory(const String& affectorType)
	{
		AffectorFactoryMap::iterator it = mAffectorFactories.find(HashCodeString(affectorType));
		if (it != mAffectorFactories.end())
		{
			return it->second;
		}
		return 0;
	}
	//-----------------------------------------------------------------------
	void ParticleSystemManager::removeAffectorFactory(const String& affectorType)
	{
		AffectorFactoryMap::iterator it = mAffectorFactories.find(HashCodeString(affectorType));
		if (it == mAffectorFactories.end())
		{
			OGRE_LOG("PU: Cannot find affector factory to remove. ParticleSystemManager::removeAffectorFactory");
		}

		mAffectorFactories.erase(it);
	}
	//-----------------------------------------------------------------------
	void ParticleSystemManager::destroyAffectorFactory(const String& affectorType)
	{
		AffectorFactoryMap::iterator it = mAffectorFactories.find(HashCodeString(affectorType));
		if (it == mAffectorFactories.end())
		{
			OGRE_LOG("PU: Cannot find affector factory to destroy. ParticleSystemManager::destroyAffectorFactory");
		}

		PU_DELETE it->second;
		mAffectorFactories.erase(it);
	}
	//-----------------------------------------------------------------------
	ParticleAffector* ParticleSystemManager::createAffector(const String& affectorType)
	{
		// Locate affector type
		AffectorFactoryMap::iterator it = mAffectorFactories.find(HashCodeString(affectorType));

		if (it == mAffectorFactories.end())
		{
			OGRE_LOG("PU: Cannot find requested affector type. ParticleSystemManager::createAffector");
			return NULL;
		}

		return it->second->createAffector();
	}
	//-----------------------------------------------------------------------
	ParticleAffector* ParticleSystemManager::cloneAffector(ParticleAffector* affector)
	{
		if (!affector)
			return 0;

		ParticleAffector* clonedAffector = createAffector(affector->getAffectorType());
		affector->copyAttributesTo(clonedAffector);
		return clonedAffector;
	}
	//-----------------------------------------------------------------------
	void ParticleSystemManager::destroyAffector(ParticleAffector* affector)
	{
		// Locate affector type
		AffectorFactoryMap::iterator it = mAffectorFactories.find(HashCodeString(affector->getAffectorType()));

		if (it == mAffectorFactories.end())
		{
			OGRE_LOG("PU: Cannot find requested affector type. ParticleSystemManager::destroyAffector");
		}

		it->second->destroyAffector(affector);
	}
	//-----------------------------------------------------------------------
	void ParticleSystemManager::addObserverFactory(ParticleObserverFactory* factory)
	{
		String type = factory->getObserverType();
		mObserverFactories[HashCodeString(type)] = factory;
		//LogManager::getSingleton().logMessage(": Particle Observer Type '" + type + "' registered");
	}
	//-----------------------------------------------------------------------
	ParticleObserverFactory* ParticleSystemManager::getObserverFactory(const String& observerType)
	{
		ObserverFactoryMap::iterator it = mObserverFactories.find(HashCodeString(observerType));
		if (it != mObserverFactories.end())
		{
			return it->second;
		}
		return 0;
	}
	//-----------------------------------------------------------------------
	void ParticleSystemManager::removeObserverFactory(const String& observerType)
	{
		ObserverFactoryMap::iterator it = mObserverFactories.find(HashCodeString(observerType));
		if (it == mObserverFactories.end())
		{
			OGRE_LOG("PU: Cannot find observer factory to remove. ParticleSystemManager::removeObserverFactory");
		}

		mObserverFactories.erase(it);
	}
	//-----------------------------------------------------------------------
	void ParticleSystemManager::destroyObserverFactory(const String& observerType)
	{
		ObserverFactoryMap::iterator it = mObserverFactories.find(HashCodeString(observerType));
		if (it == mObserverFactories.end())
		{
			OGRE_LOG("PU: Cannot find observer factory to destroy. ParticleSystemManager::destroyObserverFactory");
		}

		PU_DELETE it->second;
		mObserverFactories.erase(it);
	}
	//-----------------------------------------------------------------------
	ParticleObserver* ParticleSystemManager::createObserver(const String& observerType)
	{
		// Locate Observer type
		ObserverFactoryMap::iterator it = mObserverFactories.find(HashCodeString(observerType));

		if (it == mObserverFactories.end())
		{
			OGRE_LOG("PU: Cannot find requested observer type. ParticleSystemManager::createObserver");
			return NULL;
		}

		return it->second->createObserver();
	}
	//-----------------------------------------------------------------------
	ParticleObserver* ParticleSystemManager::cloneObserver(ParticleObserver* observer)
	{
		if (!observer)
			return 0;

		ParticleObserver* clonedObserver = createObserver(observer->getObserverType());
		observer->copyAttributesTo(clonedObserver);
		return clonedObserver;
	}
	//-----------------------------------------------------------------------
	void ParticleSystemManager::destroyObserver(ParticleObserver* observer)
	{
		// Locate Observer type
		ObserverFactoryMap::iterator it = mObserverFactories.find(HashCodeString(observer->getObserverType()));

		if (it == mObserverFactories.end())
		{
			OGRE_LOG("PU: Cannot find requested observer type. ParticleSystemManager::destroyObserver");
		}

		it->second->destroyObserver(observer);
	}
	//-----------------------------------------------------------------------
	void ParticleSystemManager::addEventHandlerFactory(ParticleEventHandlerFactory* factory)
	{
		String type = factory->getEventHandlerType();
		mEventHandlerFactories[HashCodeString(type)] = factory;
		//LogManager::getSingleton().logMessage(": Particle EventHandler Type '" + type + "' registered");
	}
	//-----------------------------------------------------------------------
	ParticleEventHandlerFactory* ParticleSystemManager::getEventHandlerFactory(const String& eventHandlerType)
	{
		EventHandlerFactoryMap::iterator it = mEventHandlerFactories.find(HashCodeString(eventHandlerType));
		if (it != mEventHandlerFactories.end())
		{
			return it->second;
		}
		return 0;
	}
	//-----------------------------------------------------------------------
	void ParticleSystemManager::removeEventHandlerFactory(const String& eventHandlerType)
	{
		EventHandlerFactoryMap::iterator it = mEventHandlerFactories.find(HashCodeString(eventHandlerType));
		if (it == mEventHandlerFactories.end())
		{
			OGRE_LOG("PU: Cannot find event handler factory to remove. ParticleSystemManager::removeEventHandlerFactory");
		}

		mEventHandlerFactories.erase(it);
	}
	//-----------------------------------------------------------------------
	void ParticleSystemManager::destroyEventHandlerFactory(const String& eventHandlerType)
	{
		EventHandlerFactoryMap::iterator it = mEventHandlerFactories.find(HashCodeString(eventHandlerType));
		if (it == mEventHandlerFactories.end())
		{
			OGRE_LOG("PU: Cannot find event handler factory to destroy. ParticleSystemManager::destroyEventHandlerFactory");
		}

		PU_DELETE it->second;
		mEventHandlerFactories.erase(it);
	}
	//-----------------------------------------------------------------------
	ParticleEventHandler* ParticleSystemManager::createEventHandler(const String& eventHandlerType)
	{
		// Locate EventHandler type
		EventHandlerFactoryMap::iterator it = mEventHandlerFactories.find(HashCodeString(eventHandlerType));

		if (it == mEventHandlerFactories.end())

		{
			OGRE_LOG("PU: Cannot find requested event handler type. ParticleSystemManager::createEventHandler");
			return NULL;
		}

		return it->second->createEventHandler();
	}
	//-----------------------------------------------------------------------
	ParticleEventHandler* ParticleSystemManager::cloneEventHandler(ParticleEventHandler* eventHandler)
	{
		if (!eventHandler)
			return 0;

		ParticleEventHandler* clonedEventHandler = createEventHandler(eventHandler->getEventHandlerType());
		eventHandler->copyAttributesTo(clonedEventHandler);
		return clonedEventHandler;
	}
	//-----------------------------------------------------------------------
	void ParticleSystemManager::destroyEventHandler(ParticleEventHandler* eventHandler)
	{
		// Locate EventHandler type
		EventHandlerFactoryMap::iterator it = mEventHandlerFactories.find(HashCodeString(eventHandler->getEventHandlerType()));

		if (it == mEventHandlerFactories.end())
		{
			OGRE_LOG("PU: Cannot find requested event handler type. ParticleSystemManager::destroyEventHandler");

		}

		it->second->destroyEventHandler(eventHandler);
	}
	//-----------------------------------------------------------------------
	void ParticleSystemManager::addRendererFactory(ParticleRendererFactory* factory)
	{
		String type = factory->getRendererType();
		mRendererFactories[HashCodeString(type)] = factory;
		//LogManager::getSingleton().logMessage(": Particle Renderer Type '" + type + "' registered");
	}
	//-----------------------------------------------------------------------
	ParticleRendererFactory* ParticleSystemManager::getRendererFactory(const String& rendererType)
	{
		RendererFactoryMap::iterator it = mRendererFactories.find(HashCodeString(rendererType));
		if (it != mRendererFactories.end())
		{
			return it->second;
		}
		return 0;
	}
	//-----------------------------------------------------------------------
	void ParticleSystemManager::removeRendererFactory(const String& rendererType)
	{
		RendererFactoryMap::iterator it = mRendererFactories.find(HashCodeString(rendererType));
		if (it == mRendererFactories.end())
		{
			OGRE_LOG("PU: Cannot find renderer factory to remove. ParticleSystemManager::removeRendererFactory");
		}

		mRendererFactories.erase(it);
	}
	//-----------------------------------------------------------------------
	void ParticleSystemManager::destroyRendererFactory(const String& rendererType)
	{
		RendererFactoryMap::iterator it = mRendererFactories.find(HashCodeString(rendererType));
		if (it == mRendererFactories.end())
		{
			OGRE_LOG("PU: Cannot find renderer factory to destroy. ParticleSystemManager::destroyRendererFactory");
		}

		PU_DELETE it->second;
		mRendererFactories.erase(it);
	}
	//-----------------------------------------------------------------------
	ParticleRenderer* ParticleSystemManager::createRenderer(const String& rendererType)
	{
		// Locate renderer type
		RendererFactoryMap::iterator it = mRendererFactories.find(HashCodeString(rendererType));

		if (it == mRendererFactories.end())
		{
			OGRE_LOG("PU: Cannot find requested renderer type. ParticleSystemManager::createRenderer");
			return NULL;
		}

		return it->second->createRenderer();
	}
	//-----------------------------------------------------------------------
	ParticleRenderer* ParticleSystemManager::cloneRenderer(ParticleRenderer* renderer)
	{
		if (!renderer)
			return 0;

		ParticleRenderer* clonedRenderer = createRenderer(renderer->getRendererType());
		renderer->copyAttributesTo(clonedRenderer);
		return clonedRenderer;
	}
	//-----------------------------------------------------------------------
	void ParticleSystemManager::destroyRenderer(ParticleRenderer* renderer)
	{
		// Locate renderer type
		RendererFactoryMap::iterator it = mRendererFactories.find(HashCodeString(renderer->getRendererType()));

		if (it == mRendererFactories.end())
		{
			OGRE_LOG("PU: Cannot find requested renderer type. ParticleSystemManager::destroyRenderer");
		}

		it->second->destroyRenderer(renderer);
	}
	//-----------------------------------------------------------------------
	void ParticleSystemManager::addExternFactory(ExternFactory* factory)
	{
		String type = factory->getExternType();
		mExternFactories[HashCodeString(type)] = factory;
		//LogManager::getSingleton().logMessage(": Particle Extern Type '" + type + "' registered");
	}
	//-----------------------------------------------------------------------
	ExternFactory* ParticleSystemManager::getExternFactory(const String& externType)
	{
		ExternFactoryMap::iterator it = mExternFactories.find(HashCodeString(externType));
		if (it != mExternFactories.end())
		{
			return it->second;
		}
		return 0;
	}
	//-----------------------------------------------------------------------
	void ParticleSystemManager::removeExternFactory(const String& externType)
	{
		ExternFactoryMap::iterator it = mExternFactories.find(HashCodeString(externType));
		if (it == mExternFactories.end())
		{
			OGRE_LOG("PU: Cannot find extern factory to remove. ParticleSystemManager::removeExternFactory");
		}

		mExternFactories.erase(it);
	}
	//-----------------------------------------------------------------------
	void ParticleSystemManager::destroyExternFactory(const String& externType)
	{
		ExternFactoryMap::iterator it = mExternFactories.find(HashCodeString(externType));
		if (it == mExternFactories.end())
		{
			OGRE_LOG("PU: Cannot find extern factory to destroy. ParticleSystemManager::destroyExternFactory");
		}

		PU_DELETE it->second;
		mExternFactories.erase(it);
	}
	//-----------------------------------------------------------------------
	Extern* ParticleSystemManager::createExtern(const String& externType)
	{
		// Locate Extern type
		ExternFactoryMap::iterator it = mExternFactories.find(HashCodeString(externType));

		if (it == mExternFactories.end())
		{
			OGRE_LOG("PU: Cannot find requested extern type. ParticleSystemManager::createExtern");
			return NULL;
		}

		return it->second->createExtern();
	}
	//-----------------------------------------------------------------------
	Extern* ParticleSystemManager::cloneExtern(Extern* externObject)
	{
		if (!externObject)
			return 0;

		Extern* clonedExtern = createExtern(externObject->getExternType());
		externObject->copyAttributesTo(clonedExtern);
		return clonedExtern;
	}
	//-----------------------------------------------------------------------
	void ParticleSystemManager::destroyExtern(Extern* externObject)
	{
		// Locate extern type
		ExternFactoryMap::iterator it = mExternFactories.find(HashCodeString(externObject->getExternType()));

		if (it == mExternFactories.end())
		{
			OGRE_LOG("PU: Cannot find requested extern type. ParticleSystemManager::destroyExtern");
		}

		it->second->destroyExtern(externObject);
	}
	//-----------------------------------------------------------------------
	void ParticleSystemManager::addBehaviourFactory(ParticleBehaviourFactory* factory)
	{
		String type = factory->getBehaviourType();
		mBehaviourFactories[HashCodeString(type)] = factory;
		LogManager::getSingleton().logMessage(": Particle Behaviour Type '" + type + "' registered");
	}
	//-----------------------------------------------------------------------
	ParticleBehaviourFactory* ParticleSystemManager::getBehaviourFactory(const String& behaviourType)
	{
		BehaviourFactoryMap::iterator it = mBehaviourFactories.find(HashCodeString(behaviourType));
		if (it != mBehaviourFactories.end())
		{
			return it->second;
		}
		return 0;
	}
	//-----------------------------------------------------------------------
	void ParticleSystemManager::removeBehaviourFactory(const String& behaviourType)
	{
		BehaviourFactoryMap::iterator it = mBehaviourFactories.find(HashCodeString(behaviourType));
		if (it == mBehaviourFactories.end())
		{
			OGRE_LOG("PU: Cannot find behaviour factory to remove. ParticleSystemManager::removeBehaviourFactory");
		}

		mBehaviourFactories.erase(it);
	}
	//-----------------------------------------------------------------------
	void ParticleSystemManager::destroyBehaviourFactory(const String& behaviourType)
	{
		BehaviourFactoryMap::iterator it = mBehaviourFactories.find(HashCodeString(behaviourType));
		if (it == mBehaviourFactories.end())
		{
			OGRE_LOG("PU: Cannot find behaviour factory to destroy. ParticleSystemManager::destroyBehaviourFactory");
		}

		PU_DELETE it->second;
		mBehaviourFactories.erase(it);
	}
	//-----------------------------------------------------------------------
	ParticleBehaviour* ParticleSystemManager::createBehaviour(const String& behaviourType)
	{
		// Locate Behaviour type
		BehaviourFactoryMap::iterator it = mBehaviourFactories.find(HashCodeString(behaviourType));

	if (it == mBehaviourFactories.end())
	{
		OGRE_LOG("PU: Cannot find requested behaviour type. ParticleSystemManager::createBehaviour");
		return NULL;
	}

		return it->second->createBehaviour();
	}
	//-----------------------------------------------------------------------
	ParticleBehaviour* ParticleSystemManager::cloneBehaviour(ParticleBehaviour* behaviour)
	{
		if (!behaviour)
			return 0;

		ParticleBehaviour* clonedBehaviour = createBehaviour(behaviour->getBehaviourType());
		behaviour->copyAttributesTo(clonedBehaviour);
		return clonedBehaviour;
	}
	//-----------------------------------------------------------------------
	void ParticleSystemManager::destroyBehaviour(ParticleBehaviour* behaviour)
	{
		// Locate Behaviour type
		BehaviourFactoryMap::iterator it = mBehaviourFactories.find(HashCodeString(behaviour->getBehaviourType()));

		if (it == mBehaviourFactories.end())
		{
			OGRE_LOG("PU: Cannot find requested behaviour type. ParticleSystemManager::destroyBehaviour");
		}

		it->second->destroyBehaviour(behaviour);
	}
	//-----------------------------------------------------------------------
	ParticleSystem* ParticleSystemManager::createParticleSystemTemplate(const String& name, const String& resourceGroupName)
	{
		OGRE_LOCK_AUTO_MUTEX;
		// Validate name and add a prefix if needed.
		String expName = name;
		if (mParticleSystemTemplates.find(HashCodeString(expName)) != mParticleSystemTemplates.end())
		{
			String message = ": Particle System template '" + expName + "' already exists; ";
#ifdef REPLACE_TEMPLATE_IF_EXISTING
			// Not defined, so delete the old template
			destroyParticleSystemTemplate(expName);
			LogManager::getSingleton().logMessage(message + "old template deleted.");
#else
			LogManager::getSingleton().logMessage(message + "create copy.");
			expName = String("CopyOf") + expName;
			return NULL;
#endif
		}

		ParticleSystem* particleSystemTemplate = PU_NEW ParticleSystem(expName, resourceGroupName);

		addParticleSystemTemplate(expName, particleSystemTemplate);
		mLastCreatedParticleSystemTemplateName = expName;

		return particleSystemTemplate;
	}
	//-----------------------------------------------------------------------
	bool ParticleSystemManager::hasParticleSystemTemplate(const String& name)
	{
		OGRE_LOCK_AUTO_MUTEX;
		// Validate name and add a prefix if needed.
		String expName = name;
		if (mParticleSystemTemplates.find(HashCodeString(expName)) != mParticleSystemTemplates.end())
		{
			String message = ": Particle System template '" + expName + "' already exists; ";
			LogManager::getSingleton().logMessage(message);
			return true;
		}
		return false;
	}
	//-----------------------------------------------------------------------
	void ParticleSystemManager::replaceParticleSystemTemplate(const String& name, ParticleSystem* system)
	{
		ParticleSystem* systemTemplate = getParticleSystemTemplate(name);
		if (systemTemplate)
		{
			*systemTemplate = *system;
			system->copyAttributesTo(systemTemplate);
		}
	}
	//-----------------------------------------------------------------------
	const String& ParticleSystemManager::getLastCreatedParticleSystemTemplateName(void)
	{
		return mLastCreatedParticleSystemTemplateName;
	}
	//-----------------------------------------------------------------------
	void ParticleSystemManager::addParticleSystemTemplate(const String& name, ParticleSystem* systemTemplate)
	{
		OGRE_LOCK_AUTO_MUTEX;
		// Validate name
		if (mParticleSystemTemplates.find(HashCodeString(name)) != mParticleSystemTemplates.end())
		{
		//	EXCEPT(Exception::ERR_DUPLICATE_ITEM, 
		//		"PU: ParticleSystem template with name '" + name + "' already exists.", 
		//		"ParticleSystemManager::addParticleSystemTemplate");
		}

		mParticleSystemTemplates[HashCodeString(name)] = systemTemplate;
	}
	//-----------------------------------------------------------------------
	ParticleSystem* ParticleSystemManager::getParticleSystemTemplate(const String& templateName)
	{
		OGRE_LOCK_AUTO_MUTEX;
		ParticleSystemTemplateMap::iterator i = mParticleSystemTemplates.find(HashCodeString(templateName));
		if (i != mParticleSystemTemplates.end())
		{
			return i->second;
		}
	
		return 0;
	}
	//-----------------------------------------------------------------------
	void ParticleSystemManager::destroyParticleSystemTemplate(const String& templateName)
	{
		ParticleSystemTemplateMap::iterator i = mParticleSystemTemplates.find(HashCodeString(templateName));
		if (i != mParticleSystemTemplates.end())
		{
			PU_DELETE (i->second);
			mParticleSystemTemplates.erase(i);
		}
	}
	//-----------------------------------------------------------------------
	void ParticleSystemManager::particleSystemTemplateNames(vector<HashString>& v)
	{
		ParticleSystemTemplateMap::iterator it;
		ParticleSystemTemplateMap::iterator itEnd = mParticleSystemTemplates.end();
		for (it = mParticleSystemTemplates.begin(); it != itEnd; ++it)
		{
			v.push_back((*it).first);
		}
	}
	//-----------------------------------------------------------------------
	ParticleSystem* ParticleSystemManager::createParticleSystem(const String& name,
		const String& templateName,
		Ogre::SceneManager* sceneManager)
	{
		Ogre::String nameStr = name;
		// Validate name
		if (mParticleSystems.find(HashCodeString(name)) != mParticleSystems.end())
		{
			OGRE_LOG("PU: ParticleSystem with name '" + name + "' already exists. ParticleSystemManager::createParticleSystem");
			nameStr += "_repeat_pu";
		}

		Ogre::NameValuePairList params;
		params["templateName"] = templateName;
	
		ParticleSystem* system = static_cast<ParticleSystem*>(
			sceneManager->createMovableObject(nameStr, ParticleSystemFactory::PU_FACTORY_TYPE_NAME,
			&params));
		system->setSceneManager(sceneManager);
		system->setTemplateName(templateName);
		mParticleSystems[HashCodeString(nameStr)] = system;
		return system;
	}
	//-----------------------------------------------------------------------
	ParticleSystem* ParticleSystemManager::createParticleSystem(const String& name,
		Ogre::SceneManager* sceneManager)
	{
		Ogre::String nameStr = name;
		// Validate name
		if (mParticleSystems.find(HashCodeString(name)) != mParticleSystems.end())
		{
			OGRE_LOG("PU: ParticleSystem with name '" + name + "' already exists. ParticleSystemManager::createParticleSystem.");
			nameStr += "_repeat_pu";
		}

		Ogre::NameValuePairList params;
		ParticleSystem* system = static_cast<ParticleSystem*>(
		sceneManager->createMovableObject(nameStr, ParticleSystemFactory::PU_FACTORY_TYPE_NAME, 
			&params));
		system->setSceneManager(sceneManager);
		mParticleSystems[HashCodeString(nameStr)] = system;
		return system;
	}
	//-----------------------------------------------------------------------
	ParticleSystem* ParticleSystemManager::getParticleSystem(const String& name)
	{
		if (name == StringUtil::BLANK)
			return 0;

		ParticleSystemMap::iterator i = mParticleSystems.find(HashCodeString(name));
		if (i != mParticleSystems.end())
		{
			return i->second;
		}
	
		return 0;
	}
	//-----------------------------------------------------------------------
	void ParticleSystemManager::destroyParticleSystem(ParticleSystem* particleSystem, Ogre::SceneManager* sceneManager)
	{
		if (!particleSystem)
			return;

		ParticleSystemMap::iterator i = mParticleSystems.find(HashCodeString(particleSystem->getName()));
		if (i != mParticleSystems.end())
		{
			mParticleSystems.erase(i);
		}

		// First determine whether the Particle System still exists, before it is really destroyed.
		if (sceneManager->hasMovableObject(particleSystem->getName(), ParticleSystemFactory::PU_FACTORY_TYPE_NAME))
		{
			sceneManager->destroyMovableObject(particleSystem);
		}
	}
	//-----------------------------------------------------------------------
#if ENABLE_HASH
	void ParticleSystemManager::destroyParticleSystem(const HashString& particleSystemName, Ogre::SceneManager* sceneManager)
	{
		// Remove it from the map although it doesn磘 exist anymore
		ParticleSystemMap::iterator i = mParticleSystems.find(particleSystemName);
		Ogre::String parName;
		if (i != mParticleSystems.end())
		{
			parName = i->second->getName();
			mParticleSystems.erase(i);
		}

		// First determine whether the Particle System still exists, before it is really destroyed.
		if (sceneManager->hasMovableObject(parName, ParticleSystemFactory::PU_FACTORY_TYPE_NAME))
		{
			sceneManager->destroyMovableObject(parName, ParticleSystemFactory::PU_FACTORY_TYPE_NAME);
		}
	}
#endif
	void ParticleSystemManager::destroyParticleSystem(const String& particleSystemName, Ogre::SceneManager* sceneManager)
	{
		// Remove it from the map although it doesn磘 exist anymore
		ParticleSystemMap::iterator i = mParticleSystems.find(HashCodeString(particleSystemName));
		if (i != mParticleSystems.end())
		{
			mParticleSystems.erase(i);
		}

		// First determine whether the Particle System still exists, before it is really destroyed.
		if (sceneManager->hasMovableObject(particleSystemName, ParticleSystemFactory::PU_FACTORY_TYPE_NAME))
		{
			sceneManager->destroyMovableObject(particleSystemName, ParticleSystemFactory::PU_FACTORY_TYPE_NAME);
		}
	}
	//-----------------------------------------------------------------------
	void ParticleSystemManager::destroyAllParticleSystems(Ogre::SceneManager* sceneManager)
	{
		// Changed function in V1.3.1
		// Delete all Particle Systems in the specified sceneManager. This doesn磘 include the templates
		// changed this function.
		ParticleSystemMap::iterator t = mParticleSystems.begin();
		while ( t != mParticleSystems.end() )
		{
			ParticleSystem* particleSystem = t->second;
			if (sceneManager->hasMovableObject(particleSystem->getName(), ParticleSystemFactory::PU_FACTORY_TYPE_NAME))
			{
				sceneManager->destroyMovableObject(particleSystem);
				mParticleSystems.erase( t++ ); // PU 1.4
			}
			else
			{
				++t;
			}
		}
	}
	//-----------------------------------------------------------------------
	ParticleSystem* ParticleSystemManager::_createSystemImpl(const String& name)
	{
		ParticleSystem* sys = PU_NEW ParticleSystem(name);

		return sys;
	}
	//-----------------------------------------------------------------------
	ParticleSystem* ParticleSystemManager::_createSystemImpl(const String& name, const String& templateName)
	{
		// Look up template
		ParticleSystem* pTemplate = getParticleSystemTemplate(templateName);
		if (!pTemplate)
		{
			pTemplate = getParticleSystemTemplate("effect_error");
			if (!pTemplate)
			{
				OGRE_LOG("PU: Cannot find required template '" + templateName + "' and effect_error not exit", "ParticleSystemManager::createSystem");
				return NULL;
			}
			LogManager::getSingleton().logMessage("ERROR PU: Cannot find required template '" + name + "' and effect_error not exit. ParticleSystemManager::createSystem");
		}

		ParticleSystem* sys = PU_NEW ParticleSystem(name);
        
		// Copy template settings
		*sys = *pTemplate;
		pTemplate->copyAttributesTo(sys);

		return sys;
	}
	//-----------------------------------------------------------------------
	void ParticleSystemManager::_destroySystemImpl(ParticleSystem* particleSystem)
	{
		PU_DELETE particleSystem;
	}
	//-----------------------------------------------------------------------
	void ParticleSystemManager::addAlias (IAlias* alias)
	{
		String name = alias->getAliasName();
		mAliasMap[HashCodeString(name)] = alias;
		LogManager::getSingleton().logMessage(": Alias '" + name + "' registered");
	}
	//-----------------------------------------------------------------------
	IAlias* ParticleSystemManager::getAlias (const String& aliasName)
	{
		AliasMap::iterator it = mAliasMap.find(HashCodeString(aliasName));
		if (it == mAliasMap.end())
		{
			OGRE_LOG("PU: Cannot find alias. ParticleSystemManager::getAlias");
			return NULL;
		}

		return it->second;
	}
	//-----------------------------------------------------------------------
	void ParticleSystemManager::destroyAlias(IAlias* alias)
	{
		switch (alias->getAliasType())
		{
			case IAlias::AT_RENDERER:
			{
				ParticleRenderer* renderer = static_cast<ParticleRenderer*>(alias);
				destroyRenderer(renderer);
			}
			break;

			case IAlias::AT_TECHNIQUE:
			{
				ParticleTechnique* technique = static_cast<ParticleTechnique*>(alias);
				destroyTechnique(technique);
			}
			break;

			case IAlias::AT_EMITTER:
			{
				ParticleEmitter* emitter = static_cast<ParticleEmitter*>(alias);
				destroyEmitter(emitter);
			}
			break;
	
			case IAlias::AT_AFFECTOR:
			{
				ParticleAffector* affector = static_cast<ParticleAffector*>(alias);
				destroyAffector(affector);
			}
			break;

			case IAlias::AT_OBSERVER:
			{
				ParticleObserver* observer = static_cast<ParticleObserver*>(alias);
				destroyObserver(observer);
			}
			break;

			case IAlias::AT_HANDLER:
			{
				ParticleEventHandler* handler = static_cast<ParticleEventHandler*>(alias);
				destroyEventHandler(handler);
			}
			break;

			case IAlias::AT_EXTERN:
			{
				Extern* externObject = static_cast<Extern*>(alias);
				destroyExtern(externObject);
			}
			break;

			case IAlias::AT_BEHAVIOUR:
			{
				ParticleBehaviour* behaviour = static_cast<ParticleBehaviour*>(alias);
				destroyBehaviour(behaviour);
			}
			break;

			case IAlias::AT_UNDEFINED:
				OGRE_LOG("PU: Trying to delete alias of undetermined type. ParticleSystemManager::~ParticleSystemManager");
			break;
		}
	}
	//-----------------------------------------------------------------------
	void ParticleSystemManager::destroyAllAliasses(void)
	{
		AliasMap::iterator it;
		for (it = mAliasMap.begin(); it != mAliasMap.end(); ++it)
		{
			destroyAlias(it->second);
		}
		mAliasMap.clear();
	}
	//-----------------------------------------------------------------------
	ParticleSystemManager::AliasMap* ParticleSystemManager::_getAliasMap(void)
	{
		return &mAliasMap;
	}
	//-----------------------------------------------------------------------
	ParticleSystemManager* ParticleSystemManager::getSingletonPtr(void)
	{
		return msSingleton;
	}
	//-----------------------------------------------------------------------
	ParticleSystemManager& ParticleSystemManager::getSingleton(void)
	{
		assert( msSingleton );  return ( *msSingleton );
	}
	//-----------------------------------------------------------------------
	void ParticleSystemManager::writeScript(ParticleSystem* particleSystem, const String& fileName)
	{
		mParticleScriptSerializer->writeScript(particleSystem, fileName);
	}
	//-----------------------------------------------------------------------
	const String& ParticleSystemManager::writeScript(ParticleSystem* particleSystem)
	{
		return mParticleScriptSerializer->writeScript(particleSystem);
	}
	//-----------------------------------------------------------------------
	ParticleScriptSerializer* ParticleSystemManager::getParticleScriptSerializer(void) const
	{
		return mParticleScriptSerializer;
	}
	//-----------------------------------------------------------------------
	CameraDependency* ParticleSystemManager::createCameraDependency(void)
	{
		mCameraDependency.setThreshold(1000);
		mCameraDependency.setIncrease(false);
		return &mCameraDependency;
	}
	//-----------------------------------------------------------------------
	void ParticleSystemManager::createDepthMap (Camera* camera, Ogre::SceneManager* sceneManager)
	{
		// Don磘 recreate the depth map
		if (mDepthMap || mDepthMapExtern)
		{
			return;
		}

		// Create a RenderTexture and material if not available already
		// directx does not support PF_R8G8B8A8,use PF_A8B8G8R8
		if (mDepthTextureName.empty())
		{
			std::stringstream ss1;
			ss1 << "DepthTexture" << this;
			mDepthTextureName = ss1.str().c_str();
			Ogre::TexturePtr depthTexturePtr = Ogre::TextureManager::getSingleton().createManual(mDepthTextureName, 
				Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME, 
				Ogre::TEX_TYPE_2D, 
				camera->getViewport()->getActualWidth(), 
				camera->getViewport()->getActualHeight(),
				0, 
//				Ogre::PF_FLOAT16_R, 
				//Ogre::PF_R8G8B8A8, 
				Ogre::PF_A8B8G8R8, 
				Ogre::TU_RENDERTARGET);
		}

		if (mDepthMaterialName.empty())
		{
			// Create a vertex and fragment program (hlsl)
			mDepthVertexName = "_DepthMapVP"; // Use ParticleUniverse_ to avoid name conflicts.
			mDepthFragmentName = "_DepthMapFP"; // Use ParticleUniverse_ to avoid name conflicts.
			Ogre::HighLevelGpuProgramPtr vertexProgram = Ogre::HighLevelGpuProgramManager::getSingleton().createProgram( 
				mDepthVertexName,
				Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME,
				"hlsl",
			Ogre::GPT_VERTEX_PROGRAM);	
			vertexProgram->setSourceFile("pu_depth_sm20.hlsl");
			vertexProgram->setParameter("target", "vs_2_0");
			vertexProgram->setParameter("entry_point", "mainVP"); // Must be same name as in pu_depth.hlsl
			vertexProgram->load();

			Ogre::HighLevelGpuProgramPtr fragmentProgram = Ogre::HighLevelGpuProgramManager::getSingleton().createProgram( 
				mDepthFragmentName,
				Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME,
				"hlsl",
				Ogre::GPT_FRAGMENT_PROGRAM);
			fragmentProgram->setSourceFile("pu_depth_sm20.hlsl");
			fragmentProgram->setParameter("target", "ps_2_0");
			fragmentProgram->setParameter("entry_point", "mainFP"); // Must be same name as in pu_depth.hlsl
			fragmentProgram->load();

			// Create a depth rendering technique, which is used for all renderables in the scene
			std::stringstream ss2;
			ss2 << "DepthMaterial" << this;
			mDepthMaterialName = ss2.str().c_str();
			Ogre::MaterialPtr depthMaterial = Ogre::MaterialManager::getSingleton().create(mDepthMaterialName, 
				Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME);
			mDepthTechnique = depthMaterial->getTechnique(0); // There is already a technique and a pass created
			mDepthPass = mDepthTechnique->getPass(0);
			Ogre::TextureUnitState* textureUnitState = mDepthPass->createTextureUnitState(mDepthTextureName);
			(void)textureUnitState;
			mDepthPass->setDepthCheckEnabled(false);
			mDepthPass->setDepthWriteEnabled(false);
			mDepthPass->setLightingEnabled(false);
			mDepthPass->setVertexProgram(mDepthVertexName);
			mDepthPass->setFragmentProgram(mDepthFragmentName);
			Ogre::GpuProgramParametersSharedPtr vertexParams = mDepthPass->getVertexProgramParameters(); 
			vertexParams->setNamedAutoConstant("worldViewProj", Ogre::GpuProgramParameters::ACT_WORLDVIEWPROJ_MATRIX); // Must be same name as in pu_depth.hlsl
			vertexParams->setNamedAutoConstant("texelOffsets", Ogre::GpuProgramParameters::ACT_TEXEL_OFFSETS); // Must be same name as in pu_depth.hlsl
			vertexParams->setNamedAutoConstant("depthRange", Ogre::GpuProgramParameters::ACT_SCENE_DEPTH_RANGE); // Must be same name as in pu_depth.hlsl
			vertexParams->setNamedConstant("depthScale", mDepthScale);
		}

		// Set depth map
		Ogre::TexturePtr depthTexturePtr = Ogre::TextureManager::getSingleton().getByName(mDepthTextureName);
		if (depthTexturePtr)
		{
			mDepthMap = depthTexturePtr->getBuffer()->getRenderTarget();
			Ogre::Viewport* viewport = mDepthMap->addViewport(camera);
			viewport->setBackgroundColour(ColourValue::Black);
			mDepthMap->addListener(&mDepthMapTargetListener);
			mDepthMap->setAutoUpdated(true);

			// Set attributes to the target listener
			mDepthMapTargetListener.mSceneManager = sceneManager;
			mDepthMapTargetListener.mDepthTechnique = mDepthTechnique;
			mDepthMapTargetListener.mCamera = camera;
			mDepthMapTargetListener.mDepthMap = mDepthMap;
		}

		// Set debug overlay for testing purposes (uncomment if you want to view the depth map in the debug overlay)
//		if (!mDebugOverlay)
//		{
//			Ogre::MaterialPtr debugMaterial = Ogre::MaterialManager::getSingleton().create("_DebugMaterial", 
//				Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME);
//			debugMaterial->getTechnique(0)->getPass(0)->createTextureUnitState(mDepthTextureName);
//			mDebugPanel = (Ogre::OverlayContainer*)(Ogre::OverlayManager::getSingleton().createOverlayElement("Panel", "_DebugDepth"));
//			mDebugPanel->_setPosition(0.7, 0);
//			mDebugPanel->_setDimensions(0.3, 0.3);
//			mDebugPanel->setMaterialName("_DebugMaterial");
//			mDebugOverlay = Ogre::OverlayManager::getSingleton().create("Core/DebugOverlay");
//			mDebugOverlay->setZOrder(500);
//			mDebugOverlay->add2D(mDebugPanel);
//			mDebugOverlay->show();
//		}
	}
	//-----------------------------------------------------------------------
	void ParticleSystemManager::destroyDepthMap (void)
	{
		// Reset the depth map.
		if (mDepthMap && !mDepthMapExtern)
		{
			mDepthMap->removeAllListeners();
			mDepthMap->removeAllViewports();
			mDepthMap = 0;
		}
	}
	//-----------------------------------------------------------------------
	bool ParticleSystemManager::notifyDepthMapNeeded(Camera* camera, Ogre::SceneManager* sceneManager)
	{
		if (mDepthMap || mDepthMapExtern)
		{
			return true;
		}
		else
		{
			 if (camera && sceneManager)
			 {
				/** Use the first camera that is passed. Although in theory, other camera's may be passed, they are ignored. The depth map is
					created for this camera viewport only.
				*/ 
				createDepthMap (camera, sceneManager);
				return true;
			 }
			 else
			 {
				 return false;
			 }
		}
	}
	//-----------------------------------------------------------------------
	void ParticleSystemManager::registerSoftParticlesRenderer(ParticleRenderer* renderer)
	{
		if (!mDepthMapExtern)
		{
			mDepthMapTargetListener.registerRenderer(renderer);
		}
	}
	//-----------------------------------------------------------------------
	void ParticleSystemManager::unregisterSoftParticlesRenderer(ParticleRenderer* renderer)
	{
		if (!mDepthMapExtern)
		{
			// Unregister the renderers and destroy the depthmap, because apparently it isn磘 used anymore
			mDepthMapTargetListener.unregisterRenderer(renderer);
			if (mDepthMapTargetListener.registeredRenderersEmpty())
			{
				destroyDepthMap();
			}
		}
	}
	//-----------------------------------------------------------------------
	Real ParticleSystemManager::getDepthScale(void) const
	{
		return mDepthScale;
	}
	//-----------------------------------------------------------------------
	void ParticleSystemManager::setDepthScale(Real depthScale)
	{
		mDepthScale = depthScale;
	}
	//-----------------------------------------------------------------------
	const String& ParticleSystemManager::getDepthTextureName (void)
	{
		return mDepthTextureName;
	}
	//-----------------------------------------------------------------------
	void ParticleSystemManager::setExternDepthTextureName (const String& depthTextureName)
	{
		mDepthTextureName = depthTextureName;
		mDepthMapExtern = true;
	}
	//-----------------------------------------------------------------------
	void ParticleSystemManager::resetExternDepthTextureName (void)
	{
		mDepthTextureName = StringUtil::BLANK;
		mDepthMapExtern = false;
	}
	//-----------------------------------------------------------------------
	DynamicAttribute* ParticleSystemManager::createDynamicAttribute(DynamicAttribute::DynamicAttributeType type)
	{
		switch (type)
		{
			case DynamicAttribute::DAT_FIXED:
				return PU_NEW_T(DynamicAttributeFixed, MEMCATEGORY_SCENE_OBJECTS)();
			break;

			case DynamicAttribute::DAT_RANDOM:
				return PU_NEW_T(DynamicAttributeRandom, MEMCATEGORY_SCENE_OBJECTS)();
			break;

			case DynamicAttribute::DAT_CURVED:
				return PU_NEW_T(DynamicAttributeCurved, MEMCATEGORY_SCENE_OBJECTS)();
			break;

			case DynamicAttribute::DAT_OSCILLATE:
				return PU_NEW_T(DynamicAttributeOscillate, MEMCATEGORY_SCENE_OBJECTS)();
			break;
		}

		return 0;
	}
	//-----------------------------------------------------------------------
	bool ParticleSystemManager::isAutoLoadMaterials(void) const
	{
		return mAutoLoadMaterials;
	}
	//-----------------------------------------------------------------------
	void ParticleSystemManager::setAutoLoadMaterials(bool autoLoadMaterials)
	{
		mAutoLoadMaterials = autoLoadMaterials;
	}
	//-----------------------------------------------------------------------
	//-----------------------------------------------------------------------
	//-----------------------------------------------------------------------
	void DepthMapTargetListener::preViewportUpdate(const Ogre::RenderTargetViewportEvent& evt)
	{
		// Exclude the particle system renderers that renders soft particles, by setting visibility to false
		vector<ParticleRenderer*>::iterator it;
		vector<ParticleRenderer*>::iterator itEnd = mRenderers.end();
		for (it = mRenderers.begin(); it != itEnd; ++it)
		{
			(*it)->setVisible(false);
		}

		// Exclude the overlays
		mCamera->getViewport()->setOverlaysEnabled(false);

		// Add the DepthMapTargetListener as a RenderableListener to replace the technique for all renderables
		Ogre::RenderQueue* queue = mSceneManager->getRenderQueue();
		queue->setRenderableListener(this);
	}
	//-----------------------------------------------------------------------
	void DepthMapTargetListener::postViewportUpdate(const Ogre::RenderTargetViewportEvent& evt)
	{
		// Include the particle system renderers again
		vector<ParticleRenderer*>::iterator it;
		vector<ParticleRenderer*>::iterator itEnd = mRenderers.end();
		for (it = mRenderers.begin(); it != itEnd; ++it)
		{
			(*it)->setVisible(true);
		}

		// Reset the RenderableListener
		Ogre::RenderQueue* queue = mSceneManager->getRenderQueue();
		queue->setRenderableListener(0);

		// Reset the overlays
		mCamera->getViewport()->setOverlaysEnabled(true);
	}
	//-----------------------------------------------------------------------
	bool DepthMapTargetListener::renderableQueued(Ogre::Renderable* rend, uint8 groupID, ushort priority, Ogre::Technique** ppTech, Ogre::RenderQueue* pQueue)
	{
		// Replace the technique of all renderables
		*ppTech = mDepthTechnique;
		return true;
	}
	//-----------------------------------------------------------------------
	void DepthMapTargetListener::registerRenderer(ParticleRenderer* renderer)
	{
		mRenderers.push_back(renderer);
	}
	//-----------------------------------------------------------------------
	void DepthMapTargetListener::unregisterRenderer(ParticleRenderer* renderer)
	{
		vector<ParticleRenderer*>::iterator it;
		vector<ParticleRenderer*>::iterator itEnd = mRenderers.end();
		for (it = mRenderers.begin(); it != itEnd; ++it)
		{
			if (*it == renderer)
			{
				// Remove it
				mRenderers.erase(it);
				return;
			}
		}
	}
	//-----------------------------------------------------------------------
	bool DepthMapTargetListener::registeredRenderersEmpty(void)
	{
		return mRenderers.empty();
	}
	//-----------------------------------------------------------------------
	String ParticleSystemFactory::PU_FACTORY_TYPE_NAME = "PUParticleSystem";
	//-----------------------------------------------------------------------
	Ogre::MovableObject* ParticleSystemFactory::createInstanceImpl( const String& name, 
		const Ogre::NameValuePairList* params)
	{
		if (params != 0)
		{
			Ogre::NameValuePairList::const_iterator ni = params->find("templateName");
			if (ni != params->end())
			{
				String templateName = ni->second;
				
				// Create using manager
				return ParticleSystemManager::getSingleton()._createSystemImpl(name, templateName);
			}
		}
		
		// Not template based, just create one with the given name
		return ParticleSystemManager::getSingleton()._createSystemImpl(name);
	}
	//-----------------------------------------------------------------------
	const String& ParticleSystemFactory::getType(void) const
	{
		return PU_FACTORY_TYPE_NAME;
	}
	//-----------------------------------------------------------------------
	void ParticleSystemFactory::destroyInstance(Ogre::MovableObject* obj)
	{
		// Use manager
		ParticleSystemManager::getSingleton()._destroySystemImpl(static_cast<ParticleSystem*>(obj));
	}

}
