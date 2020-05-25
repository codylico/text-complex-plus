/**
 * @file tcmplx-access-plus/woff2.cpp
 * @brief WOFF2 file utility API
 * @author Cody Licorish (svgmovement@gmail.com)
 */
#define TCMPLX_AP_WIN32_DLL_INTERNAL
#include "woff2.hpp"
#include <new>
#include <stdexcept>
#include <cstring>
#include "../mmaptwo-plus/mmaptwo.hpp"
#include "seq.hpp"
#include <memory>
#include "offtable.hpp"
#include <limits>
#include <algorithm>

namespace text_complex {
  namespace access {
    static struct {
      unsigned char tag[4];
    } woff2_tag_list[64] = {
      {/* 0: cmap*/{0x63,0x6d,0x61,0x70}},{/* 1: head*/{0x68,0x65,0x61,0x64}},
      {/* 2: hhea*/{0x68,0x68,0x65,0x61}},{/* 3: hmtx*/{0x68,0x6d,0x74,0x78}},
      {/* 4: maxp*/{0x6d,0x61,0x78,0x70}},{/* 5: name*/{0x6e,0x61,0x6d,0x65}},
      {/* 6: OS/2*/{0x4f,0x53,0x2f,0x32}},{/* 7: post*/{0x70,0x6f,0x73,0x74}},
      {/* 8: cvt */{0x63,0x76,0x74,0x20}},{/* 9: fpgm*/{0x66,0x70,0x67,0x6d}},
      {/*10: glyf*/{0x67,0x6c,0x79,0x66}},{/*11: loca*/{0x6c,0x6f,0x63,0x61}},
      {/*12: prep*/{0x70,0x72,0x65,0x70}},{/*13: CFF */{0x43,0x46,0x46,0x20}},
      {/*14: VORG*/{0x56,0x4f,0x52,0x47}},{/*15: EBDT*/{0x45,0x42,0x44,0x54}},
      {/*16: EBLC*/{0x45,0x42,0x4c,0x43}},{/*17: gasp*/{0x67,0x61,0x73,0x70}},
      {/*18: hdmx*/{0x68,0x64,0x6d,0x78}},{/*19: kern*/{0x6b,0x65,0x72,0x6e}},
      {/*20: LTSH*/{0x4c,0x54,0x53,0x48}},{/*21: PCLT*/{0x50,0x43,0x4c,0x54}},
      {/*22: VDMX*/{0x56,0x44,0x4d,0x58}},{/*23: vhea*/{0x76,0x68,0x65,0x61}},
      {/*24: vmtx*/{0x76,0x6d,0x74,0x78}},{/*25: BASE*/{0x42,0x41,0x53,0x45}},
      {/*26: GDEF*/{0x47,0x44,0x45,0x46}},{/*27: GPOS*/{0x47,0x50,0x4f,0x53}},
      {/*28: GSUB*/{0x47,0x53,0x55,0x42}},{/*29: EBSC*/{0x45,0x42,0x53,0x43}},
      {/*30: JSTF*/{0x4a,0x53,0x54,0x46}},{/*31: MATH*/{0x4d,0x41,0x54,0x48}},
      {/*32: CBDT*/{0x43,0x42,0x44,0x54}},{/*33: CBLC*/{0x43,0x42,0x4c,0x43}},
      {/*34: COLR*/{0x43,0x4f,0x4c,0x52}},{/*35: CPAL*/{0x43,0x50,0x41,0x4c}},
      {/*36: SVG */{0x53,0x56,0x47,0x20}},{/*37: sbix*/{0x73,0x62,0x69,0x78}},
      {/*38: acnt*/{0x61,0x63,0x6e,0x74}},{/*39: avar*/{0x61,0x76,0x61,0x72}},
      {/*40: bdat*/{0x62,0x64,0x61,0x74}},{/*41: bloc*/{0x62,0x6c,0x6f,0x63}},
      {/*42: bsln*/{0x62,0x73,0x6c,0x6e}},{/*43: cvar*/{0x63,0x76,0x61,0x72}},
      {/*44: fdsc*/{0x66,0x64,0x73,0x63}},{/*45: feat*/{0x66,0x65,0x61,0x74}},
      {/*46: fmtx*/{0x66,0x6d,0x74,0x78}},{/*47: fvar*/{0x66,0x76,0x61,0x72}},
      {/*48: gvar*/{0x67,0x76,0x61,0x72}},{/*49: hsty*/{0x68,0x73,0x74,0x79}},
      {/*50: just*/{0x6a,0x75,0x73,0x74}},{/*51: lcar*/{0x6c,0x63,0x61,0x72}},
      {/*52: mort*/{0x6d,0x6f,0x72,0x74}},{/*53: morx*/{0x6d,0x6f,0x72,0x78}},
      {/*54: opbd*/{0x6f,0x70,0x62,0x64}},{/*55: prop*/{0x70,0x72,0x6f,0x70}},
      {/*56: trak*/{0x74,0x72,0x61,0x6b}},{/*57: Zapf*/{0x5a,0x61,0x70,0x66}},
      {/*58: Silf*/{0x53,0x69,0x6c,0x66}},{/*59: Glat*/{0x47,0x6c,0x61,0x74}},
      {/*60: Gloc*/{0x47,0x6c,0x6f,0x63}},{/*61: Feat*/{0x46,0x65,0x61,0x74}},
      {/*62: Sill*/{0x53,0x69,0x6c,0x6c}},{/*63*/      {0}},
    };

    struct woff2_tag {
      unsigned int n;
      unsigned char tag[5];
    };
    static struct woff2_tag woff2_tag_table[64] = {
      {25,/*BASE*/{0x42,0x41,0x53,0x45}}, {32,/*CBDT*/{0x43,0x42,0x44,0x54}},
      {33,/*CBLC*/{0x43,0x42,0x4c,0x43}}, {13,/*CFF */{0x43,0x46,0x46,0x20}},
      {34,/*COLR*/{0x43,0x4f,0x4c,0x52}}, {35,/*CPAL*/{0x43,0x50,0x41,0x4c}},
      {15,/*EBDT*/{0x45,0x42,0x44,0x54}}, {16,/*EBLC*/{0x45,0x42,0x4c,0x43}},
      {29,/*EBSC*/{0x45,0x42,0x53,0x43}}, {61,/*Feat*/{0x46,0x65,0x61,0x74}},
      {26,/*GDEF*/{0x47,0x44,0x45,0x46}}, {27,/*GPOS*/{0x47,0x50,0x4f,0x53}},
      {28,/*GSUB*/{0x47,0x53,0x55,0x42}}, {59,/*Glat*/{0x47,0x6c,0x61,0x74}},
      {60,/*Gloc*/{0x47,0x6c,0x6f,0x63}}, {30,/*JSTF*/{0x4a,0x53,0x54,0x46}},
      {20,/*LTSH*/{0x4c,0x54,0x53,0x48}}, {31,/*MATH*/{0x4d,0x41,0x54,0x48}},
      { 6,/*OS/2*/{0x4f,0x53,0x2f,0x32}}, {21,/*PCLT*/{0x50,0x43,0x4c,0x54}},
      {36,/*SVG */{0x53,0x56,0x47,0x20}}, {58,/*Silf*/{0x53,0x69,0x6c,0x66}},
      {62,/*Sill*/{0x53,0x69,0x6c,0x6c}}, {22,/*VDMX*/{0x56,0x44,0x4d,0x58}},
      {14,/*VORG*/{0x56,0x4f,0x52,0x47}}, {57,/*Zapf*/{0x5a,0x61,0x70,0x66}},
      {38,/*acnt*/{0x61,0x63,0x6e,0x74}}, {39,/*avar*/{0x61,0x76,0x61,0x72}},
      {40,/*bdat*/{0x62,0x64,0x61,0x74}}, {41,/*bloc*/{0x62,0x6c,0x6f,0x63}},
      {42,/*bsln*/{0x62,0x73,0x6c,0x6e}}, { 0,/*cmap*/{0x63,0x6d,0x61,0x70}},
      {43,/*cvar*/{0x63,0x76,0x61,0x72}}, { 8,/*cvt */{0x63,0x76,0x74,0x20}},
      {44,/*fdsc*/{0x66,0x64,0x73,0x63}}, {45,/*feat*/{0x66,0x65,0x61,0x74}},
      {46,/*fmtx*/{0x66,0x6d,0x74,0x78}}, { 9,/*fpgm*/{0x66,0x70,0x67,0x6d}},
      {47,/*fvar*/{0x66,0x76,0x61,0x72}}, {17,/*gasp*/{0x67,0x61,0x73,0x70}},
      {10,/*glyf*/{0x67,0x6c,0x79,0x66}}, {48,/*gvar*/{0x67,0x76,0x61,0x72}},
      {18,/*hdmx*/{0x68,0x64,0x6d,0x78}}, { 1,/*head*/{0x68,0x65,0x61,0x64}},
      { 2,/*hhea*/{0x68,0x68,0x65,0x61}}, { 3,/*hmtx*/{0x68,0x6d,0x74,0x78}},
      {49,/*hsty*/{0x68,0x73,0x74,0x79}}, {50,/*just*/{0x6a,0x75,0x73,0x74}},
      {19,/*kern*/{0x6b,0x65,0x72,0x6e}}, {51,/*lcar*/{0x6c,0x63,0x61,0x72}},
      {11,/*loca*/{0x6c,0x6f,0x63,0x61}}, { 4,/*maxp*/{0x6d,0x61,0x78,0x70}},
      {52,/*mort*/{0x6d,0x6f,0x72,0x74}}, {53,/*morx*/{0x6d,0x6f,0x72,0x78}},
      { 5,/*name*/{0x6e,0x61,0x6d,0x65}}, {54,/*opbd*/{0x6f,0x70,0x62,0x64}},
      { 7,/*post*/{0x70,0x6f,0x73,0x74}}, {12,/*prep*/{0x70,0x72,0x65,0x70}},
      {55,/*prop*/{0x70,0x72,0x6f,0x70}}, {37,/*sbix*/{0x73,0x62,0x69,0x78}},
      {56,/*trak*/{0x74,0x72,0x61,0x6b}}, {23,/*vhea*/{0x76,0x68,0x65,0x61}},
      {24,/*vmtx*/{0x76,0x6d,0x74,0x78}}, {63,{0}},
    };

    /**
     * @brief Parse out a 16-bit unsigned integer.
     * @param s from here
     * @return the integer
     */
    static
    unsigned short woff2_read_u16be(void const* s);
    /**
     * @brief Parse out a tag from a WOFF2 tag table.
     * @param seq for this sequential
     * @param tag_text to this tag buffer
     * @param enc_path encode path taken for this chunk
     * @throw api_exception on sanitize check failure
     */
    static
    void woff2_read_tag
        (sequential* seq, unsigned char *tag_text, unsigned int &enc_path);
    /**
     * @brief Parse out a variable-length-encoding 32-bit unsigned integer.
     * @param s from here
     * @param[out] v to here
     * @throw api_exception on sanitize check failure
     */
    static
    void woff2_read_uv128(sequential* seq, uint32& v);

    /**
     * @brief Check if a WOFF2 table/encoding combination requires
     *   a transform length in the table directory.
     * @param tag_text tag name
     * @param enc_path encoding path
     * @return true if a transform length should be present
     */
    static
    bool woff2_req_twolen
      (unsigned char const* tag_text, unsigned int enc_path);
    /**
     * @brief Compare internal WOFF2 tags.
     * @param v left tag
     * @param w right tag
     * @return true if `v < w`
     */
    static
    bool woff2_tag_cmp(struct woff2_tag const& v, struct woff2_tag const& w);

    //BEGIN woff2 static
    bool woff2_tag_cmp(struct woff2_tag const& v, struct woff2_tag const& w) {
      return std::memcmp(v.tag, w.tag, 4)<0;
    }

    unsigned short woff2_read_u16be(void const* s) {
      unsigned char const* c = static_cast<unsigned char const*>(s);
      return (static_cast<unsigned short>(c[0])<<8)|c[1];
    }

    void woff2_read_tag
        (sequential* seq, unsigned char *tag_text, unsigned int &enc_path)
    {
      int const q_tag = seq->get_byte();
      if (q_tag < 0)
        throw api_exception(api_error::ErrSanitize);
      if ((q_tag&63) == 63) {
        int i;
        for (i = 0; i < 4; ++i) {
          int const ch = seq->get_byte();
          if (ch < 0)
            throw api_exception(api_error::ErrSanitize);
          else tag_text[i] = (unsigned char)(ch&255);
        }
      } else {
        std::memcpy(tag_text, woff2_tag_fromi(q_tag&63), 4);
      }
      enc_path = (q_tag>>6)&3;
      return;
    }

    void woff2_read_uv128(sequential* seq, uint32& v) {
      uint32 out = 0u;
      int i;
      for (i = 0; i < 5; ++i) {
        int const ch = seq->get_byte();
        if (ch < 0)
          throw api_exception(api_error::ErrSanitize);
        else if (out == 0u && ch == 0x80)
          throw api_exception(api_error::ErrSanitize);
        else if (i == 4 && ((ch&0x80) || (out > 0x1ffFFff))) {
          throw api_exception(api_error::ErrSanitize);
        } else {
          out = ((out<<7)|static_cast<uint32>(ch&0x7f));
          if (!(ch&0x80))
            break;
        }
      }
      v = out;
      return;
    }

    bool woff2_req_twolen
        (unsigned char const* tag_text, unsigned int enc_path)
    {
      constexpr unsigned char glyf[4] = {0x67,0x6c,0x79,0x66};
      constexpr unsigned char loca[4] = {0x6c,0x6f,0x63,0x61};
      constexpr unsigned char hmtx[4] = {0x68,0x6d,0x74,0x78};
      if (enc_path == 0
      &&  (  (memcmp(tag_text, glyf, 4) == 0)
          || (memcmp(tag_text, loca, 4) == 0)))
      {
        return true;
      }	else if (enc_path == 1
      &&  memcmp(tag_text, hmtx, 4) == 0)
      {
        return true;
      } else return false;
    }
    //END   woff2 static

    //BEGIN woff2 tools
    unsigned int woff2_tag_toi(unsigned char const* s) {
      unsigned int i;
      struct woff2_tag* value;
      struct woff2_tag key;
      std::memcpy(key.tag, s, 4);
      value = std::lower_bound(
          woff2_tag_table, woff2_tag_table+63, key,
          woff2_tag_cmp
        );
      return (std::memcmp(key.tag,value->tag,4)==0) ? value->n : 63u;
    }

    unsigned char const* woff2_tag_fromi(unsigned int x) {
      unsigned int check_value = x&63;
      if (check_value == 63)
        return nullptr;
      else return woff2_tag_list[check_value].tag;
    }
    //END   woff2 tools

    //BEGIN woff2 / rule-of-six
    woff2::woff2(mmaptwo::mmaptwo_i* xfh, bool sane_tf)
      : fh(xfh), offsets(nullptr)
    {
      initparse(sane_tf);
    }

    woff2::~woff2(void) {
      if (fh) {
        delete fh;
      }
      if (offsets) {
        delete offsets;
      }
      return;
    }

    woff2::woff2(woff2&& other) noexcept
      : fh(nullptr), offsets(nullptr)
    {
      transfer(static_cast<woff2&&>(other));
      return;
    }

    woff2& woff2::operator=(woff2&& other) noexcept {
      transfer(static_cast<woff2&&>(other));
      return *this;
    }

    void woff2::transfer(woff2&& other) noexcept {
      /* release-reset the mapping interface */{
        mmaptwo::mmaptwo_i *n_fh = other.fh;
        other.fh = nullptr;
        if (this->fh) {
          delete this->fh;
        }
        this->fh = n_fh;
      }
      /* release-reset the offset table */{
        offset_table *n_ot = other.offsets;
        other.offsets = nullptr;
        if (this->offsets) {
          delete this->offsets;
        }
        this->offsets = n_ot;
      }
      return;
    }

    void woff2::initparse(bool sane_tf) {
      std::unique_ptr<sequential> seq = seq_unique(fh);
      std::unique_ptr<offset_table> offsets;
      if (!seq) {
        throw api_exception(api_error::ErrInit);
      }
      /* acquire the header */{
        unsigned char wheader[48];
        int i;
        for (i = 0; i < 48; ++i) {
          int ch = seq->get_byte();
          if (ch < 0)
            break;
          else wheader[i] = static_cast<unsigned char>(ch&255);
        }
        if (i < 48) {
          throw api_exception(api_error::ErrSanitize);
        }
        /* inspect the signature */{
          unsigned char sig[4] = {0x77, 0x4F, 0x46, 0x32};
          if (std::memcmp(sig, wheader, 4) != 0) {
            throw api_exception(api_error::ErrSanitize);
          }
        }
        unsigned short table_count;
        /* parse out the table count */{
          table_count = woff2_read_u16be(wheader+12);
          if (table_count > std::numeric_limits<size_t>::max()) {
            throw api_exception(api_error::ErrMemory);
          } else {
            offsets = offtable_unique(table_count);
            if (!offsets)
              throw api_exception(api_error::ErrMemory);
          }
        }
        /* iterate through the table directory */{
          unsigned short table_i;
          uint32 next_offset = 0u;
          for (table_i = 0u; table_i < table_count; ++table_i) {
            unsigned char tag_text[4];
            unsigned int enc_path;
            uint32 len, use_len;
            woff2_read_tag(seq.get(), tag_text, enc_path);
            /* read the original length */{
              woff2_read_uv128(seq.get(), len);
            }
            /* read the transform length */
            if (woff2_req_twolen(tag_text,enc_path)) {
              woff2_read_uv128(seq.get(), use_len);
            } else use_len = len;
            /* add entry to offset table */{
              struct offset_line& line = (*offsets)[table_i];
              std::memcpy(line.tag, tag_text, 4);
              line.checksum = 0u;
              line.offset = next_offset;
              line.length = use_len;
              if (use_len > 0xFFffFFff-next_offset) {
                throw api_exception(api_error::ErrSanitize);
              }
              next_offset += use_len;
            }
          }
        }
      }
      if (sane_tf) {
        /* TODO sanitize the rest */
        if (false) {
          throw api_exception(api_error::ErrSanitize);
        }
      }
      this->offsets = offsets.release();
      return;
    }
    //END   woff2 / rule-of-six

    //BEGIN woff2 / allocation
    void* woff2::operator new(std::size_t sz) {
      return ::operator new(sz);
    }

    void* woff2::operator new[](std::size_t sz) {
      return ::operator new(sz);
    }

    void woff2::operator delete(void* p, std::size_t sz) noexcept {
      return ::operator delete(p);
    }

    void woff2::operator delete[](void* p, std::size_t sz) noexcept {
      return ::operator delete(p);
    }

    woff2* woff2_new(mmaptwo::mmaptwo_i* xfh, bool sane_tf) noexcept {
      try {
        return new woff2(xfh,sane_tf);
      } catch (api_exception const& ) {
        return nullptr;
      } catch (std::bad_alloc const& ) {
        return nullptr;
      }
    }

    util_unique_ptr<woff2> woff2_unique
        (mmaptwo::mmaptwo_i* xfh, bool sane_tf) noexcept
    {
      return util_unique_ptr<woff2>(woff2_new(xfh,sane_tf));
    }

    void woff2_destroy(woff2* x) noexcept {
      if (x) {
        delete x;
      }
    }
    //END   woff2 / allocation

    //BEGIN woff2 / public
    offset_table const& woff2::get_offsets(void) const noexcept {
      return *(this->offsets);
    }
    //END   woff2 / public
  };
};
