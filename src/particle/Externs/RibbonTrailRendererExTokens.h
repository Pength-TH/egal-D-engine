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

#ifndef __SGRL_RibbonTrailRendererEx_H__
#define __SGRL_RibbonTrailRendererEx_H__

#include "ParticleUniversePCH.h"
#include "ParticleUniversePlugin.h"
#include "OgreEffectRibbonTrail.h"

namespace ParticleUniverse
{
	/** Visual data specific for this type of renderer.
    */
	class  RibbonTrailRendererExVisualData : public IVisualData
	{
		public:

			//Constructor
			RibbonTrailRendererExVisualData (Ogre::SceneNode* sceneNode, Ogre::EffectRibbonTrail* ribbonTrail) : 
				IVisualData(), 
				node(sceneNode),
				trail(ribbonTrail),
				addedToTrail(false),
				index(0){};

			Ogre::SceneNode* node;
			bool addedToTrail;
			Ogre::EffectRibbonTrail* trail;
			size_t index;
			virtual void setVisible(bool visible)
			{
				if (visible)
				{
					if (!addedToTrail)
					{
						trail->addNode(node);
						addedToTrail = true;
					}
				}
				else
				{
					if (addedToTrail)
					{
						trail->removeNode(node);
						addedToTrail = false;
					}
				}
			};
	};

	/** The RibbonTrailRendererEx class is responsible to render particles as a RibbonTrail.
    */
	class  RibbonTrailRendererEx : public ParticleRenderer, public TechniqueListener
	{
		protected:
			vector<RibbonTrailRendererExVisualData*> mAllVisualData;
			vector<RibbonTrailRendererExVisualData*> mVisualData;
			size_t mQuota;
			Ogre::EffectRibbonTrail* mTrail;
			String mRibbonTrailName;
			bool mUseVertexColours;
			size_t mMaxChainElements;
			Real mTrailLength;
			Real mTrailWidth;
			bool mRandomInitialColour;
			bool mSetLength;
			bool mSetWidth;
			ColourValue mInitialColour;
			ColourValue mColourChange;
			Ogre::SceneNode* mChildNode;
			bool mUVExtends;

		public:
			// Constants
			static const bool DEFAULT_USE_VERTEX_COLOURS;
			static const size_t DEFAULT_MAX_ELEMENTS;
			static const Real DEFAULT_LENGTH;
			static const Real DEFAULT_WIDTH;
			static const bool DEFAULT_RANDOM_INITIAL_COLOUR;
			static const ColourValue DEFAULT_INITIAL_COLOUR;
			static const ColourValue DEFAULT_COLOUR_CHANGE;
			static const bool DEFAULT_UV_EXTENDS;
			RibbonTrailRendererEx(void);
	        virtual ~RibbonTrailRendererEx(void);

			/** Notify that the Particle System is rescaled.
	        */
			virtual void _notifyRescaled(const Vector3& scale);

			/** Getters and Setters
			*/
			bool isUVExtends(void) const;
			void setUVExtends(bool bUVExtens);

			bool isUseVertexColours(void) const;
			void setUseVertexColours(bool useVertexColours);

			size_t getMaxChainElements(void) const;
			void setMaxChainElements(size_t maxChainElements);

			Real getTrailLength(void) const;
			void setTrailLength(Real trailLength);

			Real getTrailWidth(void) const;
			void setTrailWidth(Real trailWidth);

			bool isRandomInitialColour(void) const;
			void setRandomInitialColour(bool randomInitialColour);

			const ColourValue& getInitialColour(void) const;
			void setInitialColour(const ColourValue& initialColour);

			const ColourValue& getColourChange(void) const;
			void setColourChange(const ColourValue& colourChange);

			/** Deletes all ChildSceneNodes en Entities.
			*/
			void _destroyAll(void);
			
			/** Enable the RibbonTrail
			*/
//			virtual void _notifyStart(void);

			/** Disable the RibbonTrail
			*/
//			virtual void _notifyStop(void);
			
			/** Make all objects visible or invisible.
			*/
			virtual void setVisible(bool visible);

			/** @copydoc ParticleRenderer::_prepare */
			virtual void _prepare(ParticleTechnique* technique);

			/** @copydoc ParticleRenderer::_unprepare */
			virtual void _unprepare(ParticleTechnique* technique);

			/** 
			*/
			virtual void _updateRenderQueue(Ogre::RenderQueue* queue, ParticlePool* pool);

			/** 
			*/
			virtual void _notifyAttached(Ogre::Node* parent, bool isTagPoint = false);

			/** @copydoc ParticleRenderer::_setMaterialName */
			virtual void _setMaterialName(const String& materialName);

			/** 
			*/
			virtual void _notifyCurrentCamera(Camera* cam);
	
			/** 
			*/
			virtual void _notifyParticleQuota(size_t quota);

			/** 
			*/
			virtual void _notifyDefaultDimensions(Real width, Real height, Real depth);

			/** 
			*/
			virtual void _notifyParticleResized(void);

			/** 
			*/
			virtual void _notifyParticleZRotated(void);

			/** 
			*/
			virtual void setRenderQueueGroup(uint8 queueId);

			/** 
			*/
			virtual SortMode _getSortMode(void) const;

			/** 
			*/
			virtual void copyAttributesTo (ParticleRenderer* renderer);

			/*  See TechniqueListener.
			*/
			virtual void particleEmitted(ParticleTechnique* particleTechnique, Particle* particle);

			/*  See TechniqueListener.
			*/
			virtual void particleExpired(ParticleTechnique* particleTechnique, Particle* particle);
	};

	/** The RibbonTrailRendererExTranslator parses 'RibbonTrailRendererEx' tokens
	*/
	class  RibbonTrailRendererExTranslator : public ScriptTranslator
	{
		public:
			RibbonTrailRendererExTranslator(void){};
			~RibbonTrailRendererExTranslator(void){};
			virtual bool translateChildProperty(ScriptCompiler* compiler, const AbstractNodePtr &node);
			virtual bool translateChildObject(ScriptCompiler* compiler, const AbstractNodePtr &node);
	};
	//-------------------------------------------------------------------------
	//-------------------------------------------------------------------------
	//-------------------------------------------------------------------------

	/** 
    */
	class  RibbonTrailRendererExWriter : public ParticleRendererWriter
	{
		protected:

		public:

			RibbonTrailRendererExWriter(void) {};
			virtual ~RibbonTrailRendererExWriter(void) {};

			/** @see
				ScriptWriter::write
			*/
			virtual void write(ParticleScriptSerializer* serializer, const IElement* element);
	};
	/** Factory class responsible for creating a RibbonTrailRendererEx.
    */
	class  RibbonTrailRendererExFactory : public ParticleRendererFactory
	{
		public:
			RibbonTrailRendererExFactory(void) {};
	        virtual ~RibbonTrailRendererExFactory(void) {};

			/** See ParticleRendererFactory */
			String getRendererType(void) const
			{
				return "RibbonTrailEx";
			}

			/** See ParticleRendererFactory */
			ParticleRenderer* createRenderer(void)
			{
				return _createRenderer<RibbonTrailRendererEx>();
			}

			/** See ScriptReader */
			virtual bool translateChildProperty(ScriptCompiler* compiler, const AbstractNodePtr &node)
			{
				return mRibbonTrailRendererExTranslator.translateChildProperty(compiler, node);
			};

			/** See ScriptReader */
			virtual bool translateChildObject(ScriptCompiler* compiler, const AbstractNodePtr &node)
			{
				return mRibbonTrailRendererExTranslator.translateChildObject(compiler, node);
			};

			/*  */
			virtual void write(ParticleScriptSerializer* serializer, const IElement* element)
			{
				// Delegate
				mRibbonTrailRendererExWriter.write(serializer, element);
			}


		protected:
			RibbonTrailRendererExWriter mRibbonTrailRendererExWriter;
			RibbonTrailRendererExTranslator mRibbonTrailRendererExTranslator;
	};

}

#endif // __SGRL_RibbonTrailRendererEx_H__
