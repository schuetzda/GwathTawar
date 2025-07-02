#version 450 		// Use GLSL 4.5

layout(location = 0) in vec3 inPos;
layout(location = 1) in vec3 inNormal;
layout(location = 2) in vec2 inTexcoords;

layout(location = 0) out vec3 fragColour;
layout(location = 1) out vec3 fragPosition;
layout(location = 2) out vec3 fragNormal;
layout(location = 3) out vec2 fragTexCoord;

layout(binding = 0) uniform UboViewProj {
	mat4 viewProj;
} uboViewProjection;

layout(push_constant) uniform PushModel {
	mat4 model;
	uint textureIndex;
} pushModel;

void main() {
	gl_Position = uboViewProjection.viewProj * pushModel.model * vec4(inPos, 1.0);
	fragTexCoord = inTexcoords;
	fragColour = vec3(inTexcoords.x, inTexcoords.y, 0.f);

	mat3 mNormal = transpose(inverse(mat3(pushModel.model)));
	fragNormal = mNormal * normalize(inNormal);	
	fragPosition = mat3(pushModel.model)*inPos;
}