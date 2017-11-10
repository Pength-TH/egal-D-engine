/*
-----------------------------------------------------------------------------------------------
This source file is part of the Particle Universe product.

Copyright (c) 2012 Henry van Merode

Usage of this program is licensed under the terms of the Particle Universe Commercial License.
You can find a copy of the Commercial License in the Particle Universe package.
-----------------------------------------------------------------------------------------------
*/

#ifndef __PU_DO_ENABLE_COMPONENT_EVENT_HANDLER_TOKENS_H__
#define __PU_DO_ENABLE_COMPONENT_EVENT_HANDLER_TOKENS_H__

#include "Prerequisites.h"
#include "EventHandlerTokens.h"

namespace egal
{
	/** The DoEnableComponentEventHandlerTranslator parses 'DoEnableComponentEventHandler' tokens
	*/
	class  DoEnableComponentEventHandlerTranslator : public ScriptTranslator
	{
		public:
			DoEnableComponentEventHandlerTranslator(void){};
			virtual ~DoEnableComponentEventHandlerTranslator(void){};
			virtual bool translateChildProperty(ScriptCompiler* compiler, const AbstractNodePtr &node);
			virtual bool translateChildObject(ScriptCompiler* compiler, const AbstractNodePtr &node);
	};
	//-------------------------------------------------------------------------
	//-------------------------------------------------------------------------
	//-------------------------------------------------------------------------

	/** 
    */
	class  DoEnableComponentEventHandlerWriter : public ParticleEventHandlerWriter
	{
		public:

			DoEnableComponentEventHandlerWriter(void) {};
			virtual ~DoEnableComponentEventHandlerWriter(void) {};

			/** @see
				ScriptWriter::write
			*/
			virtual void write(ParticleScriptSerializer* serializer, const IElement* element);
	};

}
#endif
