/*
-----------------------------------------------------------------------------------------------
This source file is part of the Particle Universe product.

Copyright (c) 2012 Henry van Merode

Usage of this program is licensed under the terms of the Particle Universe Commercial License.
You can find a copy of the Commercial License in the Particle Universe package.
-----------------------------------------------------------------------------------------------
*/

#ifndef __PU_SPHERE_SURFACE_EMITTER_H__
#define __PU_SPHERE_SURFACE_EMITTER_H__

#include "Prerequisites.h"
#include "Emitter.h"

namespace egal
{
	/** The SphereSurfaceEmitter emits particles from the surface of a sphere (instead within the sphere磗
		volume). The particles are emitted in a direction perpendicular to the tangentvector where 
		the particle emits. Using the angle attribute, the direction can be influenced.
    */
	class  SphereSurfaceEmitter : public ParticleEmitter
	{
		protected:
			Real mRadius;
			Vector3 mRandomVector;

		public:
			// Constants
			static const Real DEFAULT_RADIUS;

			SphereSurfaceEmitter(void);
	        virtual ~SphereSurfaceEmitter(void) {};

			/** 
	        */
			const Real getRadius(void) const;
			void setRadius(const Real radius);

			/** 
	        */
			virtual void _initParticlePosition(Particle* particle);

			/** 
	        */
			virtual void _initParticleDirection(Particle* particle);
			
			/** 
	        */
			virtual void copyAttributesTo (ParticleEmitter* emitter);
	};

}
#endif
