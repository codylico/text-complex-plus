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

namespace text_complex {
  namespace access {
    /* TODO optimize */
    static struct {
      unsigned int n;
      unsigned char tag[5];
    } tcmplxA_woff2_tag_table[64] = {
       {0, "cmap"},	{16, "EBLC"},	{32, "CBDT"},	{48, "gvar"},
       {1, "head"},	{17, "gasp"},	{33, "CBLC"},	{49, "hsty"},
       {2, "hhea"},	{18, "hdmx"},	{34, "COLR"},	{50, "just"},
       {3, "hmtx"},	{19, "kern"},	{35, "CPAL"},	{51, "lcar"},
       {4, "maxp"},	{20, "LTSH"},	{36, "SVG "},	{52, "mort"},
       {5, "name"},	{21, "PCLT"},	{37, "sbix"},	{53, "morx"},
       {6, "OS/2"},	{22, "VDMX"},	{38, "acnt"},	{54, "opbd"},
       {7, "post"},	{23, "vhea"},	{39, "avar"},	{55, "prop"},
       {8, "cvt "},	{24, "vmtx"},	{40, "bdat"},	{56, "trak"},
       {9, "fpgm"},	{25, "BASE"},	{41, "bloc"},	{57, "Zapf"},
      {10, "glyf"},	{26, "GDEF"},	{42, "bsln"},	{58, "Silf"},
      {11, "loca"},	{27, "GPOS"},	{43, "cvar"},	{59, "Glat"},
      {12, "prep"},	{28, "GSUB"},	{44, "fdsc"},	{60, "Gloc"},
      {13, "CFF "},	{29, "EBSC"},	{45, "feat"},	{61, "Feat"},
      {14, "VORG"},	{30, "JSTF"},	{46, "fmtx"},	{62, "Sill"},
      {15, "EBDT"},	{31, "MATH"},	{47, "fvar"},	{63, {0,0,0,0}}
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


    //BEGIN woff2 static
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
      for (i = 0; i < 63; ++i) {
        if (std::memcmp(s,tcmplxA_woff2_tag_table[i].tag,4) == 0) {
          return tcmplxA_woff2_tag_table[i].n;
        }
      }
      return 63u;
    }

    unsigned char const* woff2_tag_fromi(unsigned int x) {
      unsigned int check_value = x&63;
      unsigned int i;
      if (x == 63)
        return nullptr;
      else for (i = 0; i < 63; ++i) {
        if (check_value == tcmplxA_woff2_tag_table[i].n) {
          return tcmplxA_woff2_tag_table[i].tag;
        }
      }
      return nullptr;
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
