#version 450

#include "utils.glsl"

// fragment shader for debugging unproject

layout(location = 0) out vec4 out_color;

layout(location = 0) in vec2 in_uv;

layout(binding = 10) uniform sampler2D g_albedo;

layout(binding = 0) uniform Data {
    FrameData frame;
};

vec3 unproject(vec2 uv, float depth, mat4 inv_viewproj)
{
    const vec3 ndc = vec3(uv * 2.0 - vec2(1.0), depth);
    const vec4 p = inv_viewproj * vec4(ndc, 1.0);
    return p.xyz / p.w;
}

void main()
{
    // Fetch Albedo/Depth
    const vec4 albedo_depth = texelFetch(g_albedo, ivec2(gl_FragCoord.xy), 0);
    const vec3 albedo = albedo_depth.rgb;
    const float depth = albedo_depth.a;

    // Compute world position
    vec3 position = unproject(in_uv, depth, inverse(frame.camera.view_proj));

    position = ceil(position);

    const ivec3 p = abs(ivec3(position)) % 10;
  
    out_color = vec4(p / 10.0, 1.0);
}
