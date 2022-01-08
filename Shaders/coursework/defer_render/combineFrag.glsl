#version 330 core

uniform sampler2D diffuseTex;
uniform sampler2D diffuseLight;
uniform sampler2D specularLight;
uniform sampler2D skyTex;
uniform sampler2D particleTex;

uniform sampler2D depthTex;


uniform vec2 pixelSize;

in Vertex {
	vec2 texCoord;
	vec3 position;
} IN;

out vec4 fragColour;

void main(void) {

	float depth = texture(depthTex, IN.texCoord).r;

	if (depth >= 1.0f){
		fragColour = texture(skyTex, IN.texCoord.xy);
	} else {
		vec3 diffuse = texture(diffuseTex, IN.texCoord).xyz;
		vec3 light = texture(diffuseLight, IN.texCoord).xyz;
		vec3 specular = texture(specularLight, IN.texCoord).xyz;

		fragColour.xyz = diffuse * 0.3;		// ambient
		fragColour.xyz += diffuse * light;	// lambert
		fragColour.xyz += specular;			// specular
		fragColour.a = 1.0;
	}
	
	fragColour.xyz += texture(particleTex, IN.texCoord).xyz;
}