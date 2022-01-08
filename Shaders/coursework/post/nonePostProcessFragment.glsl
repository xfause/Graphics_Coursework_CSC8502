#version 330 core

uniform sampler2D sceneTex;
uniform int reverse;

in Vertex {
	vec2 texCoord;
} IN;

out vec4 fragColor;

void main(void) {
if (reverse == 1) {
fragColor = texture2D(sceneTex, vec2(IN.texCoord.x, -IN.texCoord.y));
} else {
fragColor = texture2D(sceneTex, IN.texCoord.xy);
}
	
}