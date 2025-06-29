#version 450
layout (binding = 0) uniform sampler2D samplerColor;
layout (binding = 1) uniform sampler2D samplerposition;
layout (binding = 2) uniform sampler2D samplerNormal;

layout (location = 0) in vec2 inUV;

layout (location = 0) out vec4 outColor;
void main() {
	vec4 albedo = texture(samplerColor, inUV);
	vec3 fragPos = texture(samplerposition, inUV).rgb;
	vec3 normal = texture(samplerNormal, inUV).rgb;
	outColor = vec4(albedo,1.f);
}