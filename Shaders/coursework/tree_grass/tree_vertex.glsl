#version 330

layout(location = 0) in vec3 in_position;
layout(location = 1) in vec3 in_normal;
layout(location = 2) in vec2 in_texcoords;
layout(location = 3) in vec3 in_start_circle_position;
layout(location = 4) in vec3 in_branch_details;
layout(location = 5) in vec2 in_branch_details2;

uniform mat4 modelMatrix;
uniform mat4 viewMatrix;
uniform mat4 projMatrix;
uniform vec3 cameraPos;

uniform float time;   
uniform float circlesPerBranch;
uniform vec3 wind;

out vec2 g_texcoords;

in vec3 normal;
in vec4 tangent;
in vec2 texCoord;

out Vertex {
  vec3 normal;
  vec3 tangent; 
  vec3 binormal;
} OUT;

vec3 computeOffset(vec3 vertexPosition) {
	
	float brLength = in_branch_details2.y;
	float brLevel = in_branch_details.z;
	int brCircle = int(in_branch_details.x);
	int isBranch = int(in_branch_details2.x);

	float iStepX = wind.x / circlesPerBranch;
	float iStepY = wind.y / circlesPerBranch;
	float iStepZ = wind.z / circlesPerBranch;

	float pvOffsetX = iStepX * (circlesPerBranch - 1) * time;
	float pvOffsetY = iStepY * (circlesPerBranch - 1) * time;
	float pvOffsetZ = iStepZ * (circlesPerBranch - 1) * time;

	if (isBranch == 1) {
		if (brLevel > 1) {
			pvOffsetX *= (brLevel - 1);
			pvOffsetY *= (brLevel - 1);
			pvOffsetZ *= (brLevel - 1);

			return vec3((iStepX * brCircle) * time + pvOffsetX,
						(iStepY * brCircle) * time + pvOffsetY,
						(iStepZ * brCircle) * time + pvOffsetZ
					);	
		} else {
			return vec3((iStepX * brCircle) * time,
						(iStepY * brCircle) * time,
						(iStepZ * brCircle) * time
					);	
		}
	}

	return vec3(0,0,0);
}

void main() {

	vec3 out_position = in_position + computeOffset(in_position);
    
	g_texcoords = in_texcoords.xy;

	mat3 normalMatrix = transpose(inverse(mat3(modelMatrix)));
	vec3 wNormal = normalize(normalMatrix * normalize(normal));
	vec3 wTangent = normalize(normalMatrix * normalize(tangent.xyz));
	
	OUT.normal = wNormal;
	OUT.tangent = wTangent;
	OUT.binormal = cross(wTangent, wNormal) * tangent.w;
		
	gl_Position = projMatrix * viewMatrix * modelMatrix * vec4(out_position, 1);
}
