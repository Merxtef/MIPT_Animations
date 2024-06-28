#version 330

struct VsOutput
{
  vec3 EyespaceNormal;
  vec3 WorldPosition;
  vec3 Color;
};


uniform mat4 ViewProj;
uniform mat4 M;
uniform vec4 Color;


layout(location = 0)in vec3 Position;
layout(location = 1)in vec3 Normal;

out VsOutput vsOutput;

void main()
{
  vec4 worldPos = M * vec4(Position, 1);
  vsOutput.WorldPosition = worldPos.xyz;
  gl_Position = ViewProj * worldPos;
  mat3 ModelNorm = mat3(M);
  ModelNorm[0] = normalize(ModelNorm[0]);
  ModelNorm[1] = normalize(ModelNorm[1]);
  ModelNorm[2] = normalize(ModelNorm[2]);
  vsOutput.EyespaceNormal = normalize(ModelNorm * Normal);

  vsOutput.Color = Color.rgb;
}