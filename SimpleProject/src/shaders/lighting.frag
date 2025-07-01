#version 450
layout (binding = 0) uniform sampler2D samplerColor;
layout (binding = 1) uniform sampler2D samplerposition;
layout (binding = 2) uniform sampler2D samplerNormal;

layout(binding = 3) uniform LightSource {
	vec4 position;
	vec3 color;
	float radius;
} lightSource;

layout (location = 0) in vec2 inUV;

layout (location = 0) out vec4 outColor;
void main() {
	vec3 fragcolor = vec3(0.f, 0.f, 0.f);
	vec4 albedo = texture(samplerColor, inUV);
	vec3 fragPos = texture(samplerposition, inUV).rgb;
	vec3 normal = texture(samplerNormal, inUV).rgb;

	vec3 L = lightSource.position.xyz - fragPos;
	// Distance from light to fragment position
	float dist = length(L);
		
	// Viewer to fragment
	vec3 V = vec3(0.f) - fragPos;
	V = normalize(V);
	L = normalize(L);

	// Attenuation
	float atten = lightSource.radius / (pow(dist, 2.0) + 1.0);
		
	// Diffuse part
	vec3 N = normalize(normal);
	float NdotL = max(0.0, dot(N, L));
	vec3 diff = lightSource.color * albedo.rgb * NdotL * atten;

	// Specular part
	// Specular map values are stored in alpha of albedo mrt
	vec3 R = reflect(-L, N);
	float NdotR = max(0.0, dot(R, V));
	vec3 spec = lightSource.color * albedo.a * pow(NdotR, 16.0) * atten;

	fragcolor += diff + spec;	
	outColor = vec4(fragcolor,1.f);
}