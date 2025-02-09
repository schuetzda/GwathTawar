#version 450

layout(binding = 1) uniform sampler2D texSampler;

layout(location = 0) in vec3 fragColour;
layout(location = 1) in vec2 fragTexCoord;

layout(location = 0) out vec4 outColour; 	// Final output colour (must also have location

void main() {
	outColour = texture(texSampler, fragTexCoord);
}