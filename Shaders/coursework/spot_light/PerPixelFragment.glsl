#version  330  core
uniform  sampler2D  diffuseTex;
uniform  vec3    cameraPos;
uniform  vec4    lightColour;
uniform  vec3    lightPos;
uniform  vec3    lightDirection;
uniform  float   lightCutoff;
uniform  float   lightOutCutoff;
in  Vertex {
     vec3  colour;
     vec2  texCoord;
     vec3  normal;
     vec3  worldPos;
} IN;

out  vec4  fragColour;

void main() {
    vec3 toFrag = normalize(IN.worldPos - lightPos);
    float theta = dot(toFrag, normalize(lightDirection));

    float epsilon   = lightCutoff - lightOutCutoff;
    float intensity = clamp((theta - lightOutCutoff) / epsilon, 0.0, 1.0); 

    vec3 incident = normalize(lightPos - IN.worldPos);
    vec4 diffuse = texture(diffuseTex, IN.texCoord);
    vec3 viewDir = normalize(cameraPos - IN.worldPos);
    vec3 halfDir = normalize(incident +  viewDir);
    float distance = length(lightPos - IN.worldPos);

    float lambert = max(dot(incident, IN.normal), 0.0f);
    float specFactor = clamp(dot(halfDir, IN.normal), 0.0, 1.0);
    specFactor = pow(specFactor, 60.0);

    float attenuation = 1.0 - 1.0 / (1+ pow(distance, 2.0));

    vec3 surface = (diffuse.rgb * lightColour.rgb);
    fragColour.rgb = surface * lambert * intensity;
    fragColour.rgb +=(lightColour.rgb * specFactor)*attenuation * intensity;
    fragColour.rgb += surface * 0.1f;
    fragColour.a = diffuse.a;

}