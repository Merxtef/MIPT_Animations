#version 330

struct VsOutput
{
  vec3 EyespaceNormal;
  vec3 WorldPosition;
  vec3 Color;
};

in VsOutput vsOutput;
out vec4 FragColor;

void main()
{
  FragColor = vec4(vsOutput.Color, 1.0);
}