#version 450

#include "utils.glsl"

// fragment shader for debugging depth

layout(location = 0) out vec4 out_color;

layout(binding = 10) uniform sampler2D g_albedo;

void main()
{
    // Fetch Depth
    const vec4 albedo_depth = texelFetch(g_albedo, ivec2(gl_FragCoord.xy), 0);
    const vec3 albedo = albedo_depth.rgb;
    const float depth = albedo_depth.a;

    out_color = vec4(vec3(depth * 1000), 1.0);
}