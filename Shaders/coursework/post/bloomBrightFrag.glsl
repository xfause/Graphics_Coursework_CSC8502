#version 330 core

uniform sampler2D sceneTex;


in Vertex {
	vec2 texCoord;
} IN;

out vec4 fragColor;

void main(void) {
	fragColor = texture2D(sceneTex, IN.texCoord.xy);
    float brightness = dot(fragColor.rgb, vec3(0.2126, 0.7152, 0.0722));
    if(brightness > 0.3)
        fragColor = vec4(fragColor.rgb, 1.0);
	else
		fragColor = vec4(0.0, 0.0, 0.0, 1.0);
}