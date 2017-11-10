/*
-----------------------------------------------------------------------------------------------
This source file is part of the Particle Universe product.

Copyright (c) 2012 Henry van Merode

Usage of this program is licensed under the terms of the Particle Universe Commercial License.
You can find a copy of the Commercial License in the Particle Universe package.
-----------------------------------------------------------------------------------------------
*/

#ifndef __PU_FLOCK_CENTER_AFFECTOR_FACTORY_H__
#define __PU_FLOCK_CENTER_AFFECTOR_FACTORY_H__

#include "Prerequisites.h"
#include "AffectorFactory.h"
#include "FlockCenteringAffector.h"
#include "FlockCenteringAffectorTokens.h"

namespace egal
{
	/** Factory class responsible for creating the FlockCenteringAffector.
    */
	class  FlockCenteringAffectorFactory : public ParticleAffectorFactory
	{
		public:
			FlockCenteringAffectorFactory(void) {};
	        virtual ~FlockCenteringAffectorFactory(void) {};

			/** See ParticleAffectorFactory */
			String getAffectorType(void) const
			{
				return "FlockCentering";
			}

			/** See ParticleAffectorFactory  */
			ParticleAffector* createAffector(void)
			{
				return _createAffector<FlockCenteringAffector>();
			}

			/*  */
			virtual void write(ParticleScriptSerializer* serializer, const IElement* element)
			{
				// Delegate
				mFlockCenteringAffectorWriter.write(serializer, element);
			}

		protected:
			FlockCenteringAffectorWriter mFlockCenteringAffectorWriter;
	};

}
#endif
