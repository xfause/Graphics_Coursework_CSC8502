#version 330 core

uniform sampler2D grassDiffuseTex;
uniform sampler2D grassBumpTex;
uniform sampler2D grassGlossTex;
uniform sampler2D sandDiffuseTex;
uniform sampler2D sandBumpTex;
uniform sampler2D sandGlossTex;

uniform float blendHeight;

in Vertex {
  vec3 position;
  vec4 colour;
  vec2 texCoord;
  vec3 normal;
  vec3 tangent;
  vec3 binormal;
  vec3 worldPos;
}
IN;

out vec4 fragColour[2];

void main(void) {
  mat3 TBN =
      mat3(normalize(IN.tangent), normalize(IN.binormal), normalize(IN.normal));

  vec4 diffuse = vec4(0);
  vec3 normal = vec3(0);

  float level1 = blendHeight * 0.6;
  float level2 = blendHeight * 0.5;

  if (IN.worldPos.y > level1) {
      diffuse = texture2D(grassDiffuseTex, IN.texCoord);
      normal = texture2D(grassBumpTex, IN.texCoord).rgb * 2.0 - 1.0;
      normal = normalize(TBN * normalize(normal));
  }
  else if (IN.worldPos.y > level2 && IN.worldPos.y < level1){
    float alpha = (level1 - IN.worldPos.y) / (level1-level2);
    diffuse = texture2D(grassDiffuseTex, IN.texCoord) * (1-alpha) + texture2D(sandDiffuseTex, IN.texCoord) * alpha;
    normal = texture2D(grassBumpTex, IN.texCoord).rgb * (1-alpha) + texture2D(sandBumpTex, IN.texCoord).rgb * alpha;
    normal = normal * 2.0 - 1.0;
    normal = normalize(TBN * normalize(normal));
  } else {
    diffuse = texture2D(sandDiffuseTex, IN.texCoord);
      normal = texture2D(sandBumpTex, IN.texCoord).rgb * 2.0 - 1.0;
      normal = normalize(TBN * normalize(normal));
  }
  fragColour[0] = diffuse;
  fragColour[1] = vec4(normal.xyz * 0.5 + 0.5, 1.0);

//  vec3 normal = texture2D(bumpTex, IN.texCoord).rgb * 2.0 - 1.0;
//  normal = normalize(TBN * normalize(normal));
//
//  vec4 diffuse = texture2D(diffuseTex, IN.texCoord);

//  fragColour[0] = diffuse;
//  fragColour[1] = vec4(normal.xyz * 0.5 + 0.5, 1.0);
}