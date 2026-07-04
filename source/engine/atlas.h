#ifndef WIS_ENGINE_ATLAS_H
#define WIS_ENGINE_ATLAS_H


#include "apeiron/opengl/meshset.h"


namespace wis {


class Atlas final
{
public:
  void init();
  [[nodiscard]] const apeiron::opengl::Meshset& meshes() const { return meshes_; }

private:
  apeiron::opengl::Meshset meshes_;
};


}  // namespace wis


#endif  // WIS_ENGINE_ATLAS_H
