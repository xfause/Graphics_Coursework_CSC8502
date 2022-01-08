#version 330
uniform sampler2D treeTrunkTex;
uniform sampler2D bumpTex;
in vec2 g_texcoords;

in Vertex {
  vec3 normal;
  vec3 tangent;
  vec3 binormal;
}
IN;

out vec4 fragColour[2];

void main(){
	vec3 tex1 = texture(treeTrunkTex, g_texcoords).xyz;
	if (tex1.r<0.1 && tex1.g<0.1 && tex1.b<0.1) discard;	
	fragColour[0] = vec4(tex1, 1);


	mat3 TBN =  mat3(normalize(IN.tangent), normalize(IN.binormal), normalize(IN.normal));
	vec3 normal = texture2D(bumpTex, g_texcoords).rgb * 2.0 - 1.0;
	normal = normalize(TBN * normalize(normal));
	fragColour[1] = vec4(normal.xyz * 0.5 + 0.5, 1.0);
}