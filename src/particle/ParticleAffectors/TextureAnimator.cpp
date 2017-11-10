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

#include "ParticleAffectors/ParticleUniverseTextureAnimator.h"


#include "seed_memory_leak_check.h" //the last include head file,no include after this line
namespace egal
{
	// Constants
	const Real TextureAnimator::DEFAULT_TIME_STEP = 0.0f;
	const uint16 TextureAnimator::DEFAULT_TEXCOORDS_START = 0;
	const uint16 TextureAnimator::DEFAULT_TEXCOORDS_END = 0;
	const TextureAnimator::TextureAnimationType TextureAnimator::DEFAULT_ANIMATION_TYPE = TextureAnimator::TAT_LOOP;
	const bool TextureAnimator::DEFAULT_START_RANDOM = true;

	//-----------------------------------------------------------------------
	TextureAnimator::TextureAnimator(void) : 
		ParticleAffector(),
		mAnimationTimeStep(DEFAULT_TIME_STEP),
		mAnimationTimeStepSet(false),
		mNextIndex(false),
		mTextureAnimationType(DEFAULT_ANIMATION_TYPE),
		mTextureCoordsStart(DEFAULT_TEXCOORDS_START),
		mTextureCoordsEnd(DEFAULT_TEXCOORDS_END),
		mStartRandom(DEFAULT_START_RANDOM),
		mAnimationTimeStepCount(0.0f),
		mTextrueCoordsCur(-1)
	{
	}
	//-----------------------------------------------------------------------
	TextureAnimator::~TextureAnimator(void)
	{
	}
	//-----------------------------------------------------------------------
	Real TextureAnimator::getAnimationTimeStep(void) const
	{
		return mAnimationTimeStep;
	}
	//-----------------------------------------------------------------------
	void TextureAnimator::setAnimationTimeStep(Real animationTimeStep)
	{
		mAnimationTimeStep = animationTimeStep;
		mAnimationTimeStepSet = true;
	}
	//-----------------------------------------------------------------------
	TextureAnimator::TextureAnimationType TextureAnimator::getTextureAnimationType(void) const
	{
		return mTextureAnimationType;
	}
	//-----------------------------------------------------------------------
	void TextureAnimator::setTextureAnimationType(TextureAnimator::TextureAnimationType textureAnimationType)
	{
		mTextureAnimationType = textureAnimationType;
	}
	//-----------------------------------------------------------------------
	uint16 TextureAnimator::getTextureCoordsStart(void) const
	{
		return mTextureCoordsStart;
	}
	//-----------------------------------------------------------------------
	void TextureAnimator::setTextureCoordsStart(uint16 textureCoordsStart)
	{
		mTextureCoordsStart = textureCoordsStart;
	}
	//-----------------------------------------------------------------------
	uint16 TextureAnimator::getTextureCoordsEnd(void) const
	{
		return mTextureCoordsEnd;
	}
	//-----------------------------------------------------------------------
	void TextureAnimator::setTextureCoordsEnd(uint16 textureCoordsEnd)
	{
		mTextureCoordsEnd = textureCoordsEnd;
	}
	//-----------------------------------------------------------------------
	bool TextureAnimator::isStartRandom(void) const
	{
		return mStartRandom;
	}
	//-----------------------------------------------------------------------
	void TextureAnimator::setStartRandom(bool startRandom)
	{
		mStartRandom = startRandom;
	}
	//-----------------------------------------------------------------------
	void TextureAnimator::_initParticleForEmission(Particle* particle)
	{
		// Only continue if the particle is a visual particle
		if (particle->particleType != Particle::PT_VISUAL)
			return;

		VisualParticle* visualParticle = static_cast<VisualParticle*>(particle);

		// Set first image
		if (mStartRandom)
		{
			visualParticle->textureCoordsCurrent = (uint16)Math::RangeRandom((Real)mTextureCoordsStart, (Real)mTextureCoordsEnd + 0.999f);
		}
		else
		{
			visualParticle->textureCoordsCurrent = mTextureCoordsStart;
		}

		// Calculate the animationTimeStep
		if (!mAnimationTimeStepSet)
		{
			// Set the animation time step for each particle
			switch(mTextureAnimationType)
			{
				case TAT_LOOP:
				{
					visualParticle->textureAnimationTimeStep = visualParticle->timeToLive / (mTextureCoordsEnd - mTextureCoordsStart + 1);
				}
				break;

				case TAT_UP_DOWN:
				{
					visualParticle->textureAnimationTimeStep = visualParticle->timeToLive / (2 * (mTextureCoordsEnd - mTextureCoordsStart) + 1);
				}
				break;

				case TAT_RANDOM:
				{
					visualParticle->textureAnimationTimeStep = visualParticle->timeToLive;
				}
				break;
			}
		}
	}
	//-----------------------------------------------------------------------
	void TextureAnimator::_preProcessParticles(ParticleTechnique* particleTechnique, Real timeElapsed)
	{
		// Determine the next texture coords index (global)
		if (mAnimationTimeStepSet)
		{
			mNextIndex = false;
			mAnimationTimeStepCount += timeElapsed;
			if (mAnimationTimeStepCount > mAnimationTimeStep)
			{
				mAnimationTimeStepCount -= mAnimationTimeStep;
				mNextIndex = true;
			}
		}

		if (mTextrueCoordsCur < 0 || mTextrueCoordsCur > mTextureCoordsEnd - 1)
			mTextrueCoordsCur = mTextureCoordsStart;
		else
		{
			switch (mTextureAnimationType)
			{
			case TAT_LOOP:
				mTextrueCoordsCur++;
				break;

			case TAT_RANDOM:
				mTextrueCoordsCur = (uint16)Math::RangeRandom((Real)mTextureCoordsStart, (Real)mTextureCoordsEnd + 0.999f);
				break;
			}
		}

		particleTechnique->setCurTile(mTextrueCoordsCur);

	}
	//-----------------------------------------------------------------------
	void TextureAnimator::_affect(ParticleTechnique* particleTechnique, Particle* particle, Real timeElapsed)
	{
		// Only continue if the particle is a visual particle
		if (particle->particleType != Particle::PT_VISUAL)
			return;

		VisualParticle* visualParticle = static_cast<VisualParticle*>(particle);

		// Determine the next texture coords index
		if (mAnimationTimeStepSet)
		{
			if (mNextIndex)
			{
				// Use the global one for all particles
				_determineNextTextureCoords(visualParticle);
			}
		}
		else
		{
			visualParticle->textureAnimationTimeStepCount += timeElapsed;
			if (visualParticle->textureAnimationTimeStepCount > visualParticle->textureAnimationTimeStep)
			{
				visualParticle->textureAnimationTimeStepCount -= visualParticle->textureAnimationTimeStep;
				_determineNextTextureCoords(visualParticle);
			}
		}
	}
	//-----------------------------------------------------------------------
	void TextureAnimator::_determineNextTextureCoords(VisualParticle* visualParticle)
	{
		switch(mTextureAnimationType)
		{
			case TAT_LOOP:
			{
				if (visualParticle->textureCoordsCurrent >= mTextureCoordsEnd)
				{
					visualParticle->textureCoordsCurrent = mTextureCoordsStart;
				}
				else
				{
					(visualParticle->textureCoordsCurrent)++;
				}
			}
			break;

			case TAT_UP_DOWN:
			{
				if (visualParticle->textureAnimationDirectionUp == true)
				{
					// Going up
					if (visualParticle->textureCoordsCurrent >= mTextureCoordsEnd)
					{
						(visualParticle->textureCoordsCurrent)--;
						visualParticle->textureAnimationDirectionUp = false;
					}
					else
					{
						(visualParticle->textureCoordsCurrent)++;
					}
				}
				else
				{
					// Going down
					if (visualParticle->textureCoordsCurrent <= mTextureCoordsStart)
					{
						(visualParticle->textureCoordsCurrent)++;
						visualParticle->textureAnimationDirectionUp = true;
					}
					else
					{
						(visualParticle->textureCoordsCurrent)--;
					}
				}
			}
			break;

			case TAT_RANDOM:
			{
				// Generate a random texcoord index
				visualParticle->textureCoordsCurrent = (uint16)Math::RangeRandom((Real)mTextureCoordsStart, (Real)mTextureCoordsEnd + 0.999f);
			}
			break;
		}
	}

	//-----------------------------------------------------------------------
	void TextureAnimator::copyAttributesTo (ParticleAffector* affector)
	{
		ParticleAffector::copyAttributesTo(affector);
		TextureAnimator* textureAnimator = static_cast<TextureAnimator*>(affector);
		textureAnimator->mAnimationTimeStep = mAnimationTimeStep;
		textureAnimator->mAnimationTimeStepSet = mAnimationTimeStepSet;
		textureAnimator->mTextureAnimationType = mTextureAnimationType;
		textureAnimator->mTextureCoordsStart = mTextureCoordsStart;
		textureAnimator->mTextureCoordsEnd = mTextureCoordsEnd;
		textureAnimator->mStartRandom = mStartRandom;
	}

}
