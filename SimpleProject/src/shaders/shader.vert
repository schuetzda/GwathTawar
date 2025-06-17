#version 450 		// Use GLSL 4.5

layout(location = 0) in vec3 pos;
layout(location = 1) in vec3 normal;
layout(location = 2) in vec2 texcoords;

layout(location = 0) out vec3 fragColour;
layout(location = 1) out vec2 fragTexCoord;

layout(binding = 0) uniform UboViewProj {
	mat4 viewProj;
} uboViewProjection;

/* Not in use, left for reference
layout(binding = 1) uniform UboModel {
	mat4 model;
} uboModel;*/

layout(push_constant) uniform PushModel {
	mat4 model;
	uint textureIndex;
} pushModel;

void main() {
	gl_Position = uboViewProjection.viewProj * pushModel.model * vec4(pos, 1.0);
	fragTexCoord = texcoords;
	fragColour = vec3(texcoords.x, texcoords.y, 0.f);
}