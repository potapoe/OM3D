#version 450

#include "utils.glsl"

// fragment shader for debugging albedo

layout(location = 0) out vec4 out_color;

layout(binding = 10) uniform sampler2D g_albedo;

void main()
{
    // Fetch Albedo
    const vec3 albedo = texelFetch(g_albedo, ivec2(gl_FragCoord.xy), 0).rgb;

    out_color = vec4(albedo, 1.0);
}
