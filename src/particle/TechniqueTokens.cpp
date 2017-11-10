/*
-----------------------------------------------------------------------------------------------
This source file is part of the Particle Universe product.

Copyright (c) 2012 Henry van Merode

Usage of this program is licensed under the terms of the Particle Universe Commercial License.
You can find a copy of the Commercial License in the Particle Universe package.
-----------------------------------------------------------------------------------------------
*/

#include "PCH.h"

#ifndef PARTICLE_UNIVERSE_EXPORTS
#define PARTICLE_UNIVERSE_EXPORTS
#endif

#include "TechniqueTokens.h"
#include "CameraDependency.h"


#include "seed_memory_leak_check.h" //the last include head file,no include after this line
namespace egal
{
	/**************************************************************************
	 * TechniqueTranslator
	 *************************************************************************/
	TechniqueTranslator::TechniqueTranslator()
		:mTechnique(0)
	{
	}
	//-------------------------------------------------------------------------
	void TechniqueTranslator::translate(ScriptCompiler* compiler, const AbstractNodePtr &node)
	{
		ObjectAbstractNode* obj = reinterpret_cast<ObjectAbstractNode*>(node.get());
		ObjectAbstractNode* parent = obj->parent ? reinterpret_cast<ObjectAbstractNode*>(obj->parent) : 0;
		
		// Create the technique
		mTechnique = ParticleSystemManager::getSingletonPtr()->createTechnique();
		if (!mTechnique)
		{
			compiler->addError(ScriptCompiler::CE_INVALIDPARAMETERS, obj->file, obj->line);
			return;
		}

		if (!obj->parent->context.isEmpty())
		{
			ParticleSystem* system = any_cast<ParticleSystem*>(obj->parent->context);
			system->addTechnique(mTechnique);
		}
		else
		{
			// It is an alias
			mTechnique->setAliasName(parent->name); // PU 1.4
			ParticleSystemManager::getSingletonPtr()->addAlias(mTechnique);
		}

		mTechnique->setName(obj->name);
		obj->context = Any(mTechnique); // Add this to the context, because it is needed for the underlying emitters, affectors, ...

		// Get the name of the technique
		if(!obj->name.empty())
			mTechnique->setName(obj->name);

		for(AbstractNodeList::iterator i = obj->children.begin(); i != obj->children.end(); ++i)
		{
			if((*i)->type == ANT_PROPERTY)
			{
				PropertyAbstractNode* prop = reinterpret_cast<PropertyAbstractNode*>((*i).get());
				if (prop->name == token[TOKEN_ENABLED])
				{
					// Property: enabled
					if (passValidateProperty(compiler, prop, token[TOKEN_ENABLED], VAL_BOOL))
					{
						bool val;
						if(getBoolean(prop->values.front(), &val))
						{
							mTechnique->setEnabled(val);
						}
					}
				}
				else if (prop->name == token[TOKEN_POSITION])
				{
					// Property: positon
					if (passValidateProperty(compiler, prop, token[TOKEN_POSITION], VAL_VECTOR3))
					{
						Vector3 val;
						if(getVector3(prop->values.begin(), prop->values.end(), &val))
						{
							mTechnique->position = val;
							mTechnique->originalPosition = val;
						}
					}
				}
				else if (prop->name == token[TOKEN_KEEP_LOCAL])
				{
					// Property: keep_local
					if (passValidateProperty(compiler, prop, token[TOKEN_KEEP_LOCAL], VAL_BOOL))
					{
						bool val;
						if(getBoolean(prop->values.front(), &val))
						{
							mTechnique->setKeepLocal(val);
						}
					}
				}
				else if (prop->name == token[TOKEN_TECH_VISUAL_PARTICLE_QUOTA])
				{
					// Property: visual_particle_quota
					if (passValidateProperty(compiler, prop, token[TOKEN_TECH_VISUAL_PARTICLE_QUOTA], VAL_UINT))
					{
						uint val = 0;
						if(getUInt(prop->values.front(), &val))
						{
							mTechnique->setVisualParticleQuota(val);
						}
					}
				}
				else if (prop->name == token[TOKEN_TECH_EMITTED_EMITTER_QUOTA])
				{
					// Property: emitted_emitter_quota
					if (passValidateProperty(compiler, prop, token[TOKEN_TECH_EMITTED_EMITTER_QUOTA], VAL_UINT))
					{
						uint val = 0;
						if(getUInt(prop->values.front(), &val))
						{
							mTechnique->setEmittedEmitterQuota(val);
						}
					}
				}
				else if (prop->name == token[TOKEN_TECH_EMITTED_AFFECTOR_QUOTA])
				{
					// Property: emitted_affector_quota
					if (passValidateProperty(compiler, prop, token[TOKEN_TECH_EMITTED_AFFECTOR_QUOTA], VAL_UINT))
					{
						uint val = 0;
						if(getUInt(prop->values.front(), &val))
						{
							mTechnique->setEmittedAffectorQuota(val);
						}
					}
				}
				else if (prop->name == token[TOKEN_TECH_EMITTED_TECHNIQUE_QUOTA])
				{
					// Property: emitted_technique_quota
					if (passValidateProperty(compiler, prop, token[TOKEN_TECH_EMITTED_TECHNIQUE_QUOTA], VAL_UINT))
					{
						uint val = 0;
						if(getUInt(prop->values.front(), &val))
						{
							mTechnique->setEmittedTechniqueQuota(val);
						}
					}
				}
				else if (prop->name == token[TOKEN_TECH_EMITTED_SYSTEM_QUOTA])
				{
					// Property: emitted_system_quota
					if (passValidateProperty(compiler, prop, token[TOKEN_TECH_EMITTED_SYSTEM_QUOTA], VAL_UINT))
					{
						uint val = 0;
						if(getUInt(prop->values.front(), &val))
						{
							mTechnique->setEmittedSystemQuota(val);
						}
					}
				}
				else if (prop->name == token[TOKEN_MATERIAL])
				{
					// Property: material
					if (passValidateProperty(compiler, prop, token[TOKEN_MATERIAL], VAL_STRING))
					{
						String val;
						if(getString(prop->values.front(), &val))
						{
							mTechnique->setMaterialName(val);
						}
					}
				}
				else if (prop->name == token[TOKEN_TECH_LOD_INDEX])
				{
					// Property: lod_index
					if (passValidateProperty(compiler, prop, token[TOKEN_TECH_LOD_INDEX], VAL_UINT))
					{
						uint val = 0;
						if(getUInt(prop->values.front(), &val))
						{
							mTechnique->setLodIndex(val);
						}
					}
				}
				else if (prop->name == token[TOKEN_TECH_DEFAULT_PARTICLE_WIDTH])
				{
					// Property: default_particle_width
					if (passValidateProperty(compiler, prop, token[TOKEN_TECH_DEFAULT_PARTICLE_WIDTH], VAL_REAL))
					{
						Real val = 0.0f;
						if(getReal(prop->values.front(), &val))
						{
							mTechnique->setDefaultWidth(val);
						}
					}
				}
				else if (prop->name == token[TOKEN_TECH_DEFAULT_PARTICLE_HEIGHT])
				{
					// Property: default_particle_height
					if (passValidateProperty(compiler, prop, token[TOKEN_TECH_DEFAULT_PARTICLE_HEIGHT], VAL_REAL))
					{
						Real val = 0.0f;
						if(getReal(prop->values.front(), &val))
						{
							mTechnique->setDefaultHeight(val);
						}
					}
				}
				else if (prop->name == token[TOKEN_TECH_DEFAULT_PARTICLE_DEPTH])
				{
					// Property: default_particle_depth
					if (passValidateProperty(compiler, prop, token[TOKEN_TECH_DEFAULT_PARTICLE_DEPTH], VAL_REAL))
					{
						Real val = 0.0f;
						if(getReal(prop->values.front(), &val))
						{
							mTechnique->setDefaultDepth(val);
						}
					}
				}
				else if (prop->name == token[TOKEN_TECH_SPHASHING_CELL_DIMENSION])
				{
					// Property: spatial_hashing_cell_dimension
					if (passValidateProperty(compiler, prop, token[TOKEN_TECH_SPHASHING_CELL_DIMENSION], VAL_UINT))
					{
						uint val = 0;
						if(getUInt(prop->values.front(), &val))
						{
							mTechnique->setSpatialHashingCellDimension(val);
						}
					}
				}
				else if (prop->name == token[TOKEN_TECH_SPHASHING_CELL_OVERLAP])
				{
					// Property: spatial_hashing_cell_overlap
					if (passValidateProperty(compiler, prop, token[TOKEN_TECH_SPHASHING_CELL_OVERLAP], VAL_UINT))
					{
						uint val = 0;
						if(getUInt(prop->values.front(), &val))
						{
							mTechnique->setSpatialHashingCellOverlap(val);
						}
					}
				}
				else if (prop->name == token[TOKEN_TECH_SPHASHING_SIZE])
				{
					// Property: spatial_hashtable_size
					if (passValidateProperty(compiler, prop, token[TOKEN_TECH_SPHASHING_SIZE], VAL_UINT))
					{
						uint val = 0;
						if(getUInt(prop->values.front(), &val))
						{
							mTechnique->setSpatialHashTableSize(val);
						}
					}
				}
				else if (prop->name == token[TOKEN_TECH_SPHASHING_UPDATE_INTERVAL])
				{
					// Property: spatial_hashing_update_interval
					if (passValidateProperty(compiler, prop, token[TOKEN_TECH_SPHASHING_UPDATE_INTERVAL], VAL_REAL))
					{
						Real val = 0.0f;
						if(getReal(prop->values.front(), &val))
						{
							mTechnique->setSpatialHashingInterval(val);
						}
					}
				}
				else if (prop->name == token[TOKEN_TECH_MAX_VELOCITY])
				{
					// Property: max_velocity
					if (passValidateProperty(compiler, prop, token[TOKEN_TECH_MAX_VELOCITY], VAL_REAL))
					{
						Real val = 0.0f;
						if(getReal(prop->values.front(), &val))
						{
							mTechnique->setMaxVelocity(val);
						}
					}
				}
				else if (prop->name == token[TOKEN_TECH_DIFFUSE_TEXTURE])
				{
					// Property: emission_rate
					// If it is a property, it is a fixed value
					if (passValidateProperty(compiler, prop, token[TOKEN_TECH_DIFFUSE_TEXTURE], VAL_STRING))
					{
						String val;
						if (getString(prop->values.front(), &val))
						{
							mTechnique->setDiffuseTextureName(val);
						}
					}
				}
				else if (prop->name == token[TOKEN_TECH_DEPTH_CHECK_WRITE])
				{
					// Property: emission_rate
					// If it is a property, it is a fixed value
					if (passValidateProperty(compiler, prop, token[TOKEN_TECH_DEPTH_CHECK_WRITE], VAL_BOOL))
					{
						bool val;
						if (getBoolean(prop->values.front(), &val))
						{
							mTechnique->setDepthCheckAndWrite(val);
						}
					}
				}
				else if (prop->name == token[TOKEN_TECH_DOUBLE_RENDER])
				{
					// Property: emission_rate
					// If it is a property, it is a fixed value
					if (passValidateProperty(compiler, prop, token[TOKEN_TECH_DOUBLE_RENDER], VAL_BOOL))
					{
						bool val;
						if (getBoolean(prop->values.front(), &val))
						{
							mTechnique->setDoubleRender(val);
						}
					}
				}
				else if (prop->name == token[TOKEN_TECH_SCENE_BLEND])
				{
					// Property: emission_rate
					// If it is a property, it is a fixed value
					if (passValidateProperty(compiler, prop, token[TOKEN_TECH_SCENE_BLEND], VAL_STRING))
					{
						String val;
						if (getString(prop->values.front(), &val))
						{
							mTechnique->setSourceFactor(val);
						}
					}
				}
				else if (prop->name == token[TOKEN_TECH_ALPHA_VALUE])
				{
					// Property: emission_rate
					// If it is a property, it is a fixed value
					if (passValidateProperty(compiler, prop, token[TOKEN_TECH_ALPHA_VALUE], VAL_REAL))
					{
						Real val;
						if (getReal(prop->values.front(), &val))
						{
							mTechnique->setSFAlphaValue(val);
						}
					}
				}
				else if (prop->name == token[TOKEN_TECH_UV_MASK_TEXTURE])
				{
					// Property: emission_rate
					// If it is a property, it is a fixed value
					if (passValidateProperty(compiler, prop, token[TOKEN_TECH_UV_MASK_TEXTURE], VAL_STRING))
					{
						String val;
						if (getString(prop->values.front(), &val))
						{
							mTechnique->setUVMaskTexture(val);
						}
					}
				}
				else if (prop->name == token[TOKEN_TECH_UV_CLAMP])
				{
					// Property: emission_rate
					// If it is a property, it is a fixed value
					if (passValidateProperty(compiler, prop, token[TOKEN_TECH_UV_CLAMP], VAL_VECTOR3))
					{
						Vector3 val;
						if (getVector3(prop->values.begin(), prop->values.end(), &val))
						{
							mTechnique->setUVClamp(val);
						}
					}
				}
				else if (prop->name == token[TOKEN_TECH_FOG_OVERRIDE])
				{
					// Property: emission_rate
					// If it is a property, it is a fixed value
					if (passValidateProperty(compiler, prop, token[TOKEN_TECH_FOG_OVERRIDE], VAL_BOOL))
					{
						bool val;
						if (getBoolean(prop->values.front(), &val))
						{
							mTechnique->setRenderWithFog(val);
						}
					}
				}
				else if (prop->name == token[TOKEN_TECH_DISSIPATE_TEXTURE])
				{
					// Property: emission_rate
					// If it is a property, it is a fixed value
					if (passValidateProperty(compiler, prop, token[TOKEN_TECH_DISSIPATE_TEXTURE], VAL_STRING))
					{
						String val;
						if (getString(prop->values.front(), &val))
						{
							mTechnique->setDissipateTexture(val);
						}
					}
				}
				else if (prop->name == token[TOKEN_USE_ALIAS])
				{
					// Property: use_alias
					if (passValidateProperty(compiler, prop, token[TOKEN_USE_ALIAS], VAL_STRING))
					{
						String val;
						if(getString(prop->values.front(), &val))
						{
							IAlias* alias = ParticleSystemManager::getSingletonPtr()->getAlias(val);
							switch (alias->getAliasType())
							{
								case IAlias::AT_RENDERER:
								{
									ParticleRenderer* renderer = static_cast<ParticleRenderer*>(alias);
									ParticleRenderer* newRenderer = ParticleSystemManager::getSingletonPtr()->cloneRenderer(renderer);
									mTechnique->setRenderer(newRenderer);
								}
								break;

								case IAlias::AT_EMITTER:
								{
									ParticleEmitter* emitter = static_cast<ParticleEmitter*>(alias);
									ParticleEmitter* newEmitter = ParticleSystemManager::getSingletonPtr()->cloneEmitter(emitter);
									mTechnique->addEmitter(newEmitter);
								}
								break;

								case IAlias::AT_AFFECTOR:
								{
									ParticleAffector* affector = static_cast<ParticleAffector*>(alias);
									ParticleAffector* newAffector = ParticleSystemManager::getSingletonPtr()->cloneAffector(affector);
									mTechnique->addAffector(newAffector);
								}
								break;

								case IAlias::AT_OBSERVER:
								{
									ParticleObserver* observer = static_cast<ParticleObserver*>(alias);
									ParticleObserver* newObserver = ParticleSystemManager::getSingletonPtr()->cloneObserver(observer);
									mTechnique->addObserver(newObserver);
								}
								break;

								case IAlias::AT_EXTERN:
								{
									Extern* externObject = static_cast<Extern*>(alias);
									Extern* newExternObject = ParticleSystemManager::getSingletonPtr()->cloneExtern(externObject);
									mTechnique->addExtern(newExternObject);
								}
								break;

								case IAlias::AT_BEHAVIOUR:
								{
									ParticleBehaviour* behaviour = static_cast<ParticleBehaviour*>(alias);
									ParticleBehaviour* newBehaviour = ParticleSystemManager::getSingletonPtr()->cloneBehaviour(behaviour);
									mTechnique->_addBehaviourTemplate(newBehaviour);
								}
								break;
							}
						}
					}
				}
				else if (prop->name == token[TOKEN_TECH_COLOR_SCALE])
				{
					// Property: time_to_live
					// If it is a property, it is a fixed value
					if (passValidateProperty(compiler, prop, token[TOKEN_TECH_COLOR_SCALE], VAL_REAL))
					{
						Real val = 0.0f;
						if (getReal(prop->values.front(), &val))
						{
							DynamicAttributeFixed* dynamicAttributeFixed = PU_NEW_T(DynamicAttributeFixed, MEMCATEGORY_SCENE_OBJECTS)();
							dynamicAttributeFixed->setValue(val);
							mTechnique->setDynColorScale(dynamicAttributeFixed);
						}
					}
				}
				else if (prop->name == token[TOKEN_TECH_UV_U_SPEED])
				{
					// Property: time_to_live
					// If it is a property, it is a fixed value
					if (passValidateProperty(compiler, prop, token[TOKEN_TECH_UV_U_SPEED], VAL_REAL))
					{
						Real val = 0.0f;
						if (getReal(prop->values.front(), &val))
						{
							DynamicAttributeFixed* dynamicAttributeFixed = PU_NEW_T(DynamicAttributeFixed, MEMCATEGORY_SCENE_OBJECTS)();
							dynamicAttributeFixed->setValue(val);
							mTechnique->setDynUVAnimation_U(dynamicAttributeFixed);
						}
					}
				}
				else if (prop->name == token[TOKEN_TECH_UV_V_SPEED])
				{
					// Property: time_to_live
					// If it is a property, it is a fixed value
					if (passValidateProperty(compiler, prop, token[TOKEN_TECH_UV_V_SPEED], VAL_REAL))
					{
						Real val = 0.0f;
						if (getReal(prop->values.front(), &val))
						{
							DynamicAttributeFixed* dynamicAttributeFixed = PU_NEW_T(DynamicAttributeFixed, MEMCATEGORY_SCENE_OBJECTS)();
							dynamicAttributeFixed->setValue(val);
							mTechnique->setDynUVAnimation_V(dynamicAttributeFixed);
						}
					}
				}
				else if (prop->name == token[TOKEN_TECH_DISSIPATE_TIME])
				{
					// Property: time_to_live
					// If it is a property, it is a fixed value
					if (passValidateProperty(compiler, prop, token[TOKEN_TECH_DISSIPATE_TIME], VAL_REAL))
					{
						Real val = 0.0f;
						if (getReal(prop->values.front(), &val))
						{
							DynamicAttributeFixed* dynamicAttributeFixed = PU_NEW_T(DynamicAttributeFixed, MEMCATEGORY_SCENE_OBJECTS)();
							dynamicAttributeFixed->setValue(val);
							mTechnique->setDynDissipateTime(dynamicAttributeFixed);
						}
					}
				}
				else if (prop->name == token[TOKEN_TECH_DISSIPATE_DELAY_TIME])
				{
					// Property: time_to_live
					// If it is a property, it is a fixed value
					if (passValidateProperty(compiler, prop, token[TOKEN_TECH_DISSIPATE_DELAY_TIME], VAL_REAL))
					{
						Real val = 0.0f;
						if (getReal(prop->values.front(), &val))
						{
							DynamicAttributeFixed* dynamicAttributeFixed = PU_NEW_T(DynamicAttributeFixed, MEMCATEGORY_SCENE_OBJECTS)();
							dynamicAttributeFixed->setValue(val);
							mTechnique->setDynDissipateDelayTime(dynamicAttributeFixed);
						}
					}
				}
				else if (prop->name == token[TOKEN_TECH_DISSIPATE_VALUE])
				{
					// Property: time_to_live
					// If it is a property, it is a fixed value
					if (passValidateProperty(compiler, prop, token[TOKEN_TECH_DISSIPATE_VALUE], VAL_REAL))
					{
						Real val = 0.0f;
						if (getReal(prop->values.front(), &val))
						{
							DynamicAttributeFixed* dynamicAttributeFixed = PU_NEW_T(DynamicAttributeFixed, MEMCATEGORY_SCENE_OBJECTS)();
							dynamicAttributeFixed->setValue(val);
							mTechnique->setDynDissipateValue(dynamicAttributeFixed);
						}
					}
				}
				else if (prop->name == token[TOKEN_TECH_DISSIPATE_SOFT_EDGE])
				{
					// Property: time_to_live
					// If it is a property, it is a fixed value
					if (passValidateProperty(compiler, prop, token[TOKEN_TECH_DISSIPATE_SOFT_EDGE], VAL_REAL))
					{
						Real val = 0.0f;
						if (getReal(prop->values.front(), &val))
						{
							DynamicAttributeFixed* dynamicAttributeFixed = PU_NEW_T(DynamicAttributeFixed, MEMCATEGORY_SCENE_OBJECTS)();
							dynamicAttributeFixed->setValue(val);
							mTechnique->setDynDissipateSoftEdgeValue(dynamicAttributeFixed);
						}
					}
				}
				else
				{
					errorUnexpectedProperty(compiler, prop);
				}
			}
			else if((*i)->type == ANT_OBJECT)
			{
				ObjectAbstractNode* child = reinterpret_cast<ObjectAbstractNode*>((*i).get());
				DynamicAttributeTranslator dynamicAttributeTranslator;
				if (child->cls == token[TOKEN_TECH_COLOR_SCALE])
				{
					// Property: emission_rate
					dynamicAttributeTranslator.translate(compiler, *i);
					DynamicAttribute* dynamicAttribute = any_cast<DynamicAttribute*>(child->context);
					mTechnique->setDynColorScale(dynamicAttribute);
				}
				else if (child->cls == token[TOKEN_TECH_UV_U_SPEED])
				{
					// Property: emission_rate
					dynamicAttributeTranslator.translate(compiler, *i);
					DynamicAttribute* dynamicAttribute = any_cast<DynamicAttribute*>(child->context);
					mTechnique->setDynUVAnimation_U(dynamicAttribute);
				}
				else if (child->cls == token[TOKEN_TECH_UV_V_SPEED])
				{
					// Property: emission_rate
					dynamicAttributeTranslator.translate(compiler, *i);
					DynamicAttribute* dynamicAttribute = any_cast<DynamicAttribute*>(child->context);
					mTechnique->setDynUVAnimation_V(dynamicAttribute);
				}
				else if (child->cls == token[TOKEN_TECH_DISSIPATE_TIME])
				{
					// Property: emission_rate
					dynamicAttributeTranslator.translate(compiler, *i);
					DynamicAttribute* dynamicAttribute = any_cast<DynamicAttribute*>(child->context);
					mTechnique->setDynDissipateDelayTime(dynamicAttribute);
				}
				else if (child->cls == token[TOKEN_TECH_DISSIPATE_DELAY_TIME])
				{
					// Property: emission_rate
					dynamicAttributeTranslator.translate(compiler, *i);
					DynamicAttribute* dynamicAttribute = any_cast<DynamicAttribute*>(child->context);
					mTechnique->setDynDissipateDelayTime(dynamicAttribute);
				}
				else if (child->cls == token[TOKEN_TECH_DISSIPATE_VALUE])
				{
					// Property: emission_rate
					dynamicAttributeTranslator.translate(compiler, *i);
					DynamicAttribute* dynamicAttribute = any_cast<DynamicAttribute*>(child->context);
					mTechnique->setDynDissipateValue(dynamicAttribute);
				}
				else if (child->cls == token[TOKEN_TECH_DISSIPATE_SOFT_EDGE])
				{
					// Property: emission_rate
					dynamicAttributeTranslator.translate(compiler, *i);
					DynamicAttribute* dynamicAttribute = any_cast<DynamicAttribute*>(child->context);
					mTechnique->setDynDissipateDelayTime(dynamicAttribute);
				}
				else if (child->cls == token[TOKEN_CAMERA_DEPENDENCY])
				{
					// Property: camera_dependency
					CameraDependency* cameraDependency = PU_NEW_T(CameraDependency, MEMCATEGORY_SCRIPTING)();
					child->context = Any(cameraDependency);
					CameraDependencyTranslator cameraDependencyTranslator;
					cameraDependencyTranslator.translate(compiler, *i);
					Real threshold = cameraDependency->getThreshold();
					bool increase = cameraDependency->isIncrease();
					if (child->name == token[TOKEN_TECH_DEFAULT_PARTICLE_WIDTH])
					{
						mTechnique->setWidthCameraDependency(threshold * threshold, increase);
					}
					else if (child->name == token[TOKEN_TECH_DEFAULT_PARTICLE_HEIGHT])
					{
						mTechnique->setHeightCameraDependency(threshold * threshold, increase);
					}
					else if (child->name == token[TOKEN_TECH_DEFAULT_PARTICLE_DEPTH])
					{
						mTechnique->setDepthCameraDependency(threshold * threshold, increase);
					}
					// Delete the camera dependency
					PU_DELETE_T(cameraDependency, CameraDependency, MEMCATEGORY_SCRIPTING);
				}
				else
				{
					processNode(compiler, *i);
				}
			}
			else
			{
				errorUnexpectedToken(compiler, *i);
			}
		}
	}
	//-----------------------------------------------------------------------
	//-----------------------------------------------------------------------
	//-----------------------------------------------------------------------
	void ParticleTechniqueWriter::write(ParticleScriptSerializer* serializer, const IElement* element)
	{
		// Cast the element to a ParticleTechnique
		const ParticleTechnique* technique = static_cast<const ParticleTechnique*>(element);
		DynamicAttributeFactory dynamicAttributeFactory;

		// Write the header of the ParticleTechnique
		serializer->writeLine(token[TOKEN_TECHNIQUE], technique->getName(), 4, 1);
		serializer->writeLine("{", 4);

		// Write attributes
		if (technique->_getOriginalEnabled() != ParticleTechnique::DEFAULT_ENABLED) serializer->writeLine(
			token[TOKEN_ENABLED], StringConverter::toString(technique->_getOriginalEnabled()), 8);
		if (technique->position != ParticleTechnique::DEFAULT_POSITION) serializer->writeLine(
			token[TOKEN_POSITION], StringConverter::toString(technique->position), 8);
		if (technique->isKeepLocal() != ParticleTechnique::DEFAULT_KEEP_LOCAL) serializer->writeLine(
			token[TOKEN_KEEP_LOCAL], StringConverter::toString(technique->isKeepLocal()), 8);
		if (technique->getVisualParticleQuota() != ParticleTechnique::DEFAULT_VISUAL_PARTICLE_QUOTA) serializer->writeLine(
			token[TOKEN_TECH_VISUAL_PARTICLE_QUOTA], StringConverter::toString(technique->getVisualParticleQuota()), 8);
		if (technique->getEmittedEmitterQuota() != ParticleTechnique::DEFAULT_EMITTED_EMITTER_QUOTA) serializer->writeLine(
			token[TOKEN_TECH_EMITTED_EMITTER_QUOTA], StringConverter::toString(technique->getEmittedEmitterQuota()), 8);
		if (technique->getEmittedTechniqueQuota() != ParticleTechnique::DEFAULT_EMITTED_TECHNIQUE_QUOTA) serializer->writeLine(
			token[TOKEN_TECH_EMITTED_TECHNIQUE_QUOTA], StringConverter::toString(technique->getEmittedTechniqueQuota()), 8);
		if (technique->getEmittedAffectorQuota() != ParticleTechnique::DEFAULT_EMITTED_AFFECTOR_QUOTA) serializer->writeLine(
			token[TOKEN_TECH_EMITTED_AFFECTOR_QUOTA], StringConverter::toString(technique->getEmittedAffectorQuota()), 8);
		if (technique->getEmittedSystemQuota() != ParticleTechnique::DEFAULT_EMITTED_SYSTEM_QUOTA) serializer->writeLine(
			token[TOKEN_TECH_EMITTED_SYSTEM_QUOTA], StringConverter::toString(technique->getEmittedSystemQuota()), 8);
		
		//serializer->writeLine(token[TOKEN_MATERIAL], technique->getMaterialName(), 8);
		
		if (technique->getLodIndex() != ParticleTechnique::DEFAULT_LOD_INDEX) serializer->writeLine(
			token[TOKEN_TECH_LOD_INDEX], StringConverter::toString(technique->getLodIndex()), 8);
		if (technique->getDefaultWidth() != ParticleTechnique::DEFAULT_WIDTH) serializer->writeLine(
			token[TOKEN_TECH_DEFAULT_PARTICLE_WIDTH], StringConverter::toString(technique->getDefaultWidth()), 8);
		if (technique->getDefaultHeight() != ParticleTechnique::DEFAULT_HEIGHT) serializer->writeLine(
			token[TOKEN_TECH_DEFAULT_PARTICLE_HEIGHT], StringConverter::toString(technique->getDefaultHeight()), 8);
		if (technique->getDefaultDepth() != ParticleTechnique::DEFAULT_DEPTH) serializer->writeLine(
			token[TOKEN_TECH_DEFAULT_PARTICLE_DEPTH], StringConverter::toString(technique->getDefaultDepth()), 8);
		if (technique->getSpatialHashingCellDimension() != ParticleTechnique::DEFAULT_SPATIAL_HASHING_CELL_DIM) serializer->writeLine(
			token[TOKEN_TECH_SPHASHING_CELL_DIMENSION], StringConverter::toString(technique->getSpatialHashingCellDimension()), 8);
		if (technique->getSpatialHashingCellOverlap() != ParticleTechnique::DEFAULT_SPATIAL_HASHING_CELL_OVERLAP) serializer->writeLine(
			token[TOKEN_TECH_SPHASHING_CELL_OVERLAP], StringConverter::toString(technique->getSpatialHashingCellOverlap()), 8);
		if (technique->getSpatialHashTableSize() != ParticleTechnique::DEFAULT_SPATIAL_HASHING_TABLE_SIZE) serializer->writeLine(
			token[TOKEN_TECH_SPHASHING_SIZE], StringConverter::toString(technique->getSpatialHashTableSize()), 8);
		if (technique->getSpatialHashingInterval() != ParticleTechnique::DEFAULT_SPATIAL_HASHING_INTERVAL) serializer->writeLine(
			token[TOKEN_TECH_SPHASHING_UPDATE_INTERVAL], StringConverter::toString(technique->getSpatialHashingInterval()), 8);
		if (technique->getMaxVelocity() != ParticleTechnique::DEFAULT_MAX_VELOCITY) serializer->writeLine(
			token[TOKEN_TECH_MAX_VELOCITY], StringConverter::toString(technique->getMaxVelocity()), 8);
		
		if (technique->getDiffuseTextureName() != "") serializer->writeLine(
			token[TOKEN_TECH_DIFFUSE_TEXTURE], technique->getDiffuseTextureName(), 8);
		if (technique->getDepthCheckWrite() != false) serializer->writeLine(
			token[TOKEN_TECH_DEPTH_CHECK_WRITE], StringConverter::toString(technique->getDepthCheckWrite()), 8);
		if (technique->getDoubleRender() != false) serializer->writeLine(
			token[TOKEN_TECH_DOUBLE_RENDER], StringConverter::toString(technique->getDoubleRender()), 8);
		if (technique->getSourceFactor() != "add") serializer->writeLine(
			token[TOKEN_TECH_SCENE_BLEND], technique->getSourceFactor(), 8);
		if (technique->getSFAlphaValue() != 1.0) serializer->writeLine(
			token[TOKEN_TECH_ALPHA_VALUE], StringConverter::toString(technique->getSFAlphaValue()), 8);
		if (technique->getUVClamp() != Ogre::Vector3(1,1,0)) serializer->writeLine(
			token[TOKEN_TECH_UV_CLAMP], StringConverter::toString(technique->getUVClamp()), 8);
		if (technique->getRenderWithFog() != false) serializer->writeLine(
			token[TOKEN_TECH_FOG_OVERRIDE], StringConverter::toString(technique->getRenderWithFog()), 8);
		if (technique->getUVMaskTexture() != "") serializer->writeLine(
			token[TOKEN_TECH_UV_MASK_TEXTURE], technique->getUVMaskTexture(), 8);
		if (technique->getDissipateTexture() != "") serializer->writeLine(
			token[TOKEN_TECH_DISSIPATE_TEXTURE], technique->getDissipateTexture(), 8);

		if (technique->getDynColorScale()->getType() != DynamicAttribute::DAT_FIXED ||
			technique->getDynColorScale()->getValue() != 1)
		{
			serializer->setKeyword(token[TOKEN_TECH_COLOR_SCALE]);
			serializer->setIndentation(8);
			dynamicAttributeFactory.write(serializer, technique->getDynColorScale());
		}
		if (technique->getDynUVAnimation_U()->getType() != DynamicAttribute::DAT_FIXED ||
			technique->getDynUVAnimation_U()->getValue() != 0)
		{
			serializer->setKeyword(token[TOKEN_TECH_UV_U_SPEED]);
			serializer->setIndentation(8);
			dynamicAttributeFactory.write(serializer, technique->getDynUVAnimation_U());
		}
		if (technique->getDynUVAnimation_V()->getType() != DynamicAttribute::DAT_FIXED ||
			technique->getDynUVAnimation_V()->getValue() != 0)
		{
			serializer->setKeyword(token[TOKEN_TECH_UV_V_SPEED]);
			serializer->setIndentation(8);
			dynamicAttributeFactory.write(serializer, technique->getDynUVAnimation_V());
		}
		if (technique->getDynDissipateTime()->getType() != DynamicAttribute::DAT_FIXED ||
			technique->getDynDissipateTime()->getValue() != 0)
		{
			serializer->setKeyword(token[TOKEN_TECH_DISSIPATE_TIME]);
			serializer->setIndentation(8);
			dynamicAttributeFactory.write(serializer, technique->getDynDissipateTime());
		}
		if (technique->getDynDissipateDelayTime()->getType() != DynamicAttribute::DAT_FIXED ||
			technique->getDynDissipateDelayTime()->getValue() != 0)
		{
			serializer->setKeyword(token[TOKEN_TECH_DISSIPATE_DELAY_TIME]);
			serializer->setIndentation(8);
			dynamicAttributeFactory.write(serializer, technique->getDynDissipateDelayTime());
		}
		if (technique->getDynDissipateValue()->getType() != DynamicAttribute::DAT_FIXED ||
			technique->getDynDissipateValue()->getValue() != 1)
		{
			serializer->setKeyword(token[TOKEN_TECH_DISSIPATE_VALUE]);
			serializer->setIndentation(8);
			dynamicAttributeFactory.write(serializer, technique->getDynDissipateValue());
		}
		if (technique->getDynDissipateSoftEdgeValue()->getType() != DynamicAttribute::DAT_FIXED ||
			technique->getDynDissipateSoftEdgeValue()->getValue() != 0)
		{
			serializer->setKeyword(token[TOKEN_TECH_DISSIPATE_SOFT_EDGE]);
			serializer->setIndentation(8);
			dynamicAttributeFactory.write(serializer, technique->getDynDissipateSoftEdgeValue());
		}
		if (technique->getWidthCameraDependency())
		{
			serializer->writeLine(token[TOKEN_CAMERA_DEPENDENCY], token[TOKEN_TECH_DEFAULT_PARTICLE_WIDTH], 8);
			serializer->writeLine("{", 8);
			if (Math::Sqrt(technique->getWidthCameraDependency()->getThreshold()) != CameraDependency::DEFAULT_DISTANCE_THRESHOLD) serializer->writeLine(
				token[TOKEN_DISTANCE_THRESHOLD], StringConverter::toString(Math::Sqrt(technique->getWidthCameraDependency()->getThreshold())), 12);
			if (technique->getWidthCameraDependency()->isIncrease() != CameraDependency::DEFAULT_INCREASE) serializer->writeLine(
				token[TOKEN_INCREASE], StringConverter::toString(technique->getWidthCameraDependency()->isIncrease()), 12);
			serializer->writeLine("}", 8);
		}

		if (technique->getHeightCameraDependency())
		{
			serializer->writeLine(token[TOKEN_CAMERA_DEPENDENCY], token[TOKEN_TECH_DEFAULT_PARTICLE_HEIGHT], 8);
			serializer->writeLine("{", 8);
			if (Math::Sqrt(technique->getHeightCameraDependency()->getThreshold()) != CameraDependency::DEFAULT_DISTANCE_THRESHOLD) serializer->writeLine(
				token[TOKEN_DISTANCE_THRESHOLD], StringConverter::toString(Math::Sqrt(technique->getHeightCameraDependency()->getThreshold())), 12);
			if (technique->getHeightCameraDependency()->isIncrease() != CameraDependency::DEFAULT_INCREASE) serializer->writeLine(
				token[TOKEN_INCREASE], StringConverter::toString(technique->getHeightCameraDependency()->isIncrease()), 12);
			serializer->writeLine("}", 8);
		}

		if (technique->getDepthCameraDependency())
		{
			serializer->writeLine(token[TOKEN_CAMERA_DEPENDENCY], token[TOKEN_TECH_DEFAULT_PARTICLE_DEPTH], 8);
			serializer->writeLine("{", 8);
			if (Math::Sqrt(technique->getDepthCameraDependency()->getThreshold()) != CameraDependency::DEFAULT_DISTANCE_THRESHOLD) serializer->writeLine(
				token[TOKEN_DISTANCE_THRESHOLD], StringConverter::toString(Math::Sqrt(technique->getDepthCameraDependency()->getThreshold())), 12);
			if (technique->getDepthCameraDependency()->isIncrease() != CameraDependency::DEFAULT_INCREASE) serializer->writeLine(
				token[TOKEN_INCREASE], StringConverter::toString(technique->getDepthCameraDependency()->isIncrease()), 12);
			serializer->writeLine("}", 8);
		}

		// Write renderer
		serializer->context.beginSection(RENDERER, 0); // Don磘 set the renderer
		ParticleRenderer* renderer = technique->getRenderer();
		ParticleRendererFactory* particleRendererFactory = ParticleSystemManager::getSingletonPtr()->getRendererFactory(renderer->getRendererType());
		if (particleRendererFactory)
		{
			particleRendererFactory->write(serializer, renderer);
		}
		serializer->context.endSection();

		// Write emitters
		serializer->context.beginSection(EMITTER, 0); // Don磘 set the emitter
		size_t numberOfEmitters = technique->getNumEmitters();
		for (size_t i = 0; i < numberOfEmitters; ++i)
		{
			ParticleEmitter* emitter = technique->getEmitter(i);
			ParticleEmitterFactory* particleEmitterFactory = ParticleSystemManager::getSingletonPtr()->getEmitterFactory(emitter->getEmitterType());
			if (particleEmitterFactory)
			{
				particleEmitterFactory->write(serializer, emitter);
			}
		}
		serializer->context.endSection();

		// Write affectors
		serializer->context.beginSection(AFFECTOR, 0); // Don磘 set the affector
		size_t numberOfAffectors = technique->getNumAffectors();
		for (size_t i = 0; i < numberOfAffectors; ++i)
		{
			ParticleAffector* affector = technique->getAffector(i);
			ParticleAffectorFactory* particleAffectorFactory = ParticleSystemManager::getSingletonPtr()->getAffectorFactory(affector->getAffectorType());
			if (particleAffectorFactory)
			{
				particleAffectorFactory->write(serializer, affector);
			}
		}
		serializer->context.endSection();

		// Write observers
		serializer->context.beginSection(OBSERVER, 0); // Don磘 set the observer
		size_t numberOfObservers = technique->getNumObservers();
		for (size_t i = 0; i < numberOfObservers; ++i)
		{
			ParticleObserver* observer = technique->getObserver(i);
			ParticleObserverFactory* particleObserverFactory = ParticleSystemManager::getSingletonPtr()->getObserverFactory(observer->getObserverType());
			if (particleObserverFactory)
			{
				particleObserverFactory->write(serializer, observer);
			}
		}
		serializer->context.endSection();

		// Write behaviours
		serializer->context.beginSection(BEHAVIOUR, 0); // Don磘 set the behaviour
		size_t numberOfBehaviours = technique->_getNumBehaviourTemplates();
		for (size_t i = 0; i < numberOfBehaviours; ++i)
		{
			ParticleBehaviour* behaviour = technique->_getBehaviourTemplate(i);
			ParticleBehaviourFactory* particleBehaviourFactory = ParticleSystemManager::getSingletonPtr()->getBehaviourFactory(behaviour->getBehaviourType());
			if (particleBehaviourFactory)
			{
				particleBehaviourFactory->write(serializer, behaviour);
			}
		}
		serializer->context.endSection();

		// Write externs
		serializer->context.beginSection(EXTERN, 0); // Don磘 set the extern
		size_t numberOfExterns = technique->getNumExterns();
		for (size_t i = 0; i < numberOfExterns; ++i)
		{
			Extern* externObject = technique->getExtern(i);
			ExternFactory* externFactory = ParticleSystemManager::getSingletonPtr()->getExternFactory(externObject->getExternType());
			if (externFactory)
			{
				externFactory->write(serializer, externObject);
			}
		}
		serializer->context.endSection();

		// Write the close bracket
		serializer->writeLine("}", 4);
	}

}
