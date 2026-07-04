#ifndef WIS_APP_APP_H
#define WIS_APP_APP_H


#include "apeiron/opengl/window.h"
#include "app/app_data.h"
#include "app/settings.h"


namespace wis {


class App
{
public:
  App();
  ~App();
  void init();
  int exec();

private:
  App_data app_data_;
  Settings settings_;
  apeiron::opengl::Window window_;
};


}  // namespace wis


#endif  // WIS_APP_APP_H
