#version 450

#include "utils.glsl"

// fragment shader of the Point Lights pass

layout(location = 0) out vec4 out_color;

layout(location = 0) in vec3 in_normal;
layout(location = 1) in vec2 in_uv;
layout(location = 2) in vec3 in_color;
layout(location = 3) in vec3 in_position;
layout(location = 4) in vec3 in_tangent;
layout(location = 5) in vec3 in_bitangent;

uniform vec3 current_light_position;
uniform float current_light_radius;
uniform vec3 current_light_color;

uniform uint screen_x;
uniform uint screen_y;

layout(binding = 10) uniform sampler2D g_albedo;
layout(binding = 11) uniform sampler2D g_normal;

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

    // Fetch Normal
    const vec3 normal = texelFetch(g_normal, ivec2(gl_FragCoord.xy), 0).rgb * 2.0 - 1.0;
    
    // Compute world position
    const vec2 screen_coords = vec2(gl_FragCoord.x / float(screen_x), gl_FragCoord.y / float(screen_y));
    const vec3 position = unproject(screen_coords, depth, inverse(frame.camera.view_proj));
    
    // Compute light
    const vec3 to_light = (current_light_position - position);
    const float dist = length(to_light);
    const vec3 light_vec = to_light / dist;

    const float NoL = dot(light_vec, normal);
    const float att = attenuation(dist, current_light_radius);
    
    vec3 acc = vec3(0.0, 0.0, 0.0);
    if(NoL > 0.0 && att > 0.0) {
        acc += current_light_color * (NoL * att);
    }

    out_color = vec4(albedo * acc, 1.0);
}
