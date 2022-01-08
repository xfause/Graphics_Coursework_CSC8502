#version 330 core
out vec4 FragColor;

in Vertex {
	vec2 texCoord;
} IN;

uniform sampler2D scene;
uniform sampler2D bloomBlur;
uniform float exposure;
uniform int reverse;

void main()
{
    vec2 texCoord = vec2(0,0);
	
    if (reverse == 0) {
        texCoord = vec2(IN.texCoord.x, 1-IN.texCoord.y);
    } else {
        texCoord = IN.texCoord;
    }
    const float gamma = 2.2;
    vec3 hdrColor = texture(scene, texCoord).rgb;      
    vec3 bloomColor = texture(bloomBlur, texCoord).rgb;
    hdrColor += bloomColor; // additive blending
    // tone mapping
    vec3 result = vec3(1.0) - exp(-hdrColor * exposure);
    // also gamma correct while we're at it       
    result = pow(result, vec3(1.0 / gamma));
    FragColor = vec4(result, 1.0);
}