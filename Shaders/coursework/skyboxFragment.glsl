#version 330 core

uniform samplerCube cubeTex;
uniform sampler2D depthTex;

in Vertex{
	vec3 viewDir;
	vec3 position;
} IN;

out vec4 fragColour[3];

void main(void) {
	fragColour[2] = texture(cubeTex, normalize(IN.viewDir));
}