/**
 * @file tcmplx-access-plus/fixlist.cpp
 * @brief Prefix code list
 * @author Cody Licorish (svgmovement@gmail.com)
 */
#define TCMPLX_AP_WIN32_DLL_INTERNAL
#include "text-complex-plus/access/fixlist.hpp"
#include <new>
#include <stdexcept>
#include <limits>
#include <utility>
#include <cstring>
#include <algorithm>
#include <vector>
#include <climits>

namespace text_complex {
  namespace access {
    static
    struct prefix_line const fixlist_ps_BrotliComplex[] = {
      {  0    /*0*/, 2u, 0 },
      {0xe /*1110*/, 4u, 0 },
      {0x6  /*110*/, 3u, 0 },
      {0x1   /*01*/, 2u, 0 },
      {0x2   /*10*/, 2u, 0 },
      {0xf /*1111*/, 4u, 0 }
    };
    static
    struct prefix_line const fixlist_ps_BrotliS1[] = {
      {  0    /*-*/, 0u, 0 },
    };
    static
    struct prefix_line const fixlist_ps_BrotliS2[] = {
      {  0    /*0*/, 1u, 0 },
      {0x1    /*1*/, 1u, 0 }
    };
    static
    struct prefix_line const fixlist_ps_BrotliS3[] = {
      {  0    /*0*/, 1u, 0 },
      {0x2   /*10*/, 2u, 0 },
      {0x3   /*11*/, 2u, 0 }
    };
    static
    struct prefix_line const fixlist_ps_BrotliS4A[] = {
      {  0   /*00*/, 2u, 0 },
      {0x1   /*01*/, 2u, 0 },
      {0x2   /*10*/, 2u, 0 },
      {0x3   /*11*/, 2u, 0 }
    };
    static
    struct prefix_line const fixlist_ps_BrotliS4B[] = {
      {  0    /*0*/, 1u, 0 },
      {0x2   /*10*/, 2u, 0 },
      {0x6  /*110*/, 3u, 0 },
      {0x7  /*111*/, 3u, 0 }
    };

    static
    struct {
      size_t n;
      struct prefix_line const* v;
    } const fixlist_ps[] = {
      { 6u, fixlist_ps_BrotliComplex },
      { 1u, fixlist_ps_BrotliS1 },
      { 2u, fixlist_ps_BrotliS2 },
      { 3u, fixlist_ps_BrotliS3 },
      { 4u, fixlist_ps_BrotliS4A },
      { 4u, fixlist_ps_BrotliS4B }
    };


    /**
     * @internal
     * @brief Back reference to prefix list.
     */
    struct prefix_ref {
      /** @internal @brief Left child. */
      unsigned int index;
      /** @internal @brief Right child. */
      uint32 freq;
    };
    /**
     * @internal
     * @brief Min-heap item for code tree building.
     * @todo use std::push_heap and std::pop_heap
     */
    struct prefix_heapitem {
      /** @internal @brief histogram value. */
      uint32 freq;
      /** @internal @brief block width. */
      uint32 total_frac;
      /** @internal @brief block width below midline. */
      uint32 small_frac;
      /** @internal @brief number of cells on the midline. */
      uint32 midline;
    };
    /**
     * @internal
     * @brief Arguments for length generator recursive call.
     */
    struct prefix_arg {
      /** @internal Reference index, first reference. */
      unsigned int begin;
      /** @internal Reference index, one-past-last reference. */
      unsigned int end;
      /** @internal block fraction upper bound. */
      int upper_bound;
      /** @internal block fraction lower bound. */
      int lower_bound;
      /** @internal @brief goal block width. */
      uint32 total_frac;
    };
    /**
     * @internal
     * @brief Data for length generator recursive call.
     */
    struct prefix_state {
      struct prefix_arg args;
      struct prefix_heapitem item;
    };

    /**
     * @internal
     * @brief Queue of length generator recursive calls.
     */
    template <size_t n>
    class prefix_statequeue {
    private:
      struct prefix_state p[n];
      size_t current;
      size_t total;

    public:
      /** @internal @brief Constructor. */
      prefix_statequeue(void) noexcept;
      /** @internal @return whether the queue is empty */
      bool empty(void) const noexcept;
      /**
       * @internal @brief Push a state onto the queue.
       * @param st state to push
       */
      void push(prefix_state st) noexcept;
      /** @internal @brief Remove the next element from the queue. */
      void pop(void) noexcept;
      /** @internal @return the next item in the queue */
      prefix_state& front(void) noexcept;
      /**
       * @internal @brief Array index.
       * @param i index
       * @return the item at `[i]`
       */
      prefix_state& operator[](size_t i) noexcept;
      size_t capacity(void) const noexcept;
    };

    /**
     * @internal
     * @brief Code comparison maximum length difference.
     */
    static
    constexpr unsigned int fixlist_codecmp_maxdiff =
        (CHAR_BIT)*sizeof(unsigned short);

    /**
     * @brief Add two numbers, clamping the result.
     * @param a addend
     * @param b addend
     * @return a clamped sum
     */
    static
    uint32 fixlist_addclamp(uint32 a, uint32 b);
    /**
     * @brief Sum two heap items together.
     * @param a one addend
     * @param b another addend
     * @return sum heap item
     */
    static
    prefix_heapitem operator+(prefix_heapitem a, prefix_heapitem b);
    /**
     * @brief Compare two heap items.
     * @param a one item
     * @param b another item
     * @return whether `a` is "less than" `b`
     */
    static
    bool prefix_heap_cmp(prefix_heapitem const& a, prefix_heapitem const& b);
    /**
     * @internal
     * @brief Compare two prefix list references.
     * @param a one reference
     * @param b another reference
     * @return true if `a`'s frequency is greater than `b`'s,
     *   zero otherwise
     */
    static
    bool prefix_ref_cmp(prefix_ref const& a, prefix_ref const& b);
    /**
     * @internal
     * @brief Find the least significant set bit.
     * @param x input
     * @return a number with only that set bit
     */
    static
    uint32 fixlist_min_frac(uint32 x) noexcept;
    /**
     * @brief Check if the heap can provide packable items.
     * @param h heap represented by an array
     * @param n size of heap
     * @param frac fraction to meet
     * @return nonzero if two items can fit in such
     */
    static
    bool fixlist_heap_packable
      ( std::vector<prefix_heapitem>::iterator begin,
        std::vector<prefix_heapitem>::iterator end, uint32 frac);
    /**
     * @internal
     * @brief Compare two lines.
     * @param a one line
     * @param b another line
     * @return whether `a` < `b`
     */
    static
    bool fixlist_code_cmp(prefix_line const& a, prefix_line const& b);

    //BEGIN prefix-list / static
    prefix_heapitem operator+(prefix_heapitem a, prefix_heapitem b) {
      struct prefix_heapitem const out = {
        fixlist_addclamp(a.freq,b.freq),
        fixlist_addclamp(a.total_frac, b.total_frac),
        fixlist_addclamp(a.small_frac, b.small_frac),
        fixlist_addclamp(a.midline, b.midline)
      };
      return out;
    }

    uint32 fixlist_addclamp(uint32 a, uint32 b) {
      return a > 0xFFffFFff-b ? 0xFFffFFff : a+b;
    }

    bool prefix_ref_cmp(prefix_ref const& a, prefix_ref const& b) {
      return a.freq > b.freq;
    }

    bool prefix_heap_cmp(prefix_heapitem const& a, prefix_heapitem const& b) {
      if (a.total_frac < b.total_frac)
        return false;
      else if (a.total_frac > b.total_frac)
        return true;
      else return a.freq > b.freq;
    }

    uint32 fixlist_min_frac(uint32 x) noexcept {
      unsigned int n;
      if (x == 0u)
        return 0u;
      else for (n = 0u; (x & 1u) == 0u; x >>= 1, ++n) {
        continue;
      }
      return static_cast<uint32>(1u)<<n;
    }

    bool fixlist_heap_packable
      ( std::vector<prefix_heapitem>::iterator begin,
        std::vector<prefix_heapitem>::iterator end, uint32 frac)
    {
      if (end-begin < 2)
        return false;
      else if (begin->total_frac > frac)
        return false;
      else {
        std::vector<prefix_heapitem>::iterator it = begin;
        ++it;
        if (it->total_frac <= frac)
          return true;
        ++it;
        if (it == end)
          return false;
        else return (it->total_frac <= frac);
      }
    }

    bool fixlist_code_cmp(prefix_line const& a, prefix_line const& b) {
      if (a.len < b.len)
        return true;
      else if (a.len > b.len)
        return false;
      else return a.code < b.code;
    }
    //END   prefix-list / static

    //BEGIN prefix-state-queue / public internal
    template <size_t n>
    prefix_statequeue<n>::prefix_statequeue(void) noexcept
      : current(0u), total(0u)
    {
#ifndef NDEBUG
      std::memset(p, 0, sizeof(p));
#endif /*NDEBUG*/
    }

    template <size_t n>
    bool prefix_statequeue<n>::empty(void) const noexcept {
      return current >= total;
    }

    template <size_t n>
    void prefix_statequeue<n>::push(prefix_state st) noexcept {
      p[total] = st;
      total += 1u;
    }

    template <size_t n>
    void prefix_statequeue<n>::pop(void) noexcept {
      current += 1u;
    }

    template <size_t n>
    prefix_state& prefix_statequeue<n>::front(void) noexcept {
      return p[current];
    }

    template <size_t n>
    prefix_state& prefix_statequeue<n>::operator[](size_t i) noexcept {
      return p[i];
    }

    template <size_t n>
    size_t prefix_statequeue<n>::capacity(void) const noexcept {
      return total;
    }
    //END   prefix-state-queue / public internal

    //BEGIN prefix_list / rule-of-six
    prefix_list::prefix_list(size_t n)
      : p(nullptr), n(0u)
    {
      resize(n);
      return;
    }

    prefix_list::~prefix_list(void) {
      if (this->p) {
        delete[] this->p;
      }
      this->p = nullptr;
      this->n = 0u;
      return;
    }

    prefix_list::prefix_list(prefix_list const& other)
      : p(nullptr), n(0u)
    {
      duplicate(other);
      return;
    }

    prefix_list& prefix_list::operator=(prefix_list const& other) {
      duplicate(other);
      return *this;
    }

    prefix_list::prefix_list(prefix_list&& other) noexcept
      : p(nullptr), n(0u)
    {
      transfer(static_cast<prefix_list&&>(other));
      return;
    }

    prefix_list& prefix_list::operator=(prefix_list&& other) noexcept {
      transfer(static_cast<prefix_list&&>(other));
      return *this;
    }

    void prefix_list::duplicate(prefix_list const& other) {
      if (this == &other)
        return;
      resize(other.n);
      size_t i;
      for (i = 0; i < n; ++i) {
        p[i] = other.p[i];
      }
      return;
    }

    void prefix_list::transfer(prefix_list&& other) noexcept {
      prefix_line* new_p;
      size_t new_n;
      /* release */{
        new_p = other.p;
        other.p = nullptr;
        new_n = other.n;
        other.n = 0u;
      }
      /* reset */{
        if (this->p) {
          delete[] this->p;
        }
        this->p = new_p;
        this->n = new_n;
      }
      return;
    }

    void prefix_list::resize(size_t n) {
      struct prefix_line *ptr;
      if (n == 0u) {
        if (this->p) {
          delete[] this->p;
        }
        this->p = nullptr;
        this->n = 0u;
        return;
      }
      if (n >= std::numeric_limits<size_t>::max()/
          sizeof(struct prefix_line))
      {
        throw std::bad_alloc();
      }
      ptr = new struct prefix_line[n];
      if (this->p) {
        delete[] this->p;
      }
      this->p = ptr;
      this->n = n;
      return;
    }
    //END   prefix_list / rule-of-six

    //BEGIN prefix_list / allocation
    void* prefix_list::operator new(std::size_t sz) {
      return ::operator new(sz);
    }

    void* prefix_list::operator new[](std::size_t sz) {
      return ::operator new(sz);
    }

    void prefix_list::operator delete(void* p, std::size_t sz) noexcept {
      return ::operator delete(p);
    }

    void prefix_list::operator delete[](void* p, std::size_t sz) noexcept {
      return ::operator delete(p);
    }

    prefix_list* fixlist_new(size_t n) noexcept {
      try {
        return new prefix_list(n);
      } catch (api_exception const& ) {
        return nullptr;
      } catch (std::bad_alloc const& ) {
        return nullptr;
      }
    }

    util_unique_ptr<prefix_list> fixlist_unique(size_t n) noexcept {
      return util_unique_ptr<prefix_list>(fixlist_new(n));
    }

    void fixlist_destroy(prefix_list* x) noexcept {
      if (x) {
        delete x;
      }
    }
    //END   prefix_list / allocation

    //BEGIN prefix_list / range-based
    prefix_line* prefix_list::begin(void) noexcept {
      return this->p;
    }

    prefix_line const* prefix_list::begin(void) const noexcept {
      return this->p;
    }

    prefix_line* prefix_list::end(void) noexcept {
      return this->p+this->n;
    }

    prefix_line const* prefix_list::end(void) const noexcept {
      return this->p+this->n;
    }
    //END   prefix_list / range-based

    //BEGIN prefix_list / array-compat
    size_t prefix_list::size(void) const noexcept {
      return this->n;
    }

    prefix_line& prefix_list::operator[](size_t i) noexcept {
      return this->p[i];
    }

    prefix_line const& prefix_list::operator[](size_t i) const noexcept {
      return this->p[i];
    }

    prefix_line& prefix_list::at(size_t i) {
      if (i >= this->n)
        throw std::out_of_range("text_complex::access::prefix_list::at");
      return this->p[i];
    }

    prefix_line const& prefix_list::at(size_t i) const {
      if (i >= this->n)
        throw std::out_of_range("text_complex::access::prefix_list::at");
      return this->p[i];
    }
    //END   prefix_list / array-compat

    //BEGIN prefix_histogram / rule-of-six
    prefix_histogram::prefix_histogram(size_t n)
      : p(nullptr), n(0u)
    {
      resize(n);
      return;
    }

    prefix_histogram::~prefix_histogram(void) {
      if (this->p) {
        delete[] this->p;
      }
      this->p = nullptr;
      this->n = 0u;
      return;
    }

    prefix_histogram::prefix_histogram(prefix_histogram const& other)
      : p(nullptr), n(0u)
    {
      duplicate(other);
      return;
    }

    prefix_histogram& prefix_histogram::operator=
        (prefix_histogram const& other)
    {
      duplicate(other);
      return *this;
    }

    prefix_histogram::prefix_histogram(prefix_histogram&& other) noexcept
      : p(nullptr), n(0u)
    {
      transfer(static_cast<prefix_histogram&&>(other));
      return;
    }

    prefix_histogram& prefix_histogram::operator=
        (prefix_histogram&& other) noexcept
    {
      transfer(static_cast<prefix_histogram&&>(other));
      return *this;
    }

    void prefix_histogram::duplicate(prefix_histogram const& other) {
      if (this == &other)
        return;
      resize(other.n);
      size_t i;
      for (i = 0; i < n; ++i) {
        p[i] = other.p[i];
      }
      return;
    }

    void prefix_histogram::transfer(prefix_histogram&& other) noexcept {
      uint32* new_p;
      size_t new_n;
      /* release */{
        new_p = other.p;
        other.p = nullptr;
        new_n = other.n;
        other.n = 0u;
      }
      /* reset */{
        if (this->p) {
          delete[] this->p;
        }
        this->p = new_p;
        this->n = new_n;
      }
      return;
    }

    void prefix_histogram::resize(size_t n) {
      uint32 *ptr;
      if (n == 0u) {
        if (this->p) {
          delete[] this->p;
        }
        this->p = nullptr;
        this->n = 0u;
        return;
      }
      if (n >= std::numeric_limits<size_t>::max()/sizeof(uint32))
      {
        throw std::bad_alloc();
      }
      ptr = new uint32[n];
      if (this->p) {
        delete[] this->p;
      }
      this->p = ptr;
      this->n = n;
      return;
    }
    //END   prefix_histogram / rule-of-six

    //BEGIN prefix_histogram / allocation
    void* prefix_histogram::operator new(std::size_t sz) {
      return ::operator new(sz);
    }

    void* prefix_histogram::operator new[](std::size_t sz) {
      return ::operator new(sz);
    }

    void prefix_histogram::operator delete(void* p, std::size_t sz) noexcept {
      return ::operator delete(p);
    }

    void prefix_histogram::operator delete[](void* p, std::size_t sz) noexcept {
      return ::operator delete(p);
    }

    prefix_histogram* fixlist_histogram_new(size_t n) noexcept {
      try {
        return new prefix_histogram(n);
      } catch (api_exception const& ) {
        return nullptr;
      } catch (std::bad_alloc const& ) {
        return nullptr;
      }
    }

    util_unique_ptr<prefix_histogram> fixlist_histogram_unique
        (size_t n) noexcept
    {
      return util_unique_ptr<prefix_histogram>(fixlist_histogram_new(n));
    }

    void fixlist_histogram_destroy(prefix_histogram* x) noexcept {
      if (x) {
        delete x;
      }
    }
    //END   prefix_histogram / allocation

    //BEGIN prefix_histogram / range-based
    uint32* prefix_histogram::begin(void) noexcept {
      return this->p;
    }

    uint32 const* prefix_histogram::begin(void) const noexcept {
      return this->p;
    }

    uint32* prefix_histogram::end(void) noexcept {
      return this->p+this->n;
    }

    uint32 const* prefix_histogram::end(void) const noexcept {
      return this->p+this->n;
    }
    //END   prefix_histogram / range-based

    //BEGIN prefix_histogram / array-compat
    size_t prefix_histogram::size(void) const noexcept {
      return this->n;
    }

    uint32& prefix_histogram::operator[](size_t i) noexcept {
      return this->p[i];
    }

    uint32 const& prefix_histogram::operator[](size_t i) const noexcept {
      return this->p[i];
    }

    uint32& prefix_histogram::at(size_t i) {
      if (i >= this->n)
        throw std::out_of_range("text_complex::access::prefix_histogram::at");
      return this->p[i];
    }

    uint32 const& prefix_histogram::at(size_t i) const {
      if (i >= this->n)
        throw std::out_of_range("text_complex::access::prefix_histogram::at");
      return this->p[i];
    }
    //END   prefix_histogram / array-compat

    //BEGIN prefix_list / namespace local
    void fixlist_gen_codes(prefix_list& dst, api_error& ae) noexcept {
      size_t counts[16] = {0u};
      unsigned int code_mins[16] = {0u};
      /* step 1. compute histogram */{
        size_t i;
        size_t const sz = dst.size();
        for (i = 0u; i < sz; ++i) {
          unsigned short const len = dst[i].len;
          if (len >= 16u) {
            ae = api_error::FixLenRange; return;
          } else counts[len] += 1u;
        }
      }
      /* step 2. find the minimum code for each length */{
        unsigned int next_code = 0u;
        unsigned int cap_tracker = 1u;
        int j;
        /* ignore length zero */
        for (j = 1; j < 16; ++j) {
          next_code <<= 1;
          cap_tracker <<= 1;
          code_mins[j] = next_code;
          if (counts[j] > cap_tracker-next_code) {
            ae = api_error::FixCodeAlloc; return;
          } else next_code += counts[j];
        }
      }
      /* step 3. "allocate" codes in order */{
        size_t i;
        size_t const sz = dst.size();
        for (i = 0u; i < sz; ++i) {
          struct prefix_line& line = dst[i];
          unsigned short const len = line.len;
          if (len > 0) {
            line.code = code_mins[len];
            code_mins[len] += 1u;
          } else line.code = 0u;
        }
      }
      ae = api_error::Success;
      return;
    }

    void fixlist_preset
      (prefix_list& dst, prefix_preset i, api_error& ae) noexcept
    {
      size_t const n = sizeof(fixlist_ps)/sizeof(fixlist_ps[0]);
      if (static_cast<unsigned int>(i) >= n) {
        ae = api_error::Param; return;
      }
      /* allocate */{
        unsigned int const i_u = static_cast<unsigned int>(i);
        size_t const sz = fixlist_ps[i_u].n;
        try {
          prefix_list n_list(sz);
          std::memcpy(&n_list[0], fixlist_ps[i_u].v,
            sizeof(struct prefix_line)*sz);
          dst = std::move(n_list);
        } catch (std::bad_alloc const& ) {
          ae = api_error::Memory; return;
        }
      }
      ae = api_error::Success;
      return;
    }

    void fixlist_gen_lengths
      ( prefix_list& dst, prefix_histogram const& table,
        unsigned int max_bits, api_error& ae) noexcept
    {
      if (dst.size() > 32768u || max_bits > 15u) {
        ae = api_error::FixLenRange;
        return;
      } else {
        std::vector<prefix_ref> nodes;
        std::vector<prefix_heapitem> heap;
        try {
          nodes.reserve(dst.size());
          heap.reserve(dst.size()*2u);
        } catch (std::bad_alloc const& ) {
          ae = api_error::Memory;
          return;
        }
        /* */{
          size_t i;
          for (i = 0u; i < table.size(); ++i) {
            if (table[i] > 0u) {
              struct prefix_ref const ref =
                { static_cast<unsigned int>(i), table[i] };
              nodes.push_back(ref);
            } else {
              dst[i].len = 0u;
            }
          }
          if (nodes.size() <= 2u) {
            if (max_bits > 0u) {
              for (i = 0u; i < nodes.size(); ++i)
                dst[nodes[i].index].len = 1u;
              ae = api_error::Success;
              return;
            } else {
              ae = api_error::FixLenRange;
              return;
            }
          }
          std::sort(nodes.begin(), nodes.end(), prefix_ref_cmp);
        }
        /* do the algorithm */{
          prefix_statequeue<16> states;
          /* initialize */{
            uint32 const n = static_cast<uint32>(nodes.size());
            struct prefix_state const st = {
                { /* begin */0u,
                  /* end */static_cast<unsigned int>(n),
                  /* upper_bound */-1,
                  /* lower_bound */-static_cast<int>(max_bits),
                  /* total_frac */(n-1u)<<max_bits }
              };
            states.push(st);
          }
          /* run */while (!states.empty()) {
            struct prefix_state& st = states.front();
            int l;
            unsigned long int rem_frac = st.args.total_frac;
            int const midline = st.args.upper_bound +
              (st.args.lower_bound-st.args.upper_bound-1)/2;
            /* */{
              std::memset(&st.item, 0, sizeof(struct prefix_heapitem));
            }
            for (l = st.args.lower_bound; rem_frac > 0u; ++l) {
              uint32 const frac = 1u<<(static_cast<int>(max_bits)+l);
              uint32 const min_frac = fixlist_min_frac(rem_frac);
              /* add some items */if (l <= st.args.upper_bound) {
                size_t i;
                for (i = st.args.begin; i < st.args.end; ++i) {
                  struct prefix_heapitem const it =
                    { /* freq */nodes[i].freq, /*total_frac */frac,
                      /* small_frac */(l < midline ? frac : 0u),
                      /* midline */(l == midline ? 1u : 0u)};
                  heap.push_back(it);
                }
                std::make_heap(heap.begin(), heap.end(), prefix_heap_cmp);
              }
              /* inspect */
              if (heap.empty() || frac > min_frac) {
                ae = api_error::FixLenRange;
                return;
              } else if (frac == min_frac) {
                if (heap.front().total_frac != min_frac) {
                  ae = api_error::FixLenRange;
                  return;
                } else {
                  rem_frac -= min_frac;
                  st.item = st.item + heap.front();
                  std::pop_heap(heap.begin(), heap.end(), prefix_heap_cmp);
                  heap.pop_back();
                }
              }
              /* pack the rest */{
                std::vector<prefix_heapitem>::iterator it = heap.end();
                while (fixlist_heap_packable(heap.begin(), it, frac)) {
                  struct prefix_heapitem item1 = heap.front();
                  std::pop_heap(heap.begin(), it, prefix_heap_cmp);
                  --it;
                  struct prefix_heapitem item2 = heap.front();
                  std::pop_heap(heap.begin(), it, prefix_heap_cmp);
                  --it;
                  *it = item1 + item2;
                  ++it;
                  std::push_heap(heap.begin(), it, prefix_heap_cmp);
                }
                if ((!heap.empty()) && heap.front().total_frac <= frac) {
                  /* discard */
                  std::pop_heap(heap.begin(), it, prefix_heap_cmp);
                  --it;
                }
                heap.resize(it-heap.begin());
              }
            }
            /* add new states */{
              int const bound_span =
                (st.args.upper_bound-st.args.lower_bound);
              if (bound_span >= 3) {
                /* construct "D" span */{
                  struct prefix_state const d_state = {
                      { /* begin */ st.args.end-st.item.midline,
                        /* end */ st.args.end,
                        /* upper_bound */midline-1,
                        /* lower_bound */st.args.lower_bound,
                        /* total_frac */st.item.small_frac }
                    };
                  states.push(d_state);
                }
                /* construct "A" span */{
                  int const imax_bits = static_cast<int>(max_bits);
                  struct prefix_state const a_state = {
                      { /* begin */ st.args.begin,
                        /* end */ st.args.end-st.item.midline,
                        /* upper_bound */st.args.upper_bound,
                        /* lower_bound */midline+1,
                        /* total_frac */st.item.total_frac
                            - st.item.small_frac
                            -   ( (1u<<(imax_bits+st.args.upper_bound+1))
                                - (1u<<(imax_bits+midline)))
                              * st.item.midline }
                    };
                  states.push(a_state);
                }
              }
            }
            states.pop();
            heap.clear();
          }
          /* parse out the lengths */{
            size_t i;
            uint32 lengths[16] = {0u};
            int const imax_bits = max_bits;
            for (i = 0u; i < states.capacity(); ++i) {
              struct prefix_state const& st = states[i];
              uint32 const offset =
                static_cast<uint32>(nodes.size())-st.args.end;
              int const bound_span = st.args.upper_bound-st.args.lower_bound;
              int const midline = st.args.lower_bound+(bound_span/2);
              lengths[-midline] = offset+st.item.midline;
              if (bound_span <= 2) {
                if (st.args.upper_bound > midline) {
                  uint32 const x =
                      st.item.total_frac
                    - (st.item.midline<<(imax_bits+midline))
                    - st.item.small_frac;
                  lengths[-st.args.upper_bound] =
                    (x>>(imax_bits+st.args.upper_bound)) + offset;
                }
                if (st.args.lower_bound < midline) {
                  uint32 const x = st.item.small_frac;
                  lengths[-st.args.lower_bound] =
                    (x>>(imax_bits+st.args.lower_bound)) + offset;
                }
              }
            }
            /* post to the prefix list */{
              size_t i = nodes.size();
              size_t j;
              for (j = 15u; j > 0u; --j) {
                size_t const front = nodes.size()-lengths[j];
                for (; i > front; --i) {
                  dst[nodes[i-1u].index].len = j;
                }
              }
            }
          }
        }
        ae = api_error::Success;
        return;
      }
    }

    void fixlist_codesort(prefix_list& dst, api_error& ae) noexcept {
      try {
        std::sort(dst.begin(), dst.end(), fixlist_code_cmp);
      } catch (std::bad_alloc const& ) {
        ae = api_error::Memory;
        return;
      }
      ae = api_error::Success;
      return;
    }

    size_t fixlist_codebsearch
      (prefix_list const& dst, unsigned int n, unsigned int bits) noexcept
    {
      struct prefix_line key;
      key.len = static_cast<unsigned short>(n);
      key.code = static_cast<unsigned short>(bits);
      /* */{
        struct prefix_line const* x = std::lower_bound
          (dst.begin(), dst.end(), key, fixlist_code_cmp);
        return (x < dst.end() && x->len == n && x->code == bits)
          ? static_cast<size_t>(x-dst.begin())
          : std::numeric_limits<size_t>::max();
      }
    }
    //END   prefix_list / namespace local
  };
};
