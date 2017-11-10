/*
-----------------------------------------------------------------------------
This source file is part of the Particle Universe product.

Copyright (c) 2006-2008 Henry van Merode

Usage of this program is free for non-commercial use and licensed under the
the terms of the GNU Lesser General Public License.

You should have received a copy of the GNU Lesser General Public License along with
this program; if not, write to the Free Software Foundation, Inc., 59 Temple
Place - Suite 330, Boston, MA 02111-1307, USA, or go to
http://www.gnu.org/copyleft/lesser.txt.
-----------------------------------------------------------------------------
*/

#include "stdafx.h"
#include "PCH.h"
#include "SGRLTrackParentAffector.h"


#include "memory_leak_check.h" //the last include head file,no include after this line
namespace egal
{
	//-----------------------------------------------------------------------
	void TrackParentAffector::copyAttributesTo (ParticleAffector* affector)
	{
		ParticleAffector::copyAttributesTo(affector);
		//TrackParentAffector* TrackParentAffector = static_cast<TrackParentAffector*>(affector);
	}
	//-----------------------------------------------------------------------
	void TrackParentAffector::_affect(ParticleTechnique* particleTechnique, Particle* particle, Real timeElapsed)
	{
		particle->position = particleTechnique->getParentSystem()->getDerivedPosition();
	}

	//-----------------------------------------------------------------------
	bool TrackParentAffectorTranslator::translateChildProperty(ScriptCompiler* compiler, const AbstractNodePtr &node)
	{
		PropertyAbstractNode* prop = reinterpret_cast<PropertyAbstractNode*>(node.get());
		ParticleAffector* af = any_cast<ParticleAffector*>(prop->parent->context);
		TrackParentAffector* affector = static_cast<TrackParentAffector*>(af);

		//if (prop->name == token[TOKEN_PATH_POINT])
		//{
		//	// Property: path_follower_point
		//	if (passValidateProperty(compiler, prop, token[TOKEN_PATH_POINT], VAL_VECTOR3))
		//	{
		//		Vector3 val;
		//		if(getVector3(prop->values.begin(), prop->values.end(), &val))
		//		{
		//			affector->addPoint(val);
		//			return true;
		//		}
		//	}
		//}

		return false;
	}
	//-----------------------------------------------------------------------
	bool TrackParentAffectorTranslator::translateChildObject(ScriptCompiler* compiler, const AbstractNodePtr &node)
	{
		// No objects
		return false;
	}
	//-----------------------------------------------------------------------
	//-----------------------------------------------------------------------
	//-----------------------------------------------------------------------
	void TrackParentAffectorWriter::write(ParticleScriptSerializer* serializer, const IElement* element)
	{
		// Cast the element to a PathFollower
		const TrackParentAffector* affector = static_cast<const TrackParentAffector*>(element);

		// Write the header of the PathFollower
		serializer->writeLine(token[TOKEN_AFFECTOR],affector->getAffectorType(), affector->getName(), 8);
		serializer->writeLine("{", 8);

		// Write base attributes
		ParticleAffectorWriter::write(serializer, element);

		// Write own attributes

		// Write the close bracket
		serializer->writeLine("}", 8);
	}

}
