/*
-----------------------------------------------------------------------------------------------
This source file is part of the Particle Universe product.

Copyright (c) 2012 Henry van Merode

Usage of this program is licensed under the terms of the Particle Universe Commercial License.
You can find a copy of the Commercial License in the Particle Universe package.
-----------------------------------------------------------------------------------------------
*/

#ifndef __PU_DO_AFFECTOR_EVENT_HANDLER_TOKENS_H__
#define __PU_DO_AFFECTOR_EVENT_HANDLER_TOKENS_H__

#include "Prerequisites.h"
#include "EventHandlerTokens.h"

namespace egal
{
	/** The DoAffectorEventHandlerTranslator parses 'DoAffectorEventHandler' tokens
	*/
	class  DoAffectorEventHandlerTranslator : public ScriptTranslator
	{
		public:
			DoAffectorEventHandlerTranslator(void){};
			virtual ~DoAffectorEventHandlerTranslator(void){};
			virtual bool translateChildProperty(ScriptCompiler* compiler, const AbstractNodePtr &node);
			virtual bool translateChildObject(ScriptCompiler* compiler, const AbstractNodePtr &node);
	};
	//-------------------------------------------------------------------------
	//-------------------------------------------------------------------------
	//-------------------------------------------------------------------------

	/** 
    */
	class  DoAffectorEventHandlerWriter : public ParticleEventHandlerWriter
	{
		public:

			DoAffectorEventHandlerWriter(void) {};
			virtual ~DoAffectorEventHandlerWriter(void) {};

			/** @see
				ScriptWriter::write
			*/
			virtual void write(ParticleScriptSerializer* serializer, const IElement* element);
	};

}
#endif
