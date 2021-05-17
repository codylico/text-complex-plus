
#include "text-complex-plus/access/api.hpp"
#include <iostream>

int main(int argc, char **argv) {
  using namespace text_complex;
  std::cout << "text-complex-plus version: "
    << access::api_version() << std::endl;
  /* error code "test" */ {
    int pre_v;
    for (pre_v = -11; pre_v <= 0; ++pre_v) {
      access::api_error v = static_cast<access::api_error>(pre_v);
      char const* v_text = api_error_toa(v);
      std::cout << "error value " << pre_v << ": "
        << (v_text ? v_text : "(nil)") << std::endl;
#if !(defined TextComplexAccessP_NO_EXCEPT)
      try {
        throw access::api_exception(v);
      } catch (std::exception const& e) {
        std::cout << "\tas except: " << e.what() << std::endl;
      }
#endif /*TextComplexAccessP_NO_EXCEPT*/
    }
  }
  return 0;
}
