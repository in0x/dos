#version 440 core

out vec4 color;

in VS_OUT_STRUCT
{
    vec4 color;
	vec3 normal;
} vs_in ;

out vec4 frag_colour;

void main()
{
  frag_colour = normalize(vec4(abs(vs_in.normal), 1));//normalize(vs_in.color);
}
