#version 330 core

uniform sampler2D sceneTex;
uniform sampler2D depthTex;

uniform int reverse;

in Vertex {
	vec2 texCoord;
} IN;

out vec4 fragColor;

uniform float weight[5] = float[] (0.2270270270, 0.1945945946, 0.1216216216, 0.0540540541, 0.0162162162);

void main(void) {
	vec2 tex_offset = 1.0 / textureSize(sceneTex, 0); // gets size of single texel

	vec2 texCoord = vec2(0.0, 0.0);
	if (reverse == 1) {
		texCoord = vec2(IN.texCoord.x, 1-IN.texCoord.y);
	} else {
		texCoord = IN.texCoord.xy;
	}

	fragColor = vec4(0,0,0,0);
	vec3 tmpColor = texture(sceneTex, IN.texCoord).rgb;

	float depthTex = texture(depthTex, texCoord).r;
	if (depthTex > 0.997 && depthTex< 0.999) {
//		fragColor += vec4(1.0, 0.0, 0.0, 0.3);
		tmpColor *= weight[0];
		if (reverse == 0) {
			for(int i = 1; i < 5; ++i)
			 {
				tmpColor += texture(sceneTex, IN.texCoord + vec2(tex_offset.x * i, 0.0)).rgb * weight[i];
				tmpColor += texture(sceneTex, IN.texCoord - vec2(tex_offset.x * i, 0.0)).rgb * weight[i];
			 }
		} else{
			for(int i = 1; i < 5; ++i)
			 {
				 tmpColor += texture(sceneTex, IN.texCoord + vec2(0.0, tex_offset.y * i)).rgb * weight[i];
				 tmpColor += texture(sceneTex, IN.texCoord - vec2(0.0, tex_offset.y * i)).rgb * weight[i];
			 }
		}
	}
	fragColor = vec4(tmpColor, 1.0);
}