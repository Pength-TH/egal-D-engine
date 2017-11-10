/*
-----------------------------------------------------------------------------------------------
This source file is part of the Particle Universe product.

Copyright (c) 2012 Henry van Merode

Usage of this program is licensed under the terms of the Particle Universe Commercial License.
You can find a copy of the Commercial License in the Particle Universe package.
-----------------------------------------------------------------------------------------------
*/

#ifndef __PU_COLLISION_AVOIDANCE_AFFECTOR_TOKENS_H__
#define __PU_COLLISION_AVOIDANCE_AFFECTOR_TOKENS_H__

#include "Prerequisites.h"
#include "AffectorTokens.h"

namespace egal
{
	/** The CollisionAvoidanceAffectorTranslator parses 'CollisionAvoidanceAffector' tokens
	*/
	class  CollisionAvoidanceAffectorTranslator : public ScriptTranslator
	{
		public:
			CollisionAvoidanceAffectorTranslator(void){};
			virtual ~CollisionAvoidanceAffectorTranslator(void){};
			virtual bool translateChildProperty(ScriptCompiler* compiler, const AbstractNodePtr &node);
			virtual bool translateChildObject(ScriptCompiler* compiler, const AbstractNodePtr &node);
	};
	//-------------------------------------------------------------------------
	//-------------------------------------------------------------------------
	//-------------------------------------------------------------------------

	/** 
    */
	class  CollisionAvoidanceAffectorWriter : public ParticleAffectorWriter
	{
		public:

			CollisionAvoidanceAffectorWriter(void) {};
			virtual ~CollisionAvoidanceAffectorWriter(void) {};

			/** @see
				ScriptWriter::write
			*/
			virtual void write(ParticleScriptSerializer* serializer , const IElement* element);
	};

}
#endif
