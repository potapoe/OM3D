#version 450

#include "utils.glsl"

// fragment shader for debugging normals

layout(location = 0) out vec4 out_color;

layout(binding = 11) uniform sampler2D g_normal;

void main()
{
    // Fetch Normal
    const vec3 normal = texelFetch(g_normal, ivec2(gl_FragCoord.xy), 0).rgb;

    out_color = vec4(normal, 1.0);
}
