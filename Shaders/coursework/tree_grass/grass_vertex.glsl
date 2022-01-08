#version 330

layout(location = 0) in vec3 in_position;
layout(location = 1) in vec3 in_colors;
layout(location = 2) in vec2 in_parameters;

uniform mat4 modelMatrix;
uniform mat4 viewMatrix;
uniform mat4 projMatrix;
uniform vec3 cameraPos;

uniform float time;   
uniform vec3  wind;

out vec3 fs_in_color;
out vec3 fs_camera_position;
out vec3 fs_in_position;

vec3 computeOffset(vec3 vertexPosition) {

	float groundVertex = in_parameters.x;
	float randomMovement = in_parameters.y;

	return vec3((wind.x + randomMovement) * time * groundVertex,
				(wind.y + randomMovement) * time * groundVertex,
				(wind.z + randomMovement) * time * groundVertex
				);
}
void main() {
	vec3 out_position = in_position + computeOffset(in_position);
	fs_in_color = in_colors;
	fs_in_position  = out_position;
	fs_camera_position = cameraPos;

	gl_Position = projMatrix * viewMatrix * modelMatrix * vec4(out_position, 1);
}
