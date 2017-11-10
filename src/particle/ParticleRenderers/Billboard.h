/*
-----------------------------------------------------------------------------------------------
This source file is part of the Particle Universe product.

Copyright (c) 2012 Henry van Merode

Usage of this program is licensed under the terms of the Particle Universe Commercial License.
You can find a copy of the Commercial License in the Particle Universe package.
-----------------------------------------------------------------------------------------------
*/

#ifndef __PU_BILLBOARD_H__
#define __PU_BILLBOARD_H__

#include "Prerequisites.h"

namespace egal
{
	/** This is a child of the Ogre Billboard class, with the exception that it has new friends ;-)
    */
	class  Billboard : public Ogre::Billboard
	{
        friend class BillboardRenderer;

		public:
			Billboard(void) : Ogre::Billboard(){};
	        virtual ~Billboard(void){};
	};

}
#endif
