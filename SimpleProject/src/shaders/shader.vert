#version 450 		// Use GLSL 4.5

layout(location = 0) in vec3 pos;
layout(location = 1) in vec3 normal;

layout(binding = 0) uniform UboViewProj {
	mat4 projection;
	mat4 view;
} uboViewProjection;

/* Not in use, left for reference
layout(binding = 1) uniform UboModel {
	mat4 model;
} uboModel;*/

layout(push_constant) uniform PushModel {
	mat4 model;
} pushModel;

layout(location = 0) out vec3 fragCol;

void main() {
	gl_Position = uboViewProjection.projection * uboViewProjection.view * pushModel.model * vec4(pos, 1.0);
	
	fragCol = normal;
}