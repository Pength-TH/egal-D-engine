/*
-----------------------------------------------------------------------------------------------
This source file is part of the Particle Universe product.

Copyright (c) 2012 Henry van Merode

Usage of this program is licensed under the terms of the Particle Universe Commercial License.
You can find a copy of the Commercial License in the Particle Universe package.
-----------------------------------------------------------------------------------------------
*/

#ifndef __PU_SYSTEM_MANAGER_TOKENS_H__
#define __PU_SYSTEM_MANAGER_TOKENS_H__

#include "Prerequisites.h"
#include "ScriptDeserializer.h"
#include "SystemTokens.h"
#include "AliasTokens.h"
#include "TechniqueTokens.h"
#include "RendererTokens.h"
#include "EmitterTokens.h"
#include "AffectorTokens.h"
#include "BehaviourTokens.h"
#include "ObserverTokens.h"
#include "EventHandlerTokens.h"
#include "ExternTokens.h"

namespace egal
{
	/** The BuiltinScriptTranslatorManager contains all Translators that are used by the compiler.
	*/
	class _ParticleUniverseExport BuiltinScriptTranslatorManager : public Ogre::ScriptTranslatorManager
	{
		private:
			SystemTranslator mSystemTranslator;
			AliasTranslator mAliasTranslator;
			TechniqueTranslator mTechniqueTranslator;
			RendererTranslator mRendererTranslator;
			EmitterTranslator mEmitterTranslator;
			AffectorTranslator mAffectorTranslator;
			BehaviourTranslator mBehaviourTranslator;
			ObserverTranslator mObserverTranslator;
			ParticleEventHandlerTranslator mParticleEventHandlerTranslator;
			ExternTranslator mExternTranslator;

		public:
			BuiltinScriptTranslatorManager(void);

			/** 
			*/ 
			virtual size_t getNumTranslators(void) const;

			/** 
			*/ 
			virtual ScriptTranslator* getTranslator(const AbstractNodePtr &node);
	};
	//-----------------------------------------------------------------------
	//-----------------------------------------------------------------------
	//-----------------------------------------------------------------------

	/** 
    */
	class _ParticleUniverseExport ParticleSystemManagerWriter : public ScriptWriter
	{
		protected:

		public:

			ParticleSystemManagerWriter(void) {};
			virtual ~ParticleSystemManagerWriter(void) {};

			/** @see
				ScriptWriter::write
			*/
			virtual void write(ParticleScriptSerializer* serializer, const IElement* element) {}
	};

}
#endif
