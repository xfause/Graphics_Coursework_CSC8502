#version 330 core
out vec4 fragColor[2];

in Vertex {
  vec3 position;
  vec4 colour;
  vec2 texCoord;
  vec3 normal;
  vec3 tangent;
  vec3 binormal;
  vec3 worldPos;
} IN;

uniform sampler2D TexWater;
uniform samplerCube skybox;
uniform vec3 cameraPos;

void main()
{
    vec4 FragColor = vec4(0, 0, 0, 0);
    vec4 tmp = texture(TexWater, IN.texCoord);
    FragColor = vec4(tmp.rgb, 0.9f);

    float ratio = 1.00 / 1.33;
    vec3 Normals = normalize(cross(dFdx(IN.worldPos), dFdy(IN.worldPos)));
    vec3 viewDir = normalize(IN.worldPos - cameraPos);

    vec3 refraction = refract(viewDir, normalize(-Normals), ratio);
    FragColor *= vec4(texture(skybox, refraction).rgb, 1.0);

    vec3 reflectDir = reflect(viewDir, normalize(-Normals));
    FragColor *= vec4(texture(skybox, reflectDir).rgb, 1.0);

    vec3 diffuse = vec3(-1.0, -1.0, -1.0);
    float attenuation =  dot(-normalize(cross(dFdx(IN.worldPos), dFdy(IN.worldPos))), diffuse);
    attenuation = max(attenuation, 0.0);

    vec3 hazy_ambiant = 0.4 * vec3(0.741, 0.745, 0.752);
    vec3 sunrise_ambiant = 0.4 * vec3(0.713, 0.494, 0.356);

    FragColor.xyz *= (hazy_ambiant + attenuation);
    FragColor.a = 0.9;
    fragColor[0] = FragColor;

    mat3 TBN =
      mat3(normalize(IN.tangent), normalize(IN.binormal), normalize(IN.normal));
    vec3 normal = vec3(0);
    normal = normalize(TBN * normalize(Normals));
    fragColor[1] = vec4(normal.xyz * 0.5 + 0.5, 1.0);
}