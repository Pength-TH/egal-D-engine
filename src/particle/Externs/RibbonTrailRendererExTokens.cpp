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
#include "Ogre.h"
#include "SGRLRibbonTrailRendererEx.h"
#include "SGRLSystem.h"


#include "memory_leak_check.h" //the last include head file,no include after this line
namespace egal
{
	#define UVEXTENDS_TOKEN "uvextends"
	// Constants
	const bool RibbonTrailRendererEx::DEFAULT_USE_VERTEX_COLOURS = true;
	const size_t RibbonTrailRendererEx::DEFAULT_MAX_ELEMENTS = 10;
	const Real RibbonTrailRendererEx::DEFAULT_LENGTH = 400;
	const Real RibbonTrailRendererEx::DEFAULT_WIDTH = 5;
	const bool RibbonTrailRendererEx::DEFAULT_RANDOM_INITIAL_COLOUR = true;
	const ColourValue RibbonTrailRendererEx::DEFAULT_INITIAL_COLOUR(1, 1, 1);
	const ColourValue RibbonTrailRendererEx::DEFAULT_COLOUR_CHANGE = ColourValue(0.5, 0.5, 0.5, 0.5);
	const bool RibbonTrailRendererEx::DEFAULT_UV_EXTENDS = false;

	//-----------------------------------------------------------------------
	RibbonTrailRendererEx::RibbonTrailRendererEx(void) : 
	ParticleRenderer(),
		mQuota(0),
		mTrail(0),
		mUseVertexColours(DEFAULT_USE_VERTEX_COLOURS),
		mUVExtends(DEFAULT_UV_EXTENDS),
		mMaxChainElements(DEFAULT_MAX_ELEMENTS),
		mSetLength(false),
		mTrailLength(DEFAULT_LENGTH),
		mSetWidth(false),
		mTrailWidth(DEFAULT_WIDTH),
		mRandomInitialColour(DEFAULT_RANDOM_INITIAL_COLOUR),
		mInitialColour(DEFAULT_INITIAL_COLOUR),
		mColourChange(DEFAULT_COLOUR_CHANGE),
		mChildNode(0)
	{
		std::stringstream ss; 
		ss << "RibbonTrail" << this;
		mRibbonTrailName = ss.str().c_str();
		autoRotate = false;
	}
	//-----------------------------------------------------------------------
	RibbonTrailRendererEx::~RibbonTrailRendererEx(void)
	{
		if (!mParentTechnique)
			return;

		_destroyAll();
	}
	//-----------------------------------------------------------------------
	void RibbonTrailRendererEx::_notifyRescaled(const Vector3& scale)
	{
		ParticleRenderer::_notifyRescaled(scale);
		if (mTrail)
		{
			// Use the average length (is there a good alternative?
			mTrail->setTrailLength(scale.y * mTrailLength);
			size_t numberOfChains = mTrail->getNumberOfChains();
			for (size_t i = 0; i < numberOfChains; ++i)
			{
				mTrail->setInitialWidth(i, scale.x * mTrailWidth);
			}
		}
	}
	bool RibbonTrailRendererEx::isUVExtends(void) const
	{
		return mUVExtends;
	}
	void RibbonTrailRendererEx::setUVExtends(bool bUVExtens)
	{
		mUVExtends = bUVExtens;
	}
	//-----------------------------------------------------------------------
	bool RibbonTrailRendererEx::isUseVertexColours(void) const
	{
		return mUseVertexColours;
	} 
	//-----------------------------------------------------------------------
	void RibbonTrailRendererEx::setUseVertexColours(bool useVertexColours)
	{
		mUseVertexColours = useVertexColours;
	} 
	//-----------------------------------------------------------------------
	size_t RibbonTrailRendererEx::getMaxChainElements(void) const
	{
		return mMaxChainElements;
	} 
	//-----------------------------------------------------------------------
	void RibbonTrailRendererEx::setMaxChainElements(size_t maxChainElements)
	{
		mMaxChainElements = maxChainElements;
	} 
	//-----------------------------------------------------------------------
	Real RibbonTrailRendererEx::getTrailLength(void) const
	{
		return mTrailLength;
	} 
	//-----------------------------------------------------------------------
	void RibbonTrailRendererEx::setTrailLength(Real trailLength)
	{
		mTrailLength = trailLength;
		mSetLength = true;
	} 
	//-----------------------------------------------------------------------
	Real RibbonTrailRendererEx::getTrailWidth(void) const
	{
		return mTrailWidth;
	} 
	//-----------------------------------------------------------------------
	void RibbonTrailRendererEx::setTrailWidth(Real trailWidth)
	{
		mTrailWidth = trailWidth;
		mSetWidth = true;
	} 
	//-----------------------------------------------------------------------
	bool RibbonTrailRendererEx::isRandomInitialColour(void) const
	{
		return mRandomInitialColour;
	} 
	//-----------------------------------------------------------------------
	void RibbonTrailRendererEx::setRandomInitialColour(bool randomInitialColour)
	{
		mRandomInitialColour = randomInitialColour;
	} 
	//-----------------------------------------------------------------------
	const ColourValue& RibbonTrailRendererEx::getInitialColour(void) const
	{
		return mInitialColour;
	} 
	//-----------------------------------------------------------------------
	void RibbonTrailRendererEx::setInitialColour(const ColourValue& initialColour)
	{
		mInitialColour = initialColour;
	} 
	//-----------------------------------------------------------------------
	const ColourValue& RibbonTrailRendererEx::getColourChange(void) const
	{
		return mColourChange;
	} 
	//-----------------------------------------------------------------------
	void RibbonTrailRendererEx::setColourChange(const ColourValue& colourChange)
	{
		mColourChange = colourChange;
	} 
	//-----------------------------------------------------------------------
	void RibbonTrailRendererEx::_destroyAll(void)
	{
		if (!mParentTechnique)
			return;

		// Remove the listener
		mParentTechnique->removeTechniqueListener(this);

		// Detach the Ribbontrail
		if (mChildNode && mTrail && mTrail->isAttached())
		{
			mChildNode->detachObject(mTrail);
		}

		// Delete the Ribbontrail
		Ogre::SceneManager* sceneManager = mParentTechnique->getParentSystem()->getSceneManager();
		if (mTrail && sceneManager && sceneManager->hasRibbonTrail(mRibbonTrailName))
		{
			sceneManager->destroyRibbonTrail(mRibbonTrailName);
			mTrail = 0;
		}

		// Delete the visual data
		vector<RibbonTrailRendererExVisualData*>::const_iterator it;
		vector<RibbonTrailRendererExVisualData*>::const_iterator itEnd = mAllVisualData.end();
		for (it = mAllVisualData.begin(); it != itEnd; ++it)
		{
			PU_DELETE_T(*it, RibbonTrailRendererExVisualData, MEMCATEGORY_SCENE_OBJECTS);
		}

		mAllVisualData.clear();
		mVisualData.clear();

		// Reset the visual data in the pool
		mParentTechnique->initVisualDataInPool();

		// Destroy the children of the childnode, but not the childnode itself, because this gives a delete-order problem
		if (mChildNode)
		{
			mChildNode->removeAndDestroyAllChildren();
		}
	}
	//-----------------------------------------------------------------------
	void RibbonTrailRendererEx::setVisible(bool visible)
	{
		if (mTrail)
		{
			mTrail->setVisible(visible);
		}
	}
	//-----------------------------------------------------------------------
	void RibbonTrailRendererEx::_prepare(ParticleTechnique* technique)
	{
		if (!technique || mRendererInitialised)
			return;

		// Register itself to the technique
		if (mParentTechnique)
		{
			// Although it is safe to assume that technique == mParentTechnique, use the mParentTechnique, because the mParentTechnique is
			// also used for unregistering.
			mParentTechnique->addTechniqueListener(this);
		}

		mQuota = technique->getVisualParticleQuota();
		if (!mChildNode)
		{
			// Create a childnode if not created earlier
			std::stringstream ss; 
			ss << "" << this;
			String childNodeNodeName = ss.str().c_str();
			//mChildNode = mParentTechnique->getParentSystem()->getParentSceneNode()->createChildSceneNode(childNodeNodeName);
			mChildNode = SGRLSystem::getSingletonPtr()->getBaseSceneNode()->createChildSceneNode(childNodeNodeName);
			mChildNode->setInheritOrientation(false);
		}

		if (mChildNode)
		{
			// Create RibbonTrail
			Ogre::SceneManager* sceneManager = mParentTechnique->getParentSystem()->getSceneManager();
			//sceneManager->createRibbonTrail(mRibbonTrailName);
			mTrail = static_cast<Ogre::EffectRibbonTrail*>(
				sceneManager->createMovableObject(mRibbonTrailName, Ogre::EffectRibbonTrailFactory::FACTORY_TYPE_NAME));
			mTrail->setNumberOfChains(mQuota);
			mTrail->setMaxChainElements(mMaxChainElements);
			mTrail->setMaterialName(technique->getMaterialName());
			mTrail->setRenderQueueGroup(mQueueId);
			mTrail->setUVExtends(isUVExtends());
			if (mSetLength)
			{
				mTrail->setTrailLength(_mRendererScale.y * mTrailLength);
			}
			else
			{
				mTrail->setTrailLength(_mRendererScale.y * technique->getDefaultHeight());
			}
			mTrail->setUseVertexColours(mUseVertexColours);

			// Create number of VisualData objects
			String sceneNodeName;
			std::stringstream ss; 
			ss << this;
			for (size_t i = 0; i < mQuota; i++)
			{
				sceneNodeName = "" + ss.str() + StringConverter::toString(i);
				RibbonTrailRendererExVisualData* visualData = 
					PU_NEW_T(RibbonTrailRendererExVisualData, MEMCATEGORY_SCENE_OBJECTS)(mChildNode->createChildSceneNode(sceneNodeName), mTrail);
				visualData->node->setInheritOrientation(false);
				visualData->index = i;
				mAllVisualData.push_back(visualData); // Managed by this renderer
				mVisualData.push_back(visualData); // Used to assign to a particle
				if (mRandomInitialColour)
				{
					mTrail->setInitialColour(i, Math::UnitRandom(), Math::UnitRandom(), Math::UnitRandom());
				}
				else
				{
					mTrail->setInitialColour(i, mInitialColour);
				}
				mTrail->setColourChange(i, mColourChange);
				if (mSetWidth)
				{
					mTrail->setInitialWidth(i, _mRendererScale.x * mTrailWidth);
				}
				else
				{
					mTrail->setInitialWidth(i, _mRendererScale.x * mParentTechnique->getDefaultWidth());
				}
			}
			mChildNode->attachObject(mTrail);
			mRendererInitialised = true;
		}
	}
	//-----------------------------------------------------------------------
	void RibbonTrailRendererEx::_unprepare(ParticleTechnique* technique)
	{
		// Todo: The childnodes still exist, but because the parent node can also have other childnodes, they need to be deleted with caution
		mRendererInitialised = false; // Retriggers the _prepare function
		_destroyAll(); // Delete all nodes, they will be rebuild
	}
	//-----------------------------------------------------------------------
	void RibbonTrailRendererEx::_updateRenderQueue(Ogre::RenderQueue* queue, ParticlePool* pool)
	{
		// Always perform this one
		ParticleRenderer::_updateRenderQueue(queue, pool);

		if (!mVisible)
			return;

		Particle* particle = static_cast<Particle*>(pool->getFirst());
		RibbonTrailRendererExVisualData* visualData = 0;
		while (!pool->end())
		{
			if (particle)
			{
				if (!particle->visualData && !mVisualData.empty())
				{
					particle->visualData = mVisualData.back();
					mVisualData.pop_back();
					visualData = static_cast<RibbonTrailRendererExVisualData*>(particle->visualData);
				}
				visualData = static_cast<RibbonTrailRendererExVisualData*>(particle->visualData);
				if (visualData)
				{
					Ogre::SceneNode* node = visualData->node;
					node->_setDerivedPosition(particle->position);

					// Set the width of the trail if required
					if (particle->particleType == Particle::PT_VISUAL)
					{
						VisualParticle* visualParticle = static_cast<VisualParticle*>(particle);
						if (visualParticle->ownDimensions)
						{
							mTrail->setInitialWidth(visualData->index, visualParticle->width);
						}
					}
					visualData->setVisible(true);
				}
			}
			particle = static_cast<Particle*>(pool->getNext());
		}
	}
	//-----------------------------------------------------------------------
	void RibbonTrailRendererEx::_notifyAttached(Ogre::Node* parent, bool isTagPoint)
	{
		// No implementation here
	}
	//-----------------------------------------------------------------------
	void RibbonTrailRendererEx::_setMaterialName(const String& materialName)
	{
		if (mTrail)
		{
			mTrail->setMaterialName(materialName);
		}
	}
	//-----------------------------------------------------------------------
	void RibbonTrailRendererEx::_notifyCurrentCamera(Camera* cam)
	{
		if (mTrail)
		{
			mTrail->_notifyCurrentCamera(cam);
		}
	}
	//-----------------------------------------------------------------------
	void RibbonTrailRendererEx::_notifyParticleQuota(size_t quota)
	{
		mRendererInitialised = false; // Retriggers the _prepare function
		_destroyAll(); // Delete all nodes, they will be rebuild
	}
	//-----------------------------------------------------------------------
	void RibbonTrailRendererEx::_notifyDefaultDimensions(Real width, Real height, Real depth)
	{
		// No implementation
	}
	//-----------------------------------------------------------------------
	void RibbonTrailRendererEx::_notifyParticleResized(void)
	{
		// No implementation
	}
	//-----------------------------------------------------------------------
	void RibbonTrailRendererEx::_notifyParticleZRotated(void)
	{
		// No implementation
	}
	//-----------------------------------------------------------------------
	void RibbonTrailRendererEx::setRenderQueueGroup(uint8 queueId)
	{
		mQueueId = queueId;
		if (mTrail)
		{
			mTrail->setRenderQueueGroup(queueId);
		}
	}
	//-----------------------------------------------------------------------
	SortMode RibbonTrailRendererEx::_getSortMode(void) const
	{
		return SM_DISTANCE; // In fact there is no sorting
	}
	//-----------------------------------------------------------------------
	void RibbonTrailRendererEx::particleEmitted(ParticleTechnique* particleTechnique, Particle* particle)
	{
		// Assigning visual data already done in _updateRenderQueue()
	}
	//-----------------------------------------------------------------------
	void RibbonTrailRendererEx::particleExpired(ParticleTechnique* particleTechnique, Particle* particle)
	{
		if (particle->visualData)
		{
			RibbonTrailRendererExVisualData* ribbonTrailRendererExVisualData = static_cast<RibbonTrailRendererExVisualData*>(particle->visualData);
			ribbonTrailRendererExVisualData->setVisible(false);
		}
	}
	//-----------------------------------------------------------------------
	void RibbonTrailRendererEx::copyAttributesTo (ParticleRenderer* renderer)
	{
		// First copy parent attributes
		ParticleRenderer::copyAttributesTo(renderer);

		// First cast to RibbonTrailRendererEx
		RibbonTrailRendererEx* ribbonTrailRendererEx = static_cast<RibbonTrailRendererEx*>(renderer);
		ribbonTrailRendererEx->mUseVertexColours = mUseVertexColours;
		ribbonTrailRendererEx->mMaxChainElements = mMaxChainElements;
		ribbonTrailRendererEx->mTrailLength = mTrailLength;
		ribbonTrailRendererEx->mSetLength = mSetLength;
		ribbonTrailRendererEx->mTrailWidth = mTrailWidth;
		ribbonTrailRendererEx->mSetWidth = mSetWidth;
		ribbonTrailRendererEx->mRandomInitialColour = mRandomInitialColour;
		ribbonTrailRendererEx->mInitialColour = mInitialColour;
		ribbonTrailRendererEx->mColourChange = mColourChange;
		ribbonTrailRendererEx->mUVExtends = mUVExtends;
	}


	//-----------------------------------------------------------------------
	bool RibbonTrailRendererExTranslator::translateChildProperty(ScriptCompiler* compiler, const AbstractNodePtr &node)
	{
		PropertyAbstractNode* prop = reinterpret_cast<PropertyAbstractNode*>(node.get());
		ParticleRenderer* ren = any_cast<ParticleRenderer*>(prop->parent->context);
		RibbonTrailRendererEx* renderer = static_cast<RibbonTrailRendererEx*>(ren);

		if (prop->name == token[TOKEN_USE_VERTEX_COLOURS])
		{
			// Property: use_vertex_colours
			if (passValidateProperty(compiler, prop, token[TOKEN_USE_VERTEX_COLOURS], VAL_BOOL))
			{
				bool val;
				if(getBoolean(prop->values.front(), &val))
				{
					renderer->setUseVertexColours(val);
					return true;
				}
			}
		}
		else if (prop->name == token[TOKEN_RIBBONTRAIL_VERTEX_COLOURS])
		{
			// Property: ribbontrail_vertex_colours (deprecated and replaced by 'use_vertex_colours')
			if (passValidateProperty(compiler, prop, token[TOKEN_RIBBONTRAIL_VERTEX_COLOURS], VAL_BOOL))
			{
				bool val;
				if(getBoolean(prop->values.front(), &val))
				{
					renderer->setUseVertexColours(val);
					return true;
				}
			}
		}
		else if (prop->name == token[TOKEN_MAX_ELEMENTS])
		{
			// Property: max_elements
			if (passValidateProperty(compiler, prop, token[TOKEN_MAX_ELEMENTS], VAL_UINT))
			{
				uint val = 0;
				if(getUInt(prop->values.front(), &val))
				{
					renderer->setMaxChainElements(val);
					return true;
				}
			}
		}
		else if (prop->name == token[TOKEN_RIBBONTRAIL_MAX_ELEMENTS])
		{
			// Property: ribbontrail_max_elements (deprecated and replaced by 'max_elements')
			if (passValidateProperty(compiler, prop, token[TOKEN_RIBBONTRAIL_MAX_ELEMENTS], VAL_UINT))
			{
				uint val = 0;
				if(getUInt(prop->values.front(), &val))
				{
					renderer->setMaxChainElements(val);
					return true;
				}
			}
		}
		else if (prop->name == token[TOKEN_RIBBONTRAIL_LENGTH])
		{
			// Property: ribbontrail_length
			if (passValidateProperty(compiler, prop, token[TOKEN_RIBBONTRAIL_MAX_ELEMENTS], VAL_REAL))
			{
				Real val = 0;
				if(getReal(prop->values.front(), &val))
				{
					renderer->setTrailLength(val);
					return true;
				}
			}
		}
		else if (prop->name == token[TOKEN_RIBBONTRAIL_WIDTH])
		{
			// Property: ribbontrail_width
			if (passValidateProperty(compiler, prop, token[TOKEN_RIBBONTRAIL_WIDTH], VAL_REAL))
			{
				Real val = 0;
				if(getReal(prop->values.front(), &val))
				{
					renderer->setTrailWidth(val);
					return true;
				}
			}
		}
		else if (prop->name == token[TOKEN_RANDOM_INITIAL_COLOUR])
		{
			// Property: random_initial_colour
			if (passValidateProperty(compiler, prop, token[TOKEN_RANDOM_INITIAL_COLOUR], VAL_BOOL))
			{
				bool val;
				if(getBoolean(prop->values.front(), &val))
				{
					renderer->setRandomInitialColour(val);
					return true;
				}
			}
		}
		else if (prop->name == token[TOKEN_RIBBONTRAIL_RANDOM_INITIAL_COLOUR])
		{
			// Property: ribbontrail_random_initial_colour (deprecated and replaced by 'random_initial_colour'))
			if (passValidateProperty(compiler, prop, token[TOKEN_RIBBONTRAIL_RANDOM_INITIAL_COLOUR], VAL_BOOL))
			{
				bool val;
				if(getBoolean(prop->values.front(), &val))
				{
					renderer->setRandomInitialColour(val);
					return true;
				}
			}
		}
		else if (prop->name == token[TOKEN_INITIAL_COLOUR])
		{
			// Property: initial_colour
			if (passValidateProperty(compiler, prop, token[TOKEN_INITIAL_COLOUR], VAL_COLOURVALUE))
			{
				ColourValue val;
				if(getColour(prop->values.begin(), prop->values.end(), &val))
				{
					renderer->setInitialColour(val);
					return true;
				}
			}
		}
		else if (prop->name == token[TOKEN_RIBBONTRAIL_INITIAL_COLOUR])
		{
			// Property: ribbontrail_initial_colour (deprecated and replaced by 'initial_colour')
			if (passValidateProperty(compiler, prop, token[TOKEN_RIBBONTRAIL_INITIAL_COLOUR], VAL_COLOURVALUE))
			{
				ColourValue val;
				if(getColour(prop->values.begin(), prop->values.end(), &val))
				{
					renderer->setInitialColour(val);
					return true;
				}
			}
		}
		else if (prop->name == token[TOKEN_COLOUR_CHANGE])
		{
			// Property: colour_change
			if (passValidateProperty(compiler, prop, token[TOKEN_COLOUR_CHANGE], VAL_COLOURVALUE))
			{
				ColourValue val;
				if(getColour(prop->values.begin(), prop->values.end(), &val))
				{
					renderer->setColourChange(val);
					return true;
				}
			}
		}
		else if (prop->name == token[TOKEN_RIBBONTRAIL_COLOUR_CHANGE])
		{
			// Property: ribbontrail_colour_change (deprecated and replaced by 'colour_change')
			if (passValidateProperty(compiler, prop, token[TOKEN_RIBBONTRAIL_COLOUR_CHANGE], VAL_COLOURVALUE))
			{
				ColourValue val;
				if(getColour(prop->values.begin(), prop->values.end(), &val))
				{
					renderer->setColourChange(val);
					return true;
				}
			}
		}
		else if (prop->name == UVEXTENDS_TOKEN)
		{
			if (passValidateProperty(compiler, prop, UVEXTENDS_TOKEN, VAL_BOOL))
			{
				bool val;
				if(getBoolean(prop->values.front(), &val))
				{
					renderer->setUVExtends(val);
					return true;
				}
			}
		}

		return false;
	}
	//-----------------------------------------------------------------------
	bool RibbonTrailRendererExTranslator::translateChildObject(ScriptCompiler* compiler, const AbstractNodePtr &node)
	{
		// No objects
		return false;
	}
	//-----------------------------------------------------------------------
	//-----------------------------------------------------------------------
	//-----------------------------------------------------------------------
	void RibbonTrailRendererExWriter::write(ParticleScriptSerializer* serializer, const IElement* element)
	{
		// Cast the element to a RibbonTrailRendererEx
		const RibbonTrailRendererEx* renderer = static_cast<const RibbonTrailRendererEx*>(element);

		// Write the header of the LightRenderer
		serializer->writeLine(token[TOKEN_RENDERER], renderer->getRendererType(), 8);
		serializer->writeLine("{", 8);

		// Write base attributes
		ParticleRendererWriter::write(serializer, element);

		// Write own attributes
		if (renderer->isUseVertexColours() != RibbonTrailRendererEx::DEFAULT_USE_VERTEX_COLOURS) serializer->writeLine(
			token[TOKEN_USE_VERTEX_COLOURS], StringConverter::toString(renderer->isUseVertexColours()), 12);
		if (renderer->getMaxChainElements() != RibbonTrailRendererEx::DEFAULT_MAX_ELEMENTS) serializer->writeLine(
			token[TOKEN_MAX_ELEMENTS], StringConverter::toString(renderer->getMaxChainElements()), 12);
		if (renderer->getTrailLength() != RibbonTrailRendererEx::DEFAULT_LENGTH) serializer->writeLine(
			token[TOKEN_RIBBONTRAIL_LENGTH], StringConverter::toString(renderer->getTrailLength()), 12);
		if (renderer->getTrailWidth() != RibbonTrailRendererEx::DEFAULT_WIDTH) serializer->writeLine(
			token[TOKEN_RIBBONTRAIL_WIDTH], StringConverter::toString(renderer->getTrailWidth()), 12);
		if (renderer->isRandomInitialColour() != RibbonTrailRendererEx::DEFAULT_RANDOM_INITIAL_COLOUR) serializer->writeLine(
			token[TOKEN_RANDOM_INITIAL_COLOUR], StringConverter::toString(renderer->isRandomInitialColour()), 12);
		if (renderer->getInitialColour() != RibbonTrailRendererEx::DEFAULT_INITIAL_COLOUR) serializer->writeLine(
			token[TOKEN_INITIAL_COLOUR], StringConverter::toString(renderer->getInitialColour()), 12);
		if (renderer->getColourChange() != RibbonTrailRendererEx::DEFAULT_COLOUR_CHANGE) serializer->writeLine(
			token[TOKEN_COLOUR_CHANGE], StringConverter::toString(renderer->getColourChange()), 12);
		if (renderer->isUVExtends() != RibbonTrailRendererEx::DEFAULT_UV_EXTENDS) serializer->writeLine(
			UVEXTENDS_TOKEN, StringConverter::toString(renderer->isUVExtends()), 12);
		// Write the close bracket
		serializer->writeLine("}", 8);
	}

}
