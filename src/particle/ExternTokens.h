/*
-----------------------------------------------------------------------------------------------
This source file is part of the Particle Universe product.

Copyright (c) 2012 Henry van Merode

Usage of this program is licensed under the terms of the Particle Universe Commercial License.
You can find a copy of the Commercial License in the Particle Universe package.
-----------------------------------------------------------------------------------------------
*/

#ifndef __PU_PARTICLE_EXTERN_TOKENS_H__
#define __PU_PARTICLE_EXTERN_TOKENS_H__

#include "Prerequisites.h"
#include "Extern.h"
#include "ScriptDeserializer.h"

namespace egal
{
	/** The ExternTranslator parses 'Extern' tokens
	*/
	class  ExternTranslator : public ScriptTranslator
	{
		protected:
			Extern* mExtern;

		public:
			ExternTranslator(void);
			virtual ~ExternTranslator(void){};
			virtual void translate(ScriptCompiler* compiler, const AbstractNodePtr &node);
	};
	//-------------------------------------------------------------------------
	//-------------------------------------------------------------------------
	//-------------------------------------------------------------------------

	/** 
    */
	class  ExternWriter : public ScriptWriter
	{
		public:

			ExternWriter(void) {};
			virtual ~ExternWriter(void) {};

			/** @see
				ScriptWriter::write
			*/
			virtual void write(ParticleScriptSerializer* serializer, const IElement* element);
	};

}
#endif
