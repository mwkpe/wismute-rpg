#include <iostream>
#include <locale>


#include "app/app.h"
#include "app/error.h"


int main([[maybe_unused]] int argc, [[maybe_unused]] char* argv[])
{
  std::setlocale(LC_ALL, "en_US.UTF-8");

  wis::App app;

  try {
    app.init();
  }
  catch (const wis::Error& e) {
    std::cerr << e.what() << std::endl;
    return 1;
  }

  return app.exec();
}
