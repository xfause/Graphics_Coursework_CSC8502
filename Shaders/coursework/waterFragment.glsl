#version  330  core

uniform  sampler2D  diffuseTex;
uniform  sampler2D  bumpTex;
uniform  samplerCube  cubeTex;

uniform  vec3    cameraPos;
uniform  vec3    lightPos;
uniform  vec4    lightColour;
uniform  float   lightRadius;

in  Vertex {
     vec4  colour;
     vec2  texCoord;
     vec3  normal;
     vec3  tangent;
     vec3  binormal;
     vec3  worldPos;
} IN;

out  vec4  fragColour[2];

void main(void) {

    vec4 diffuse = texture(diffuseTex, IN.texCoord);
    vec3 viewDir = normalize(cameraPos - IN.worldPos);

    vec3 reflectDir = reflect(viewDir, normalize(IN.normal));
    vec4 reflectTex = texture(cubeTex, reflectDir);

    mat3 TBN = mat3(normalize(IN.tangent), normalize(IN.binormal), normalize(IN.normal));
    vec3 bumpNormal = texture(bumpTex, IN.texCoord).rgb;
    bumpNormal = normalize(TBN * normalize(bumpNormal * 2.0 -1.0));

    vec3 bumpDir = reflect(-(lightPos - IN.worldPos), normalize(bumpNormal));
    vec4 bumpTex = texture(cubeTex, bumpDir);
    float bumpPerc = dot(normalize(reflect(-(lightPos - IN.worldPos), normalize(bumpNormal))),viewDir);

//   fragColour = reflectTex*1.2 + bumpTex * bumpPerc * lightColour*0.3 + (diffuse * 0.1f);

    fragColour[0] = diffuse  * 0.5 + reflectTex*0.4;
    fragColour[1] = vec4(bumpNormal.xyz * 0.5 + 0.5, 1.0);
}