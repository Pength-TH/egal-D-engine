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

#include "Externs/ParticleUniverseGravityExtern.h"
#include "OgreNode.h"


#include "seed_memory_leak_check.h" //the last include head file,no include after this line
namespace egal
{
	//-----------------------------------------------------------------------
	void GravityExtern::_preProcessParticles(ParticleTechnique* particleTechnique, Real timeElapsed)
	{
		// Use the position of the parent node in this case.
		if (isAttached())
		{
			position = getParentNode()->_getDerivedPosition();
			mDerivedPosition = position;
		}
	}
	//-----------------------------------------------------------------------
	void GravityExtern::_interface(ParticleTechnique* technique, 
		Particle* particle, 
		Real timeElapsed)
	{
		if (isAttached())
		{
			if (mDistanceThresholdSet && mDerivedPosition.distance(particle->position) > mDistanceThreshold)
				return;

			// Use the derived GravityAffector functionality here
			_affect(technique, particle, timeElapsed);
		}
	}
	//-----------------------------------------------------------------------
	void GravityExtern::copyAttributesTo (Extern* externObject)
	{
		Attachable::copyAttributesTo(externObject);

		GravityExtern* gravityExtern = static_cast<GravityExtern*>(externObject);
		GravityAffector* gravityAffector = static_cast<GravityAffector*>(gravityExtern);
		GravityAffector::copyAttributesTo(gravityAffector);
	}
}
