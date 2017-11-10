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

#ifndef __PU_TRACK_PARENT_TOKENS_H__
#define __PU_TRACK_PARENT_TOKENS_H__

#include "Prerequisites.h"
#include "ExternTokens.h"
#include "AffectorTokens.h"
#include "TrackParentAffector.h"

namespace egal
{
	/** The PathFollowerTranslator parses 'TrackParent' tokens
	*/
	class  TrackParentAffectorTranslator : public ScriptTranslator
	{
		public:
			TrackParentAffectorTranslator(void){};
			~TrackParentAffectorTranslator(void){};
			virtual bool translateChildProperty(ScriptCompiler* compiler, const AbstractNodePtr &node);
			virtual bool translateChildObject(ScriptCompiler* compiler, const AbstractNodePtr &node);
	};
	//-------------------------------------------------------------------------
	//-------------------------------------------------------------------------
	//-------------------------------------------------------------------------

	/** 
    */
	class  TrackParentAffectorWriter : public ParticleAffectorWriter, ExternWriter
	{
		public:

			TrackParentAffectorWriter(void) {};
			virtual ~TrackParentAffectorWriter(void) {};


			/** @see
				ScriptWriter::write
			*/
			virtual void write(ParticleScriptSerializer* serializer , const IElement* element);
	};
}

#endif // __SGRLPU_TRACK_PARENT_AFFECT_H__
