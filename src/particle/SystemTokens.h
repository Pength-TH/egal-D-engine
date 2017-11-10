/*
-----------------------------------------------------------------------------------------------
This source file is part of the Particle Universe product.

Copyright (c) 2012 Henry van Merode

Usage of this program is licensed under the terms of the Particle Universe Commercial License.
You can find a copy of the Commercial License in the Particle Universe package.
-----------------------------------------------------------------------------------------------
*/

#ifndef __PU_SYSTEM_TOKENS_H__
#define __PU_SYSTEM_TOKENS_H__

#include "Prerequisites.h"
#include "ScriptWriter.h"
#include "ScriptDeserializer.h"
#include "TechniqueTokens.h"

namespace egal
{
	/** The SystemTranslator parses all 'system' tokens
	*/
	class  SystemTranslator : public ScriptTranslator
	{
		protected:
			ParticleSystem* mSystem;
		public:
			SystemTranslator(void);
			virtual ~SystemTranslator(void){};
			virtual void translate(ScriptCompiler* compiler, const AbstractNodePtr &node);
	};
	//-------------------------------------------------------------------------
	//-------------------------------------------------------------------------
	//-------------------------------------------------------------------------

	/** The ParticleSystemWriter writes all 'system' tokens
    */
	class  ParticleSystemWriter : public ScriptWriter
	{
		public:

			ParticleSystemWriter(void) {};
			virtual ~ParticleSystemWriter(void) {};

			/** @see
				ScriptWriter::write
			*/
			virtual void write(ParticleScriptSerializer* serializer, const IElement* element);

		protected:
			// Gets called during serialization
			ParticleTechniqueWriter mParticleTechniqueWriter;
	};

}
#endif
