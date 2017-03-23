#version 440 core

layout (location = 0) in vec3 position;
layout (location = 1) in vec3 normal;

layout (location = 2) uniform vec3 cameraPos;
layout (location = 3) uniform mat4 modelTf;
layout (location = 4) uniform mat4 viewTf;
layout (location = 5) uniform mat4 projectionTf;

out VS_OUT_STRUCT // Data Type
{
    vec4 color;
	vec4 pos;
	vec3 normal;
} vs_out ; // Instance name

void main()
{
  vs_out.color = vec4(1,0,0,1);
  vs_out.normal = normal;

  vec4 pos = projectionTf * viewTf * modelTf * vec4(position,1);

  vs_out.pos = pos;
  gl_Position = projectionTf * viewTf * modelTf * vec4(position,1);
}
