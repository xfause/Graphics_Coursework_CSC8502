#version 330

in vec3 fs_in_position;
in vec3 fs_camera_position;
in vec3 fs_in_color;

out vec4 fragColour[2];

void main(){

	vec3 tex1 = fs_in_color;
	if (tex1.r<0.1 && tex1.g<0.1 && tex1.b<0.1) discard;	

	fragColour[0] = vec4(tex1, 1);

}