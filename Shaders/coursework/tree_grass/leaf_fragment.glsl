#version 330
uniform sampler2D treeLeafTex;
uniform sampler2D textura1;
in vec2 g_texcoords;

out vec4 fragColour[2];

void main(){
	vec3 tex1;
	//if (tex == 0) {
		tex1 = texture(treeLeafTex, g_texcoords).xyz;
	//} else {
	//	tex1 = texture(textura1, g_texcoords).xyz;
	//}
	

	if (tex1.r<0.1 && tex1.g<0.1 && tex1.b<0.1) discard;	
	
	fragColour[0] = vec4(tex1, 1);
}