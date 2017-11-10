/*
-----------------------------------------------------------------------------------------------
This source file is part of the Particle Universe product.

Copyright (c) 2012 Henry van Merode

Usage of this program is licensed under the terms of the Particle Universe Commercial License.
You can find a copy of the Commercial License in the Particle Universe package.
-----------------------------------------------------------------------------------------------
*/

#ifndef __PU_LINEAR_FORCE_AFFECTOR_H__
#define __PU_LINEAR_FORCE_AFFECTOR_H__

#include "ParticleUniversePrerequisites.h"
#include "ParticleUniverseBaseForceAffector.h"

namespace ParticleUniverse
{
	/** Adds a force to particles.
    */
	class _ParticleUniverseExport LinearForceAffector : public BaseForceAffector
	{
		public:
			LinearForceAffector(void) : 
				BaseForceAffector()
			{
				m_bLocateFlag = false;
			};
	        virtual ~LinearForceAffector(void) {};

			/** 
	        */
			virtual void copyAttributesTo (ParticleAffector* affector);

			/** 
	        */
			virtual void _preProcessParticles(ParticleTechnique* particleTechnique, Real timeElapsed);

			/** 
	        */
			virtual void _affect(ParticleTechnique* particleTechnique, Particle* particle, Real timeElapsed);

			void setLocateOritation(bool value){ m_bLocateFlag = value; }
			bool getLocateOritation(){ return m_bLocateFlag; }
		protected:
			bool m_bLocateFlag;
	};

}
#endif
