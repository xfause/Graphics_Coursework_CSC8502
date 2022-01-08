#version 330
uniform sampler2D diffuseTex;

in Vertex {
	vec2 texCoord;
//	vec4 vertexColor;
} IN;

out vec4 gl_FragColor;

void main(void){
	gl_FragColor = texture(diffuseTex, IN.texCoord);
//	gl_FragColor = IN.vertexColor;

}