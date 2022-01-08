#version 330 core

uniform sampler2D diffuseTex;

in Vertex	{
	vec4 colour;
	vec2 texCoord;
} IN;

//out vec4 gl_FragColor;
out vec4 fragColour[4];

void main(void)	{
	vec4 texColor = IN.colour* texture(diffuseTex, IN.texCoord);
	if(texColor.a < 0.1) {
			discard;
	}
	fragColour[3] = texColor;
}
