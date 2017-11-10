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

#ifndef __PU_TRACK_PARENT_FACTORY_H__
#define __PU_TRACK_PARENT_FACTORY_H__

#include "Prerequisites.h"
#include "Affector.h"
#include "AffectorTokens.h"
#include "TrackParentAffector.h"
#include "TrackParentAffectorTokens.h"
#include "ExternFactory.h"

namespace egal
{
	/** Factory class responsible for creating the TrackParent.
    */
	class  TrackParentAffectorFactory : public ExternFactory
	{
		public:
			TrackParentAffectorFactory(void) {};
	        virtual ~TrackParentAffectorFactory(void) {};

			/** See ParticleAffectorFactory */
			String getAffectorType(void) const
			{
				return "TrackParentAffector";
			}

			/** See ParticleAffectorFactory  */
			ParticleAffector* createAffector(void)
			{
				return _createAffector<TrackParentAffector>();
			}

			/** See ScriptReader */
			virtual bool translateChildProperty(ScriptCompiler* compiler, const AbstractNodePtr &node)
			{
				return mTrackParentAffectorTranslator.translateChildProperty(compiler, node);
			};

			/** See ScriptReader */
			virtual bool translateChildObject(ScriptCompiler* compiler, const AbstractNodePtr &node)
			{
				return mTrackParentAffectorTranslator.translateChildObject(compiler, node);
			};

			/*  */
			virtual void write(ParticleScriptSerializer* serializer , const IElement* element)
			{
				// Delegate
				mTrackParentAffectorWriter.write(serializer, element);
			}

		protected:
			TrackParentAffectorWriter mTrackParentAffectorWriter;
			TrackParentAffectorTranslator mTrackParentAffectorTranslator;
	};

}

#endif // __SGRLPU_TRACK_PARENT_AFFECT_H__
