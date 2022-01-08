#version 330 core
layout (location = 0) in vec3 aPos;

uniform mat4 model;

uniform int type;
in vec4 jointWeights;
in ivec4 jointIndices;

uniform mat4 joints[128];

void main()
{
    if (type ==0) {
        gl_Position = model * vec4(aPos, 1.0);
    } else {
        vec4 skelPos = vec4(0,0,0,0);
        vec4 localPos = vec4(aPos, 1.0f);
    	for (int i=0;i<4;i++){
		    int jointIndex = jointIndices[i];
		    float jointWeight = jointWeights[i];

		    skelPos += joints[jointIndex] * localPos * jointWeight;
	    }
        gl_Position = model * vec4(skelPos.xyz, 1.0);
    }
}