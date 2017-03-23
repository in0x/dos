#version 440 core

//layout (location = 2) uniform vec3 cameraPos;
//layout (location = 3) uniform mat4 viewTf;

in VS_OUT_STRUCT
{
    vec4 color;
	vec4 pos;
	vec3 normal;
} vs_in ;

vec4 light_pos_world = vec4(-4, 4, 0, 1);

out vec4 frag_colour;

void main() 
{
	//vec4 light_pos_eye = viewTf * light_pos_world;
	//vec4 camera_pos_eye = viewTf * vec4(cameraPos, 1);

	frag_colour = normalize(vs_in.color);
}
