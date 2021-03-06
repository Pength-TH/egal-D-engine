#ifndef _lua_shader_h_
#define _lua_shader_h_
#include "common/type.h"
#include "common/math/egal_math.h"

#include "lua_manager.h"

#include "common/resource/shader_manager.h"
#include "common/egal_string.h"

namespace egal
{
	static void registerCFunction(lua_State* L, const char* name, lua_CFunction function)
	{
		lua_pushcfunction(L, function);
		lua_setglobal(L, name);
	}

	static ShaderCombinations* getCombinations(lua_State* L)
	{
		ShaderCombinations* ret = nullptr;
		if (lua_getglobal(L, "this") == LUA_TLIGHTUSERDATA)
		{
			ret = lua_manager::toType<ShaderCombinations*>(L, -1);
		}
		lua_pop(L, 1);
		return ret;
	}


	static Shader* getShader(lua_State* L)
	{
		Shader* ret = nullptr;
		if (lua_getglobal(L, "shader") == LUA_TLIGHTUSERDATA)
		{
			ret = lua_manager::toType<Shader*>(L, -1);
		}
		lua_pop(L, 1);
		return ret;
	}

	static void texture_slot(lua_State* state, const char* name, const char* uniform)
	{
		auto* shader = getShader(state);
		if (!shader) return;
		auto& slot = shader->m_texture_slots[shader->m_texture_slot_count];
		StringUnitl::copyString(slot.name, name);
		slot.uniform_handle = bgfx::createUniform(uniform, bgfx::UniformType::Int1);
		StringUnitl::copyString(slot.uniform, uniform);
		++shader->m_texture_slot_count;
	}


	static Renderer* getRendererGlobal(lua_State* L)
	{
		Renderer* renderer = nullptr;
		if (lua_getglobal(L, "renderer") == LUA_TLIGHTUSERDATA)
		{
			renderer = lua_manager::toType<Renderer*>(L, -1);
		}
		lua_pop(L, 1);

		if (!renderer)
		{
			log_error("Renderer Error executing function texture_define, missing renderer global variable");
		}
		return renderer;
	}


	static void texture_define(lua_State* L, const char* define)
	{
		auto* shader = getShader(L);
		if (!shader) return;
		Renderer* renderer = getRendererGlobal(L);
		if (!renderer) return;

		auto& slot = shader->m_texture_slots[shader->m_texture_slot_count - 1];
		slot.define_idx = renderer->getShaderDefineIdx(lua_tostring(L, -1));
	}


	static void uniform(lua_State* L, const char* name, const char* type)
	{
		auto* shader = getShader(L);
		if (!shader) return;
		auto& u = shader->m_uniforms.emplace();
		StringUnitl::copyString(u.name, name);
		u.name_hash = crc32(name);
		if (StringUnitl::equalStrings(type, "float"))
		{
			u.type = Shader::Uniform::FLOAT;
			u.handle = bgfx::createUniform(name, bgfx::UniformType::Vec4);
		}
		else if (StringUnitl::equalStrings(type, "color"))
		{
			u.type = Shader::Uniform::COLOR;
			u.handle = bgfx::createUniform(name, bgfx::UniformType::Vec4);
		}
		else if (StringUnitl::equalStrings(type, "int"))
		{
			u.type = Shader::Uniform::INT32;
			u.handle = bgfx::createUniform(name, bgfx::UniformType::Int1);
		}
		else if (StringUnitl::equalStrings(type, "matrix4"))
		{
			u.type = Shader::Uniform::MATRIX4;
			u.handle = bgfx::createUniform(name, bgfx::UniformType::Mat4);
		}
		else if (StringUnitl::equalStrings(type, "time"))
		{
			u.type = Shader::Uniform::TIME;
			u.handle = bgfx::createUniform(name, bgfx::UniformType::Vec4);
		}
		else if (StringUnitl::equalStrings(type, "vec3"))
		{
			u.type = Shader::Uniform::FLOAT3;
			u.handle = bgfx::createUniform(name, bgfx::UniformType::Vec4);
		}
		else if (StringUnitl::equalStrings(type, "vec2"))
		{
			u.type = Shader::Uniform::FLOAT2;
			u.handle = bgfx::createUniform(name, bgfx::UniformType::Vec4);
		}
		else
		{
			log_error("Renderer Unknown uniform type %s in %s.", type, shader->getPath().c_str());
		}
	}

	static void pass(lua_State* state, const char* name)
	{
		auto* cmb = getCombinations(state);
		StringUnitl::copyString(cmb->passes[cmb->pass_count].data, name);
		cmb->vs_local_mask[cmb->pass_count] = 0;
		cmb->fs_local_mask[cmb->pass_count] = 0;
		++cmb->pass_count;
	}

	static int indexOf(ShaderCombinations& combination, byte define_idx)
	{
		for (int i = 0; i < combination.define_count; ++i)
		{
			if (combination.defines[i] == define_idx)
			{
				return i;
			}
		}

		combination.defines[combination.define_count] = define_idx;
		++combination.define_count;
		return combination.define_count - 1;
	}


	static void alpha_blending(lua_State* L, const char* mode)
	{
		Shader* shader = getShader(L);
		if (!shader) return;
		if (StringUnitl::equalStrings(mode, "add"))
		{
			shader->m_render_states |= BGFX_STATE_BLEND_ADD;
		}
		else if (StringUnitl::equalStrings(mode, "alpha"))
		{
			shader->m_render_states |= BGFX_STATE_BLEND_ALPHA;
		}
		else
		{
			log_error("Renderer Uknown blend mode %s in s%.", mode, shader->getPath().c_str());
		}
	}


	static void depth_test(lua_State* L, bool enabled)
	{
		Shader* shader = nullptr;
		if (lua_getglobal(L, "shader") == LUA_TLIGHTUSERDATA)
		{
			shader = lua_manager::toType<Shader*>(L, -1);
		}
		lua_pop(L, 1);
		if (!shader) return;
		if (enabled)
		{
			shader->m_render_states |= BGFX_STATE_DEPTH_TEST_LEQUAL;
		}
		else
		{
			shader->m_render_states &= ~BGFX_STATE_DEPTH_TEST_MASK;
		}
	}


	static void fs(lua_State* L)
	{
		auto* cmb = getCombinations(L);
		Renderer* renderer = getRendererGlobal(L);
		if (!renderer) return;

		lua_manager::checkTableArg(L, 1);
		int len = (int)lua_rawlen(L, 1);
		for (int i = 0; i < len; ++i)
		{
			if (lua_rawgeti(L, 1, i + 1) == LUA_TSTRING)
			{
				const char* tmp = lua_tostring(L, -1);
				int define_idx = renderer->getShaderDefineIdx(tmp);
				cmb->all_defines_mask |= 1 << define_idx;
				cmb->fs_local_mask[cmb->pass_count - 1] |= 1 << indexOf(*cmb, define_idx);
			}
			lua_pop(L, 1);
		}
	}


	static void vs(lua_State* L)
	{
		auto* cmb = getCombinations(L);
		Renderer* renderer = getRendererGlobal(L);
		if (!renderer) return;

		lua_manager::checkTableArg(L, 1);
		int len = (int)lua_rawlen(L, 1);
		for (int i = 0; i < len; ++i)
		{
			if (lua_rawgeti(L, 1, i + 1) == LUA_TSTRING)
			{
				const char* tmp = lua_tostring(L, -1);
				int define_idx = renderer->getShaderDefineIdx(tmp);
				cmb->all_defines_mask |= 1 << define_idx;
				cmb->vs_local_mask[cmb->pass_count - 1] |= 1 << indexOf(*cmb, define_idx);
			}
			lua_pop(L, 1);
		}
	}
	static void registerFunctions(Shader* shader, ShaderCombinations* combinations, Renderer* renderer, lua_State* L)
	{
		lua_pushlightuserdata(L, combinations);
		lua_setglobal(L, "this");
		lua_pushlightuserdata(L, renderer);
		lua_setglobal(L, "renderer");
		lua_pushlightuserdata(L, shader);
		lua_setglobal(L, "shader");

		registerCFunction(L, "pass",			&lua_manager::wrap<decltype(&pass), pass>);
		registerCFunction(L, "fs",				&lua_manager::wrap<decltype(&fs), fs>);
		registerCFunction(L, "vs",				&lua_manager::wrap<decltype(&vs), vs>);
		registerCFunction(L, "depth_test",		&lua_manager::wrap<decltype(&depth_test), depth_test>);
		registerCFunction(L, "alpha_blending",	&lua_manager::wrap<decltype(&alpha_blending), alpha_blending>);
		registerCFunction(L, "texture_slot",	&lua_manager::wrap<decltype(&texture_slot), texture_slot>);
		registerCFunction(L, "texture_define",	&lua_manager::wrap<decltype(&texture_define), texture_define>);
		registerCFunction(L, "uniform",			&lua_manager::wrap<decltype(&uniform), uniform>);
	}






}

#endif
