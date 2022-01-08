#version 330 core

uniform mat4 modelMatrix;
uniform mat4 viewMatrix;
uniform mat4 projMatrix;

in vec3 position;

out Vertex{
	vec3 viewDir;
	vec3 position;
} OUT;

void main(void) {
	vec3 pos = position;
	mat4 invproj = inverse(projMatrix);
	pos.xy *= vec2(invproj[0][0], invproj[1][1]);
	pos.z = -1.0f;
	OUT.viewDir = transpose(mat3(viewMatrix)) * normalize(pos);
	gl_Position = vec4(position.x, position.y, position.z, 1.0);

	OUT.position = pos;
}