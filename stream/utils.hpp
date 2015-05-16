#include <cstdint>
#include <cstddef>

namespace stream
{
    template <typename Tp, Tp mask, int k>
    inline Tp swap_bits(Tp p)
    {
        Tp q = ((p>>k)^p) & mask;
        return p^q^(q<<k);
    }

    // Reversing bits in a 64-bit word.

    uint64_t reverse_bits(uint64_t n)
    {
        static constexpr uint64_t m0 = 0x5555555555555555LLU;
        static constexpr uint64_t m1 = 0x0300c0303030c303LLU;
        static constexpr uint64_t m2 = 0x00c0300c03f0003fLLU;
        static constexpr uint64_t m3 = 0x00000ffc00003fffLLU;

        n = ((n>>1)&m0) | (n&m0)<<1;
        n = swap_bits<uint64_t, m1, 4>(n);
        n = swap_bits<uint64_t, m2, 8>(n);
        n = swap_bits<uint64_t, m3, 20>(n);
        n = (n >> 34) | (n << 30);
        return n;
    }

    // Reversing bits in a word, basic interchange scheme.

    uint32_t reserve_bits(uint32_t x)
    {
        x = (x & 0x55555555) <<  1 | (x & 0xAAAAAAAA) >>  1;
        x = (x & 0x33333333) <<  2 | (x & 0xCCCCCCCC) >>  2;
        x = (x & 0x0F0F0F0F) <<  4 | (x & 0xF0F0F0F0) >>  4;
        x = (x & 0x00FF00FF) <<  8 | (x & 0xFF00FF00) >>  8;
        x = (x & 0x0000FFFF) << 16 | (x & 0xFFFF0000) >> 16;
        return x;
    }

    constexpr size_t
    rank(size_t value)
    {
        return (value == 0 ? 0 :
                value & 1  ? 1 :
                1 + rank(value >> 1));
    }

    constexpr size_t
    log2(size_t value)
    {
        return value == 1 ? 0 : 1 + log2(value >> 1);
    }
}
