/*
-----------------------------------------------------------------------------------------------
This source file is part of the Particle Universe product.

Copyright (c) 2012 Henry van Merode

Usage of this program is licensed under the terms of the Particle Universe Commercial License.
You can find a copy of the Commercial License in the Particle Universe package.
-----------------------------------------------------------------------------------------------
*/

#ifndef __PU_TRACK_PARENT_H__
#define __PU_TRACK_PARENT_H__

#include "ParticleUniversePrerequisites.h"
#include "ParticleUniverseAffector.h"

namespace ParticleUniverse
{
	class _ParticleUniverseExport TrackParentAffector : public ParticleAffector
	{
		public:
			TrackParentAffector(void) : 
				ParticleAffector()
			{
			};
	        virtual ~TrackParentAffector(void) {};		
			/** 
	        */
			virtual void _affect(ParticleTechnique* particleTechnique, Particle* particle, Real timeElapsed);

			/** @copydoc ParticleAffector::copyAttributesTo */
			virtual void copyAttributesTo (ParticleAffector* affector);
	};
}

#endif // __PU_TRACK_PARENT_H__
