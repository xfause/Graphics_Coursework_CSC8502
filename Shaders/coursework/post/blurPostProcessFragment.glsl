#version 330 core

uniform sampler2D sceneTex;

uniform int isVertical;

in Vertex {
	vec2 texCoord;
} IN;

out vec4 fragColor;

const float blueScaleFactors[7] = 
	float [](0.006, 0.061, 0.242, 0.383, 0.242, 0.061, 0.006);

void main(void) {
	fragColor = vec4(0,0,0,1);
	vec2 delta = vec2(0,0);
	if (isVertical == 1){
		delta = dFdy(vec2(IN.texCoord.x, IN.texCoord.y));
	} else {
		delta = dFdx(IN.texCoord);
	}

	for (int i=0;i<7;i++){
		vec2 offset = delta * (i-3);
		vec4 tmp = texture2D(sceneTex, IN.texCoord.xy + offset);
		fragColor += tmp * blueScaleFactors[i];
	}
}