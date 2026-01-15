
#include <text-complex-plus/access/brcvt.hpp>
#include <iostream>
#include <fstream>
#include <string>
#include <cstdlib>
#include <cstring>

static
int do_in(std::istream& in, std::ostream& out) {
  unsigned char inbuf[256];
  unsigned char outbuf[256];
  int ec = EXIT_SUCCESS;
  namespace tca = text_complex::access;
  tca::brcvt_state state(1, 16777200u, 1);
  bool done = false;
  while (ec == EXIT_SUCCESS && !done) {
    if (in.eof())
      break;
    else if (in.fail()) {
      ec = EXIT_FAILURE;
      break;
    }
    in.read(reinterpret_cast<char*>(inbuf), 256);
    std::streamsize const in_count = in.gcount();
    unsigned char const* src;
    for (src = inbuf; ec == EXIT_SUCCESS && src < inbuf+in_count; ) {
      unsigned char* dst;
      tca::api_error res = tca::brcvt_in
        (state, src, inbuf+in_count, src, outbuf, outbuf+256, dst);
      if (res < tca::api_error::Success) {
        ec = EXIT_FAILURE;
        std::cerr << "error code from conversion:\n\t"
          <<tca::api_error_toa(res) << std::endl;
      } else {
        out.write(reinterpret_cast<char*>(outbuf), dst-outbuf);
        if (!out) {
          ec = EXIT_FAILURE;
          std::cerr << "error from file write" << std::endl;
        } else if (res == tca::api_error::EndOfFile) {
          done = true;
          break;
        }
      }
    }
  }
  return ec;
}

static
int do_out(std::istream& in, std::ostream& out) {
  unsigned char inbuf[256];
  unsigned char outbuf[256];
  int ec = EXIT_SUCCESS;
  bool done = false;
  namespace tca = text_complex::access;
  tca::brcvt_state state(sizeof(inbuf), 32768, 1);
  while (ec == EXIT_SUCCESS) {
    if (in.eof())
      break;
    else if (in.fail()) {
      ec = EXIT_FAILURE;
      break;
    }
    in.read(reinterpret_cast<char*>(inbuf), 256);
    std::streamsize const in_count = in.gcount();
    for (unsigned char const* src = inbuf;
      ec == EXIT_SUCCESS && src<inbuf+in_count; )
    {
      unsigned char* dst;
      tca::api_error const res = tca::brcvt_out
        (state, src, inbuf+in_count, src,
        outbuf, outbuf+sizeof(outbuf), dst);
      if (res < tca::api_error::Success) {
        ec = EXIT_FAILURE;
        std::cerr << "error code from conversion:\n\t"
          << tca::api_error_toa(res) << std::endl;
      } else {
        out.write(reinterpret_cast<char*>(outbuf), dst-outbuf);
        if (out.bad()) {
          ec = EXIT_FAILURE;
          std::cerr << "error from file write" << std::endl;
        }
      }
    }
  }
  if (ec != EXIT_SUCCESS)
    return ec;
  while (ec == EXIT_SUCCESS && !done) {
    unsigned char* dst;
    tca::api_error res = tca::brcvt_unshift(state, outbuf,
      outbuf+sizeof(outbuf), dst);
    if (res < tca::api_error::Success) {
      ec = EXIT_FAILURE;
      std::cerr << "error code from conclusion:\n\t"
        << tca::api_error_toa(res) << std::endl;
    } else {
      out.write(reinterpret_cast<char*>(outbuf), dst-outbuf);
      if (out.bad()) {
        ec = EXIT_FAILURE;
        std::cerr << "error from file write" << std::endl;
      }
      done = (res == tca::api_error::EndOfFile);
    }
  }
  return ec;
}

int main(int argc, char**argv) {
  std::string ifilename;
  std::string ofilename;
  bool tozs = true;
  /* */{
    int i;
    for (i = 1; i < argc; ++i) {
      if (std::strcmp(argv[i], "-d") == 0) {
        tozs = false;
      } else if (ifilename.empty()) {
        ifilename = argv[i];
      } else if (ofilename.empty()) {
        ofilename = argv[i];
      }
    }
    if (ifilename.empty() || ofilename.empty()) {
      std::cerr << "usage: tcmplx_access_brcvt [-d] (input) (output)"
        << std::endl;
      return EXIT_FAILURE;
    }
  }
  std::ifstream ifile
    (ifilename.c_str(), std::ios_base::in | std::ios_base::binary);
  std::ofstream ofile
    (ofilename.c_str(), std::ios_base::out | std::ios_base::binary);
  if (ifile && ofile) {
    return tozs ? do_out(ifile, ofile) : do_in(ifile, ofile);
  } else {
    if (!ifile)
      std::cerr << "failed to open input file" << std::endl;
    if (!ofile)
      std::cerr << "failed to open output file" << std::endl;
    return EXIT_FAILURE;
  }
}
