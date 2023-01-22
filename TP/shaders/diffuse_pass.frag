#version 450

#include "utils.glsl"

// fragment shader of the diffuse/ambient lighting pass

layout(location = 0) out vec4 out_color;

layout(location = 0) in vec2 in_uv;

layout(binding = 10) uniform sampler2D g_albedo;
layout(binding = 11) uniform sampler2D g_normal;

layout(binding = 0) uniform Data {
    FrameData frame;
};

const vec3 ambient = vec3(1.0);
const float diffuse_strength = 0.8;

void main()
{
    // Fetch Albedo/Depth
    const vec4 albedo_depth = texelFetch(g_albedo, ivec2(gl_FragCoord.xy), 0);
    const vec3 albedo = albedo_depth.rgb;
    const float depth = albedo_depth.a;

    // Fetch Normal
    const vec3 normal = texelFetch(g_normal, ivec2(gl_FragCoord.xy), 0).rgb * 2.0 - 1.0;

    // Compute Diffuse + ambient
    const vec3 diffuse = frame.sun_color * max(0.0, dot(frame.sun_dir, normal));

    const vec3 acc = diffuse * diffuse_strength + ambient * (1.0 - diffuse_strength);

    out_color = vec4(albedo * acc, 1.0);
}
