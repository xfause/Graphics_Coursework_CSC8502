#version 330 core

in   vec3  position;
in   vec2  texCoord;

out  Vertex {
	vec2  texCoord;
	vec3  position;
} OUT;

void  main(void)    {
	gl_Position      = vec4(position , 1.0);
	OUT.texCoord     = texCoord;
	OUT.position     = position;
}