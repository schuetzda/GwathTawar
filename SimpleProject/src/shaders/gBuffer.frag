#version 450
#extension GL_EXT_nonuniform_qualifier : enable

layout(set = 1, binding = 1) uniform sampler2D texSampler[];

layout(location = 0) in vec3 inColour;
layout(location = 1) in vec3 inPosition;
layout(location = 2) in vec3 inNormal;
layout(location = 3) in vec2 inTexCoord;

layout(location = 0) out vec4 outColour; 	// Final output colour (must also have location
layout (location = 1) out vec4 outPosition;
layout (location = 2) out vec4 outNormal;

layout(push_constant) uniform PushModel {
    mat4 modelMatrix;
    uint textureIndex;
} pushConstants;

void main() {
	outColour = texture(texSampler[pushConstants.textureIndex], inTexCoord);
    outPosition = vec4(inPosition, 1.0f);
    outNormal = vec4(inNormal, 1.0f);
}