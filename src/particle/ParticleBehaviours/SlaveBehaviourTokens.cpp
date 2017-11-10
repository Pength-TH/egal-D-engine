/*
-----------------------------------------------------------------------------------------------
This source file is part of the Particle Universe product.

Copyright (c) 2012 Henry van Merode

Usage of this program is licensed under the terms of the Particle Universe Commercial License.
You can find a copy of the Commercial License in the Particle Universe package.
-----------------------------------------------------------------------------------------------
*/

#include "PCH.h"

#ifndef _EXPORTS
#define _EXPORTS
#endif

#include "ParticleBehaviours/ParticleUniverseSlaveBehaviour.h"
#include "ParticleBehaviours/ParticleUniverseSlaveBehaviourTokens.h"



#include "seed_memory_leak_check.h" //the last include head file,no include after this line
namespace egal
{
	//-----------------------------------------------------------------------
	void SlaveBehaviourWriter::write(ParticleScriptSerializer* serializer, const IElement* element)
	{
		// Cast the element to a SlaveBehaviour
		const SlaveBehaviour* behaviour = static_cast<const SlaveBehaviour*>(element);

		// Write the header of the SlaveBehaviour
		serializer->writeLine(token[TOKEN_BEHAVIOUR], behaviour->getBehaviourType(), 8);
		serializer->writeLine("{", 8);

		// Write base attributes
		ParticleBehaviourWriter::write(serializer, element);

		// Write the close bracket
		serializer->writeLine("}", 8);
	}

}
