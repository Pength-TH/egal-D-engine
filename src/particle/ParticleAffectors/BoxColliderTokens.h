/*
-----------------------------------------------------------------------------------------------
This source file is part of the Particle Universe product.

Copyright (c) 2012 Henry van Merode

Usage of this program is licensed under the terms of the Particle Universe Commercial License.
You can find a copy of the Commercial License in the Particle Universe package.
-----------------------------------------------------------------------------------------------
*/

#ifndef __PU_BOX_COLLIDER_TOKENS_H__
#define __PU_BOX_COLLIDER_TOKENS_H__

#include "Prerequisites.h"
#include "BaseColliderTokens.h"

namespace egal
{
	/** The BoxColliderTranslator parses 'BoxCollider' tokens
	*/
	class  BoxColliderTranslator : public ScriptTranslator
	{
		public:
			BoxColliderTranslator(void){};
			virtual ~BoxColliderTranslator(void){};
			virtual bool translateChildProperty(ScriptCompiler* compiler, const AbstractNodePtr &node);
			virtual bool translateChildObject(ScriptCompiler* compiler, const AbstractNodePtr &node);
	};
	//-------------------------------------------------------------------------
	//-------------------------------------------------------------------------
	//-------------------------------------------------------------------------

	/** 
    */
	class  BoxColliderWriter : public BaseColliderWriter
	{
		public:
			BoxColliderWriter(void) {};
			virtual ~BoxColliderWriter(void) {};

			/** @see
				ScriptWriter::write
			*/
			virtual void write(ParticleScriptSerializer* serializer , const IElement* element);
	};

}
#endif
