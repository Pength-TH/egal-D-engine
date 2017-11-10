/*
-----------------------------------------------------------------------------------------------
This source file is part of the Particle Universe product.

Copyright (c) 2012 Henry van Merode

Usage of this program is licensed under the terms of the Particle Universe Commercial License.
You can find a copy of the Commercial License in the Particle Universe package.
-----------------------------------------------------------------------------------------------
*/

#ifndef __PU_ONCOUNT_OBSERVER_TOKENS_H__
#define __PU_ONCOUNT_OBSERVER_TOKENS_H__

#include "ParticleUniversePrerequisites.h"
#include "ParticleUniverseObserverTokens.h"

namespace egal
{
	/** The OnCountObserverTranslator parses 'OnCountObserver' tokens
	*/
	class _ParticleUniverseExport OnCountObserverTranslator : public ScriptTranslator
	{
		public:
			OnCountObserverTranslator(void){};
			virtual ~OnCountObserverTranslator(void){};
			virtual bool translateChildProperty(ScriptCompiler* compiler, const AbstractNodePtr &node);
			virtual bool translateChildObject(ScriptCompiler* compiler, const AbstractNodePtr &node);
	};
	//-------------------------------------------------------------------------
	//-------------------------------------------------------------------------
	//-------------------------------------------------------------------------

	/** 
    */
	class _ParticleUniverseExport OnCountObserverWriter : public ParticleObserverWriter
	{
		public:

			OnCountObserverWriter(void) {};
			virtual ~OnCountObserverWriter(void) {};

			/** @see
				ScriptWriter::write
			*/
			virtual void write(ParticleScriptSerializer* serializer, const IElement* element);
	};

}
#endif
