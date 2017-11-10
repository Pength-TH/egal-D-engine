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

#include "Prerequisites.h"
#include "OgreRoot.h"
#include "Plugin.h"


#include "seed_memory_leak_check.h" //the last include head file,no include after this line
namespace egal
{
	ParticleUniversePlugin* plugin;
	//-----------------------------------------------------------------------
	extern "C" void  dllStartPlugin(void) throw()
	{
		plugin = PU_NEW ParticleUniversePlugin();
		Ogre::Root::getSingleton().installPlugin(plugin);
	}

	//-----------------------------------------------------------------------
	extern "C" void  dllStopPlugin(void)
	{
		Ogre::Root::getSingleton().uninstallPlugin(plugin);
		PU_DELETE plugin;
	}

}
