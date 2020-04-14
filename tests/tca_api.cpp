
#include "../tcmplx-access-plus/api.hpp"
#include <iostream>

int main(int argc, char **argv) {
  using namespace text_complex;
  std::cout << "text-complex-plus version: "
    << access::api_version() << std::endl;
  return 0;
}
