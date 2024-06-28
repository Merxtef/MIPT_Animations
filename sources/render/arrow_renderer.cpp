#include "arrow_renderer.h"

#include <span>
#include <vector>
#include <render/material.h>
#include <render/mesh.h>
#include <render/shader.h>

#define SHADER_PATH "sources/shaders"

mat4 direction_mat(vec3 from, vec3 to)
{
  return toMat4(quat(normalize(from), normalize(to)));
}

struct ArrowMeshBuilder
{
  std::vector<uint> index;
  std::vector<vec3> vert;
  std::vector<vec3> norm;

  uint create_vert(vec3 v)
  {
    vert.push_back(v);
    return static_cast<uint>(vert.size() - 1);
  }

  void create_triangle(uint a, uint b, uint c)
  {
    index.push_back(a);
    index.push_back(b);
    index.push_back(c);

    vec3 a_ = vert.at(a);
    vec3 b_ = vert.at(b);
    vec3 c_ = vert.at(c);

    vec3 n = normalize(cross(b_ - a_, c_ - a_));
    norm.push_back(n);
    norm.push_back(n);
    norm.push_back(n);
  }

  MeshPtr build()
  {
    return make_mesh(index, vert, norm);
  }
};

struct BoneArrow
{
  MaterialPtr mat;
  MeshPtr mesh;
  std::vector<mat4> M;
  std::vector<vec4> color;

  BoneArrow()
  {
    mat = make_material("bone_arrow", SHADER_PATH"/bone_arrow_vs.glsl", SHADER_PATH"/bone_arrow_ps.glsl");
    ArrowMeshBuilder builder;

    uint point = builder.create_vert(vec3(0, 1, 0));
    uint sides[4] = {};
    
    for (int i = 0; i < 4; ++i)
    {
      float angle = (i / 4.f) * 2 * PI;
      sides[i] = builder.create_vert(vec3(cos(angle), 0, sin(angle)));
    }

    for (int i = 0; i < 4; ++i)
    {
      builder.create_triangle(sides[(i + 1) % 4], sides[i % 4], point);
    }

    builder.create_triangle(sides[0], sides[1], sides[2]);
    builder.create_triangle(sides[0], sides[2], sides[3]);
    mesh = builder.build();
  }

  void add_arrow(const vec3& from, const vec3& to, vec3 col, float size)
  {
    vec3 delta = to - from;
    float len = length(delta);
    
    M.emplace_back(
      translate(mat4(1.f), from) *
      direction_mat(vec3(0, 1, 0), delta) *
      scale(mat4(1.f), vec3(size, len, size))
    );
    color.emplace_back(vec4(col, 1.f));
  }

  void clear()
  {
    M.clear();
    color.clear();
  }
};

static std::unique_ptr<BoneArrow> instance;

void init_arrow()
{
  instance = std::make_unique<BoneArrow>();
}

void draw_arrow(const mat4 &transform, const vec3 &from, const vec3 &to, vec3 color, float size)
{
  instance->add_arrow(transform * vec4(from, 1), transform * vec4(to, 1), color, size);
}

void draw_arrow(const vec3 &from, const vec3 &to, vec3 color, float size)
{
  instance->add_arrow(from, to, color, size);
}

void render_arrows(const mat4 &cameraProjView, vec3 cameraPosition, const DirectionLight &light)
{
  const auto& shader = instance->mat->get_shader();

  glDepthFunc(GL_ALWAYS);
  glDepthMask(GL_FALSE);

  shader.use();

  shader.set_mat4x4("ViewProj", cameraProjView);

  size_t count = instance->M.size();
  for (int i = 0; i < count; ++i)
  {
    shader.set_mat4x4("M", instance->M.at(i));
    shader.set_vec4("Color", instance->color.at(i));
    render(instance->mesh);
  }

  glDepthFunc(GL_LESS);
  glDepthMask(GL_TRUE);

  instance->clear();
}
