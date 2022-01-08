#version 330
uniform sampler2D diffuseTex;

in Vertex {
	vec2 texCoord;
} IN;

out vec4 fragColour[4];

void main(void){
	fragColour[0] = texture(diffuseTex, IN.texCoord);
}