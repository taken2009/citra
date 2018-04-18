// Copyright 2015 Citra Emulator Project
// Licensed under GPLv2 or any later version
// Refer to the license.txt file included.

#pragma once

#include <array>
#include <cstring>
#include <functional>
#include <string>
#include <type_traits>
#include "common/hash.h"
#include "video_core/regs.h"
#include "video_core/shader/shader.h"

namespace GLShader {

enum Attributes {
    ATTRIBUTE_POSITION,
    ATTRIBUTE_COLOR,
    ATTRIBUTE_TEXCOORD0,
    ATTRIBUTE_TEXCOORD1,
    ATTRIBUTE_TEXCOORD2,
    ATTRIBUTE_TEXCOORD0_W,
    ATTRIBUTE_NORMQUAT,
    ATTRIBUTE_VIEW,
};

// NOTE: MSVC15 (Update 2) doesn't think `delete`'d constructors and operators are TC.
//       This makes BitField not TC when used in a union or struct so we have to resort
//       to this ugly hack.
//       Once that bug is fixed we can use Pica::Regs::TevStageConfig here.
//       Doesn't include const_color because we don't sync it, see comment in BuildFromRegs()
struct TevStageConfigRaw {
    u32 sources_raw;
    u32 modifiers_raw;
    u32 ops_raw;
    u32 scales_raw;
    explicit operator Pica::TexturingRegs::TevStageConfig() const noexcept {
        Pica::TexturingRegs::TevStageConfig stage;
        stage.sources_raw = sources_raw;
        stage.modifiers_raw = modifiers_raw;
        stage.ops_raw = ops_raw;
        stage.const_color = 0;
        stage.scales_raw = scales_raw;
        return stage;
    }
};

struct PicaShaderConfigState {
    Pica::FramebufferRegs::CompareFunc alpha_test_func;
    Pica::RasterizerRegs::ScissorMode scissor_test_mode;
    Pica::TexturingRegs::TextureConfig::TextureType texture0_type;
    bool texture2_use_coord1;
    std::array<TevStageConfigRaw, 6> tev_stages;
    u8 combiner_buffer_input;

    Pica::RasterizerRegs::DepthBuffering depthmap_enable;
    Pica::TexturingRegs::FogMode fog_mode;
    bool fog_flip;

    struct {
        struct {
            unsigned num;
            bool directional;
            bool two_sided_diffuse;
            bool dist_atten_enable;
            bool spot_atten_enable;
            bool geometric_factor_0;
            bool geometric_factor_1;
            bool shadow_enable;
        } light[8];

        bool enable;
        unsigned src_num;
        Pica::LightingRegs::LightingBumpMode bump_mode;
        unsigned bump_selector;
        bool bump_renorm;
        bool clamp_highlights;

        Pica::LightingRegs::LightingConfig config;
            bool enable_primary_alpha;
            bool enable_secondary_alpha;

            bool enable_shadow;
            bool shadow_primary;
            bool shadow_secondary;
            bool shadow_invert;
            bool shadow_alpha;
            unsigned shadow_selector;

        struct {
            bool enable;
            bool abs_input;
            Pica::LightingRegs::LightingLutInput type;
            float scale;
        } lut_d0, lut_d1, lut_sp, lut_fr, lut_rr, lut_rg, lut_rb;
    } lighting;

    struct {
        bool enable;
        u32 coord;
        Pica::TexturingRegs::ProcTexClamp u_clamp, v_clamp;
        Pica::TexturingRegs::ProcTexCombiner color_combiner, alpha_combiner;
        bool separate_alpha;
        bool noise_enable;
        Pica::TexturingRegs::ProcTexShift u_shift, v_shift;
        u32 lut_width;
        u32 lut_offset;
        Pica::TexturingRegs::ProcTexFilter lut_filter;
    } proctex;
};

/**
 * This struct contains all state used to generate the GLSL shader program that emulates the current
 * Pica register configuration. This struct is used as a cache key for generated GLSL shader
 * programs. The functions in gl_shader_gen.cpp should retrieve state from this struct only, not by
 * directly accessing Pica registers. This should reduce the risk of bugs in shader generation where
 * Pica state is not being captured in the shader cache key, thereby resulting in (what should be)
 * two separate shaders sharing the same key.
 */
struct PicaShaderConfig : Common::HashableStruct<PicaShaderConfigState> {

    /// Construct a PicaShaderConfig with the given Pica register configuration.
    static PicaShaderConfig BuildFromRegs(const Pica::Regs& regs);

    bool TevStageUpdatesCombinerBufferColor(unsigned stage_index) const {
        return (stage_index < 4) && (state.combiner_buffer_input & (1 << stage_index));
    }

    bool TevStageUpdatesCombinerBufferAlpha(unsigned stage_index) const {
        return (stage_index < 4) && ((state.combiner_buffer_input >> 4) & (1 << stage_index));
    }
};

struct PicaShaderConfigCommon {
    void Init(const Pica::ShaderRegs& regs, Pica::Shader::ShaderSetup& setup);

    u64 program_hash;
    u64 swizzle_hash;
    u32 main_offset;
    bool sanitize_mul;

    u32 num_outputs;
    // reg to attribute
    std::array<u32, 16> output_map;
};

struct PicaVSConfig : Common::HashableStruct<PicaShaderConfigCommon> {
    explicit PicaVSConfig(const Pica::Regs& regs, Pica::Shader::ShaderSetup& setup) {
        state.Init(regs.vs, setup);
    }
};

struct PicaGSConfigCommonRaw {
    void Init(const Pica::Regs& regs);

    u32 vs_output_attributes;
    u32 gs_output_attributes;

    struct SemanticMap {
        u32 attribute_index;
        u32 component_index;
    };

    std::array<SemanticMap, 24> semantic_maps;
};

struct PicaGSConfigCommon : Common::HashableStruct<PicaGSConfigCommonRaw> {
    explicit PicaGSConfigCommon(const Pica::Regs& regs) {
        state.Init(regs);
    }
};

static_assert(std::is_trivial<PicaShaderConfigCommon>(), "!");
static_assert(std::is_trivial<PicaGSConfigCommonRaw>(), "!");

struct PicaGSConfigRaw : PicaShaderConfigCommon, PicaGSConfigCommonRaw {
    void Init(const Pica::Regs& regs, Pica::Shader::ShaderSetup& setup);

    u32 num_inputs;
    // reg to attribute
    std::array<u32, 16> input_map;

    u32 attributes_per_vertex;
};

struct PicaGSConfig : Common::HashableStruct<PicaGSConfigRaw> {
    explicit PicaGSConfig(const Pica::Regs& regs, Pica::Shader::ShaderSetup& setups) {
        state.Init(regs, setups);
    }
};

/**
 * Generates the GLSL default vertex shader program source code for the SW pipeline
 * @returns String of the shader source code
 */
std::string GenerateDefaultVertexShader(bool separable_shader);

/**
 * Generates the GLSL vertex shader program source code for the given VS program and its main offset
 * @returns String of the shader source code
 */
std::string GenerateVertexShader(const Pica::Shader::ShaderSetup& setup, const PicaVSConfig& config,
                                 bool separable_shader);

/**
 * Generates the GLSL default geometry shader program source code for the HW pipeline
 * @returns String of the shader source code
 */
std::string GenerateDefaultGeometryShader(const PicaGSConfigCommon& config, bool separable_shader);

/**
 * Generates the GLSL geometry shader program source code for the given GS program and its
 * configuration
 * @returns String of the shader source code
 */
std::string GenerateGeometryShader(const Pica::Shader::ShaderSetup& setup,
                                   const PicaGSConfig& config, bool separable_shader);

/**
 * Generates the GLSL fragment shader program source code for the current Pica state
 * @param config ShaderCacheKey object generated for the current Pica state, used for the shader
 *               configuration (NOTE: Use state in this struct only, not the Pica registers!)
 * @returns String of the shader source code
 */
std::string GenerateFragmentShader(const PicaShaderConfig& config, bool separable_shader);

} // namespace GLShader

namespace std {
template <>
struct hash<GLShader::PicaShaderConfig> {
    size_t operator()(const GLShader::PicaShaderConfig& k) const {
        return k.Hash();
    }
};

template <>
struct hash<GLShader::PicaVSConfig> {
    size_t operator()(const GLShader::PicaVSConfig& k) const {
        return k.Hash();
    }
};
template <>
struct hash<GLShader::PicaGSConfigCommon> {
    size_t operator()(const GLShader::PicaGSConfigCommon& k) const {
        return k.Hash();
    }
};

template <>
struct hash<GLShader::PicaGSConfig> {
    size_t operator()(const GLShader::PicaGSConfig& k) const {
        return k.Hash();
    }
};
} // namespace std
