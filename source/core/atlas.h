#ifndef WIS_CORE_ATLAS_H
#define WIS_CORE_ATLAS_H


#include "apeiron/opengl/meshset.h"


namespace wis {


class Atlas final
{
public:
  void init();
  [[nodiscard]] const apeiron::opengl::Meshset& stage() const { return stage_; }
  [[nodiscard]] const apeiron::opengl::Meshset& ui() const { return ui_; }

private:
  apeiron::opengl::Meshset stage_;
  apeiron::opengl::Meshset ui_;
};


}  // namespace wis


#endif  // WIS_CORE_ATLAS_H
