#pragma once
#include <map>
#include <memory>
#include <string>
#include <vector>
#include <3dmath.h>


struct Mesh
{
  struct Bone
  {
    std::string name;
    mat4x4 bPose;
    mat4x4 bPoseInv;
    size_t id;
    size_t parentId;
  };

  const uint32_t vertexArrayBufferObject;
  const int numIndices;

  std::vector<Bone> bones;

  Mesh(uint32_t vertexArrayBufferObject, int numIndices) :
    vertexArrayBufferObject(vertexArrayBufferObject),
    numIndices(numIndices)
    {}
};

using MeshPtr = std::shared_ptr<Mesh>;

MeshPtr load_mesh(const char *path, int idx);
MeshPtr make_plane_mesh();
MeshPtr make_mesh(const std::vector<uint32_t> &indices, const std::vector<vec3> &vertices, const std::vector<vec3> &normals);

void render(const MeshPtr &mesh);