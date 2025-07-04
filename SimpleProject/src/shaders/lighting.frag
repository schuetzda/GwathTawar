#version 450
layout (binding = 0) uniform sampler2D samplerColor;
layout (binding = 1) uniform sampler2D samplerPosition;
layout (binding = 2) uniform sampler2D samplerNormal;

struct Light {
	vec4 position;
	vec3 color;
	float radius;
};


layout(binding = 3) uniform LightInformation {
	Light lights[30];
	vec3 position;
}ubo;

layout (location = 0) in vec2 inUV;

layout (location = 0) out vec4 outColor;
void main() {
	vec3 fragColor = vec3(0.f, 0.f, 0.f);
	vec4 albedo = texture(samplerColor, inUV);
	vec3 fragPos = texture(samplerPosition, inUV).rgb;
	vec3 normal = texture(samplerNormal, inUV).rgb;
	
	for(int i = 0; i < 30; i++)
	{
		// Phong lighting model
		//https://rodolphe-vaillant.fr/entry/85/phong-illumination-model-cheat-sheet
		vec3 L = ubo.lights[i].position.xyz - fragPos;
		float dist = length(L);
			
		// Viewer to fragment
		vec3 V = ubo.position - fragPos;
		V = normalize(V);
		L = normalize(L);

		// Attenuation
		float atten = ubo.lights[i].radius / (pow(dist, 2.0) + 1.0);
			
		// Diffuse
		vec3 N = normalize(normal);
		float NdotL = max(0.0, dot(N, L));
		vec3 diff = ubo.lights[i].color * albedo.rgb * NdotL * atten;

		// Specular
		vec3 R = reflect(-L, N);
		float NdotR = max(0.0, dot(R, V));
		vec3 spec = ubo.lights[i].color * albedo.a * pow(NdotR, 16.0) * atten;

		fragColor += diff + spec;	
	}
	outColor = vec4(fragColor,1.f);
}