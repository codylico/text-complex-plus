/**
 * @file text-complex-plus/access/bdict_p.hpp
 * @brief built-in dictionary
 * @author Cody Licorish (svgmovement@gmail.com)
 */
#ifndef hg_TextComplexAccessP_BDict_pH_
#define hg_TextComplexAccessP_BDict_pH_

namespace text_complex {
  namespace access {
    //BEGIN built-in dictionary / transform
    /** @internal @brief Callback types. */
    enum struct bdict_cb : unsigned short {
      Identity = 0u,
      FermentFirst = 1u,
      FermentAll = 2u,
      OmitFirst1 = 3u,
      OmitFirst2 = 4u,
      OmitFirst3 = 5u,
      OmitFirst4 = 6u,
      OmitFirst5 = 7u,
      OmitFirst6 = 8u,
      OmitFirst7 = 9u,
      OmitFirst8 = 10u,
      OmitFirst9 = 11u,
      OmitLast1 = 12u,
      OmitLast2 = 13u,
      OmitLast3 = 14u,
      OmitLast4 = 15u,
      OmitLast5 = 16u,
      OmitLast6 = 17u,
      OmitLast7 = 18u,
      OmitLast8 = 19u,
      OmitLast9 = 20u
    };

    /**
     * @internal
     * @brief Perform a word transform and add to an output buffer.
     * @param[out] dst output buffer
     * @param[in,out] dstlen write position
     * @param src source word
     * @param srclen length of source word
     */
    void bdict_cb_do
      ( unsigned char* dst, unsigned int& dstlen,
        unsigned char const* src, unsigned int srclen, bdict_cb k) noexcept;
    //END   built-in dictionary / transform

    //BEGIN built-in dictionary / access
    /**
     * @brief Access the 4-length section of the dictionary.
     * @param i word index
     * @return a pointer to the requested word in the 4-length
     *   dictionary section
     */
    unsigned char const* bdict_access04(unsigned int i) noexcept;

    /**
     * @brief Access the 5-length section of the dictionary.
     * @param i word index
     * @return a pointer to the requested word in the 5-length
     *   dictionary section
     */
    unsigned char const* bdict_access05(unsigned int i) noexcept;

    /**
     * @brief Access the 6-length section of the dictionary.
     * @param i word index
     * @return a pointer to the requested word in the 6-length
     *   dictionary section
     */
    unsigned char const* bdict_access06(unsigned int i) noexcept;

    /**
     * @brief Access the 7-length section of the dictionary.
     * @param i word index
     * @return a pointer to the requested word in the 7-length
     *   dictionary section
     */
    unsigned char const* bdict_access07(unsigned int i) noexcept;

    /**
     * @brief Access the 8-length section of the dictionary.
     * @param i word index
     * @return a pointer to the requested word in the 8-length
     *   dictionary section
     */
    unsigned char const* bdict_access08(unsigned int i) noexcept;

    /**
     * @brief Access the 9-length section of the dictionary.
     * @param i word index
     * @return a pointer to the requested word in the 9-length
     *   dictionary section
     */
    unsigned char const* bdict_access09(unsigned int i) noexcept;

    /**
     * @brief Access the 10-length section of the dictionary.
     * @param i word index
     * @return a pointer to the requested word in the 10-length
     *   dictionary section
     */
    unsigned char const* bdict_access10(unsigned int i) noexcept;

    /**
     * @brief Access the 11-length section of the dictionary.
     * @param i word index
     * @return a pointer to the requested word in the 11-length
     *   dictionary section
     */
    unsigned char const* bdict_access11(unsigned int i) noexcept;

    /**
     * @brief Access the 12-length section of the dictionary.
     * @param i word index
     * @return a pointer to the requested word in the 12-length
     *   dictionary section
     */
    unsigned char const* bdict_access12(unsigned int i) noexcept;

    /**
     * @brief Access the 13-length section of the dictionary.
     * @param i word index
     * @return a pointer to the requested word in the 13-length
     *   dictionary section
     */
    unsigned char const* bdict_access13(unsigned int i) noexcept;

    /**
     * @brief Access the 14-length section of the dictionary.
     * @param i word index
     * @return a pointer to the requested word in the 14-length
     *   dictionary section
     */
    unsigned char const* bdict_access14(unsigned int i) noexcept;

    /**
     * @brief Access the 15-length section of the dictionary.
     * @param i word index
     * @return a pointer to the requested word in the 15-length
     *   dictionary section
     */
    unsigned char const* bdict_access15(unsigned int i) noexcept;

    /**
     * @brief Access the 16-length section of the dictionary.
     * @param i word index
     * @return a pointer to the requested word in the 16-length
     *   dictionary section
     */
    unsigned char const* bdict_access16(unsigned int i) noexcept;

    /**
     * @brief Access the 17-length section of the dictionary.
     * @param i word index
     * @return a pointer to the requested word in the 17-length
     *   dictionary section
     */
    unsigned char const* bdict_access17(unsigned int i) noexcept;

    /**
     * @brief Access the 18-length section of the dictionary.
     * @param i word index
     * @return a pointer to the requested word in the 18-length
     *   dictionary section
     */
    unsigned char const* bdict_access18(unsigned int i) noexcept;

    /**
     * @brief Access the 19-length section of the dictionary.
     * @param i word index
     * @return a pointer to the requested word in the 19-length
     *   dictionary section
     */
    unsigned char const* bdict_access19(unsigned int i) noexcept;

    /**
     * @brief Access the 20-length section of the dictionary.
     * @param i word index
     * @return a pointer to the requested word in the 20-length
     *   dictionary section
     */
    unsigned char const* bdict_access20(unsigned int i) noexcept;

    /**
     * @brief Access the 21-length section of the dictionary.
     * @param i word index
     * @return a pointer to the requested word in the 21-length
     *   dictionary section
     */
    unsigned char const* bdict_access21(unsigned int i) noexcept;

    /**
     * @brief Access the 22-length section of the dictionary.
     * @param i word index
     * @return a pointer to the requested word in the 22-length
     *   dictionary section
     */
    unsigned char const* bdict_access22(unsigned int i) noexcept;

    /**
     * @brief Access the 23-length section of the dictionary.
     * @param i word index
     * @return a pointer to the requested word in the 23-length
     *   dictionary section
     */
    unsigned char const* bdict_access23(unsigned int i) noexcept;

    /**
     * @brief Access the 24-length section of the dictionary.
     * @param i word index
     * @return a pointer to the requested word in the 24-length
     *   dictionary section
     */
    unsigned char const* bdict_access24(unsigned int i) noexcept;
    //END   built-in dictionary / access
  };
};

#endif /*hg_TextComplexAccessP_BDict_pH_*/
