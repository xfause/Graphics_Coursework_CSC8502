#version 330 core

uniform sampler2D sceneTex;

in Vertex {
	vec2 texCoord;
} IN;

out vec4 fragColor;

void main(void) {
	float gamma = 2.2;
	vec3 color = texture2D(sceneTex, IN.texCoord.xy).rgb;
    color = pow(color, vec3(1.0/gamma));
    fragColor = vec4(color, 1.0);
}