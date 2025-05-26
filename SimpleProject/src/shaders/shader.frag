#version 450
#extension GL_EXT_nonuniform_qualifier : enable

layout(set = 1, binding = 1) uniform sampler2D texSampler[];

layout(location = 0) in vec3 fragColour;
layout(location = 1) in vec2 fragTexCoord;

layout(location = 0) out vec4 outColour; 	// Final output colour (must also have location

layout(push_constant) uniform PushModel {
    mat4 modelMatrix;
    uint textureIndex;
} pushConstants;

void main() {
	outColour = texture(texSampler[pushConstants.textureIndex], fragTexCoord);
}