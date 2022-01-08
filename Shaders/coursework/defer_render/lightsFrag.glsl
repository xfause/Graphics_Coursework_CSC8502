#version 330 core

uniform sampler2D depthTex;
uniform sampler2D normTex;

uniform samplerCube depthCubeMapTex;

uniform vec2 pixelSize; // reciprocal of resolution
uniform vec3 cameraPos;

uniform float lightRadius;
uniform vec3 lightPos;
uniform vec4 lightColour;

uniform mat4 inverseProjView;

uniform float far_plane;

//out vec4 diffuseOutput;
//out vec4 specularOutput;

// array of offset direction for sampling
vec3 gridSamplingDisk[20] = vec3[]
(
   vec3(50, 50,  50), vec3( 50, -50,  50), vec3(-50, -50,  50), vec3(-50, 50,  50), 
   vec3(50, 50, -50), vec3( 50, -50, -50), vec3(-50, -50, -50), vec3(-50, 50, -50),
   vec3(50, 50,  0), vec3( 50, -50,  0), vec3(-50, -50,  0), vec3(-50, 50,  0),
   vec3(50, 0,  50), vec3(-50,  0,  50), vec3( 50,  0, -50), vec3(-50, 0, -50),
   vec3(0, 50,  50), vec3( 0, -50,  50), vec3( 0, -50, -50), vec3( 0, 50, -50)
);

out vec4 fragColour[3];

float ShadowCalculation1(vec3 fragPos, vec3 normal, vec3 viewDir) {
    // get vector between fragment position and light position
    vec3 fragToLight = fragPos - lightPos;
    // use the light to fragment vector to sample from the depth map    
    float closestDepth = texture(depthCubeMapTex, fragToLight).r;
    // it is currently in linear range between [0,1]. Re-transform back to original value
    closestDepth *= far_plane;
    // now get current linear depth as the length between the fragment and light position
    float currentDepth = length(fragToLight);
    // now test for shadows
    float bias = 0.05; 
    vec4 pushVal = vec4(normal, 0) * dot(viewDir, normal);
    float shadow = currentDepth -  length(pushVal) + bias > closestDepth ? 1.0 : 0.0;

    return shadow;
    // return closestDepth / far_plane;
}

float ShadowCalculation2(vec3 fragPos, vec3 normal, vec3 viewDir)
{

	vec3 fragToLight = fragPos - lightPos;
	float currentDepth = length(fragToLight);
	float shadow = 0.0;
    float bias = 0.05;
    int samples = 20;
    float viewDistance = length(cameraPos - fragPos);
    float diskRadius = (1.0 + (viewDistance / far_plane)) / 25.0;
    for(int i = 0; i < samples; ++i)
    {
        float closestDepth = texture(depthCubeMapTex, fragToLight + gridSamplingDisk[i] * diskRadius).r;
        closestDepth *= far_plane;   // undo mapping [0;1]
        if(currentDepth - bias > closestDepth)
            shadow += 1.0;
    }
    shadow /= float(samples);
    return shadow;
}

void main(void) {
		vec2 texCoord = vec2(gl_FragCoord.xy * pixelSize);
		float depth = texture(depthTex, texCoord.xy).r;
		vec3 ndcPos = vec3(texCoord, depth) * 2.0 - 1.0;
		vec4 invClipPos = inverseProjView * vec4(ndcPos, 1.0);
		vec3 worldPos = invClipPos.xyz / invClipPos.w;

		float dist = length(lightPos - worldPos);
		float atten = 1.0 - clamp(dist / lightRadius, 0.0, 1.0);

		if (atten == 0.0) {
			discard;
		}

		vec3 normal = normalize(texture(normTex, texCoord.xy).xyz * 2.0 - 1.0);
		vec3 incident = normalize(lightPos - worldPos);
		vec3 viewDir = normalize(cameraPos - worldPos);
		vec3 halfDir = normalize(incident + viewDir);

		float lambert = clamp(dot(incident, normal), 0.0, 1.0);
		float rFactor = clamp(dot(halfDir, normal), 0.0, 1.0);
		float specFactor = clamp(dot(halfDir, normal), 0.0, 1.0);
		specFactor = pow(specFactor, 60.0);
		vec3 attenuated = lightColour.xyz * atten;
    
		// calculate shadow;
//		float shadow = ShadowCalculation1(worldPos, normal, viewDir);  
		float shadow = ShadowCalculation2(worldPos, normal, viewDir);  

		fragColour[0] = vec4(attenuated * lambert * (1.0-shadow), 1.0);
		fragColour[1] = vec4(attenuated * specFactor * 0.33, 1.0);
		 gl_FragDepth = 0.3;
    
}