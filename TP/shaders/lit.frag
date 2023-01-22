#version 450

#include "utils.glsl"

// fragment shader of the main lighting pass

// #define DEBUG_NORMAL

layout (location = 0) out vec4 g_albedo;
layout (location = 1) out vec4 g_normal;

layout(location = 0) in vec3 in_normal;
layout(location = 1) in vec2 in_uv;
layout(location = 2) in vec3 in_color;
layout(location = 3) in vec3 in_position;
layout(location = 4) in vec3 in_tangent;
layout(location = 5) in vec3 in_bitangent;

layout(binding = 0) uniform sampler2D in_texture;
layout(binding = 1) uniform sampler2D in_normal_texture;

uniform bool near_object_transparency;
uniform uint transparency_patch_size;
uniform bool gradual_object_transparency;

layout(binding = 0) uniform Data {
    FrameData frame;
};

const float depth_threshold_before_transparency = 0.0005f;
const float high_threshold = 0.0006f;
const int max_space = 10;
const int min_space = 2;

void main()
{
    // Compute Albedo/Depth
    vec4 color = vec4(in_color, 1.0);

#ifdef TEXTURED
    color *= texture(in_texture, in_uv);
#endif

#ifdef DEBUG_NORMAL
    color = vec4(normal * 0.5 + 0.5, 1.0);
#endif

    if (near_object_transparency && gl_FragCoord.z > depth_threshold_before_transparency)
    {
        int patch_space;
        if (gradual_object_transparency)
        {
            float fspace = (gl_FragCoord.z - high_threshold) * (max_space - min_space) / (depth_threshold_before_transparency-high_threshold) + min_space;
            patch_space = int(ceil(fspace));
            if (patch_space < min_space) patch_space = min_space;
        }
        else
        {
            patch_space = min_space;
        }
        ivec2 spos = ivec2(gl_FragCoord.xy) / int(transparency_patch_size);
        if (spos.x % patch_space == 0 && spos.y % patch_space == 0)
            discard;
        if (spos.x % patch_space == patch_space/2 && spos.y % patch_space == patch_space/2)
            discard;
    }

    // Compute normals
#ifdef NORMAL_MAPPED
    const vec3 normal_map = unpack_normal_map(texture(in_normal_texture, in_uv).xy);
    const vec3 normal = normal_map.x * in_tangent +
                        normal_map.y * in_bitangent +
                        normal_map.z * in_normal;
#else
    const vec3 normal = in_normal;
#endif

    // Store Albedo/Depth
    g_albedo = color;
    g_albedo.a = gl_FragCoord.z;

    // Store normals
    g_normal.xyz = (normalize(normal) + 1.0) / 2.0;

}