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

#include "ParticleObservers/ParticleUniverseOnCollisionObserver.h"



#include "seed_memory_leak_check.h" //the last include head file,no include after this line
namespace egal
{
	//-----------------------------------------------------------------------
	bool OnCollisionObserver::_observe (ParticleTechnique* particleTechnique, Particle* particle, Real timeElapsed)
	{
		return particle->hasEventFlags(Particle::PEF_COLLIDED);
	}
}
