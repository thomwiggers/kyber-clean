#include "consts.h"
#include "params.h"
#include "rejsample.h"

#include <immintrin.h>
#include <stdint.h>


static const uint8_t idx[256][8] = {
    { 0,  0,  0,  0,  0,  0,  0,  0},
    { 0,  0,  0,  0,  0,  0,  0,  0},
    { 2,  0,  0,  0,  0,  0,  0,  0},
    { 0,  2,  0,  0,  0,  0,  0,  0},
    { 4,  0,  0,  0,  0,  0,  0,  0},
    { 0,  4,  0,  0,  0,  0,  0,  0},
    { 2,  4,  0,  0,  0,  0,  0,  0},
    { 0,  2,  4,  0,  0,  0,  0,  0},
    { 6,  0,  0,  0,  0,  0,  0,  0},
    { 0,  6,  0,  0,  0,  0,  0,  0},
    { 2,  6,  0,  0,  0,  0,  0,  0},
    { 0,  2,  6,  0,  0,  0,  0,  0},
    { 4,  6,  0,  0,  0,  0,  0,  0},
    { 0,  4,  6,  0,  0,  0,  0,  0},
    { 2,  4,  6,  0,  0,  0,  0,  0},
    { 0,  2,  4,  6,  0,  0,  0,  0},
    { 8,  0,  0,  0,  0,  0,  0,  0},
    { 0,  8,  0,  0,  0,  0,  0,  0},
    { 2,  8,  0,  0,  0,  0,  0,  0},
    { 0,  2,  8,  0,  0,  0,  0,  0},
    { 4,  8,  0,  0,  0,  0,  0,  0},
    { 0,  4,  8,  0,  0,  0,  0,  0},
    { 2,  4,  8,  0,  0,  0,  0,  0},
    { 0,  2,  4,  8,  0,  0,  0,  0},
    { 6,  8,  0,  0,  0,  0,  0,  0},
    { 0,  6,  8,  0,  0,  0,  0,  0},
    { 2,  6,  8,  0,  0,  0,  0,  0},
    { 0,  2,  6,  8,  0,  0,  0,  0},
    { 4,  6,  8,  0,  0,  0,  0,  0},
    { 0,  4,  6,  8,  0,  0,  0,  0},
    { 2,  4,  6,  8,  0,  0,  0,  0},
    { 0,  2,  4,  6,  8,  0,  0,  0},
    {10,  0,  0,  0,  0,  0,  0,  0},
    { 0, 10,  0,  0,  0,  0,  0,  0},
    { 2, 10,  0,  0,  0,  0,  0,  0},
    { 0,  2, 10,  0,  0,  0,  0,  0},
    { 4, 10,  0,  0,  0,  0,  0,  0},
    { 0,  4, 10,  0,  0,  0,  0,  0},
    { 2,  4, 10,  0,  0,  0,  0,  0},
    { 0,  2,  4, 10,  0,  0,  0,  0},
    { 6, 10,  0,  0,  0,  0,  0,  0},
    { 0,  6, 10,  0,  0,  0,  0,  0},
    { 2,  6, 10,  0,  0,  0,  0,  0},
    { 0,  2,  6, 10,  0,  0,  0,  0},
    { 4,  6, 10,  0,  0,  0,  0,  0},
    { 0,  4,  6, 10,  0,  0,  0,  0},
    { 2,  4,  6, 10,  0,  0,  0,  0},
    { 0,  2,  4,  6, 10,  0,  0,  0},
    { 8, 10,  0,  0,  0,  0,  0,  0},
    { 0,  8, 10,  0,  0,  0,  0,  0},
    { 2,  8, 10,  0,  0,  0,  0,  0},
    { 0,  2,  8, 10,  0,  0,  0,  0},
    { 4,  8, 10,  0,  0,  0,  0,  0},
    { 0,  4,  8, 10,  0,  0,  0,  0},
    { 2,  4,  8, 10,  0,  0,  0,  0},
    { 0,  2,  4,  8, 10,  0,  0,  0},
    { 6,  8, 10,  0,  0,  0,  0,  0},
    { 0,  6,  8, 10,  0,  0,  0,  0},
    { 2,  6,  8, 10,  0,  0,  0,  0},
    { 0,  2,  6,  8, 10,  0,  0,  0},
    { 4,  6,  8, 10,  0,  0,  0,  0},
    { 0,  4,  6,  8, 10,  0,  0,  0},
    { 2,  4,  6,  8, 10,  0,  0,  0},
    { 0,  2,  4,  6,  8, 10,  0,  0},
    {12,  0,  0,  0,  0,  0,  0,  0},
    { 0, 12,  0,  0,  0,  0,  0,  0},
    { 2, 12,  0,  0,  0,  0,  0,  0},
    { 0,  2, 12,  0,  0,  0,  0,  0},
    { 4, 12,  0,  0,  0,  0,  0,  0},
    { 0,  4, 12,  0,  0,  0,  0,  0},
    { 2,  4, 12,  0,  0,  0,  0,  0},
    { 0,  2,  4, 12,  0,  0,  0,  0},
    { 6, 12,  0,  0,  0,  0,  0,  0},
    { 0,  6, 12,  0,  0,  0,  0,  0},
    { 2,  6, 12,  0,  0,  0,  0,  0},
    { 0,  2,  6, 12,  0,  0,  0,  0},
    { 4,  6, 12,  0,  0,  0,  0,  0},
    { 0,  4,  6, 12,  0,  0,  0,  0},
    { 2,  4,  6, 12,  0,  0,  0,  0},
    { 0,  2,  4,  6, 12,  0,  0,  0},
    { 8, 12,  0,  0,  0,  0,  0,  0},
    { 0,  8, 12,  0,  0,  0,  0,  0},
    { 2,  8, 12,  0,  0,  0,  0,  0},
    { 0,  2,  8, 12,  0,  0,  0,  0},
    { 4,  8, 12,  0,  0,  0,  0,  0},
    { 0,  4,  8, 12,  0,  0,  0,  0},
    { 2,  4,  8, 12,  0,  0,  0,  0},
    { 0,  2,  4,  8, 12,  0,  0,  0},
    { 6,  8, 12,  0,  0,  0,  0,  0},
    { 0,  6,  8, 12,  0,  0,  0,  0},
    { 2,  6,  8, 12,  0,  0,  0,  0},
    { 0,  2,  6,  8, 12,  0,  0,  0},
    { 4,  6,  8, 12,  0,  0,  0,  0},
    { 0,  4,  6,  8, 12,  0,  0,  0},
    { 2,  4,  6,  8, 12,  0,  0,  0},
    { 0,  2,  4,  6,  8, 12,  0,  0},
    {10, 12,  0,  0,  0,  0,  0,  0},
    { 0, 10, 12,  0,  0,  0,  0,  0},
    { 2, 10, 12,  0,  0,  0,  0,  0},
    { 0,  2, 10, 12,  0,  0,  0,  0},
    { 4, 10, 12,  0,  0,  0,  0,  0},
    { 0,  4, 10, 12,  0,  0,  0,  0},
    { 2,  4, 10, 12,  0,  0,  0,  0},
    { 0,  2,  4, 10, 12,  0,  0,  0},
    { 6, 10, 12,  0,  0,  0,  0,  0},
    { 0,  6, 10, 12,  0,  0,  0,  0},
    { 2,  6, 10, 12,  0,  0,  0,  0},
    { 0,  2,  6, 10, 12,  0,  0,  0},
    { 4,  6, 10, 12,  0,  0,  0,  0},
    { 0,  4,  6, 10, 12,  0,  0,  0},
    { 2,  4,  6, 10, 12,  0,  0,  0},
    { 0,  2,  4,  6, 10, 12,  0,  0},
    { 8, 10, 12,  0,  0,  0,  0,  0},
    { 0,  8, 10, 12,  0,  0,  0,  0},
    { 2,  8, 10, 12,  0,  0,  0,  0},
    { 0,  2,  8, 10, 12,  0,  0,  0},
    { 4,  8, 10, 12,  0,  0,  0,  0},
    { 0,  4,  8, 10, 12,  0,  0,  0},
    { 2,  4,  8, 10, 12,  0,  0,  0},
    { 0,  2,  4,  8, 10, 12,  0,  0},
    { 6,  8, 10, 12,  0,  0,  0,  0},
    { 0,  6,  8, 10, 12,  0,  0,  0},
    { 2,  6,  8, 10, 12,  0,  0,  0},
    { 0,  2,  6,  8, 10, 12,  0,  0},
    { 4,  6,  8, 10, 12,  0,  0,  0},
    { 0,  4,  6,  8, 10, 12,  0,  0},
    { 2,  4,  6,  8, 10, 12,  0,  0},
    { 0,  2,  4,  6,  8, 10, 12,  0},
    {14,  0,  0,  0,  0,  0,  0,  0},
    { 0, 14,  0,  0,  0,  0,  0,  0},
    { 2, 14,  0,  0,  0,  0,  0,  0},
    { 0,  2, 14,  0,  0,  0,  0,  0},
    { 4, 14,  0,  0,  0,  0,  0,  0},
    { 0,  4, 14,  0,  0,  0,  0,  0},
    { 2,  4, 14,  0,  0,  0,  0,  0},
    { 0,  2,  4, 14,  0,  0,  0,  0},
    { 6, 14,  0,  0,  0,  0,  0,  0},
    { 0,  6, 14,  0,  0,  0,  0,  0},
    { 2,  6, 14,  0,  0,  0,  0,  0},
    { 0,  2,  6, 14,  0,  0,  0,  0},
    { 4,  6, 14,  0,  0,  0,  0,  0},
    { 0,  4,  6, 14,  0,  0,  0,  0},
    { 2,  4,  6, 14,  0,  0,  0,  0},
    { 0,  2,  4,  6, 14,  0,  0,  0},
    { 8, 14,  0,  0,  0,  0,  0,  0},
    { 0,  8, 14,  0,  0,  0,  0,  0},
    { 2,  8, 14,  0,  0,  0,  0,  0},
    { 0,  2,  8, 14,  0,  0,  0,  0},
    { 4,  8, 14,  0,  0,  0,  0,  0},
    { 0,  4,  8, 14,  0,  0,  0,  0},
    { 2,  4,  8, 14,  0,  0,  0,  0},
    { 0,  2,  4,  8, 14,  0,  0,  0},
    { 6,  8, 14,  0,  0,  0,  0,  0},
    { 0,  6,  8, 14,  0,  0,  0,  0},
    { 2,  6,  8, 14,  0,  0,  0,  0},
    { 0,  2,  6,  8, 14,  0,  0,  0},
    { 4,  6,  8, 14,  0,  0,  0,  0},
    { 0,  4,  6,  8, 14,  0,  0,  0},
    { 2,  4,  6,  8, 14,  0,  0,  0},
    { 0,  2,  4,  6,  8, 14,  0,  0},
    {10, 14,  0,  0,  0,  0,  0,  0},
    { 0, 10, 14,  0,  0,  0,  0,  0},
    { 2, 10, 14,  0,  0,  0,  0,  0},
    { 0,  2, 10, 14,  0,  0,  0,  0},
    { 4, 10, 14,  0,  0,  0,  0,  0},
    { 0,  4, 10, 14,  0,  0,  0,  0},
    { 2,  4, 10, 14,  0,  0,  0,  0},
    { 0,  2,  4, 10, 14,  0,  0,  0},
    { 6, 10, 14,  0,  0,  0,  0,  0},
    { 0,  6, 10, 14,  0,  0,  0,  0},
    { 2,  6, 10, 14,  0,  0,  0,  0},
    { 0,  2,  6, 10, 14,  0,  0,  0},
    { 4,  6, 10, 14,  0,  0,  0,  0},
    { 0,  4,  6, 10, 14,  0,  0,  0},
    { 2,  4,  6, 10, 14,  0,  0,  0},
    { 0,  2,  4,  6, 10, 14,  0,  0},
    { 8, 10, 14,  0,  0,  0,  0,  0},
    { 0,  8, 10, 14,  0,  0,  0,  0},
    { 2,  8, 10, 14,  0,  0,  0,  0},
    { 0,  2,  8, 10, 14,  0,  0,  0},
    { 4,  8, 10, 14,  0,  0,  0,  0},
    { 0,  4,  8, 10, 14,  0,  0,  0},
    { 2,  4,  8, 10, 14,  0,  0,  0},
    { 0,  2,  4,  8, 10, 14,  0,  0},
    { 6,  8, 10, 14,  0,  0,  0,  0},
    { 0,  6,  8, 10, 14,  0,  0,  0},
    { 2,  6,  8, 10, 14,  0,  0,  0},
    { 0,  2,  6,  8, 10, 14,  0,  0},
    { 4,  6,  8, 10, 14,  0,  0,  0},
    { 0,  4,  6,  8, 10, 14,  0,  0},
    { 2,  4,  6,  8, 10, 14,  0,  0},
    { 0,  2,  4,  6,  8, 10, 14,  0},
    {12, 14,  0,  0,  0,  0,  0,  0},
    { 0, 12, 14,  0,  0,  0,  0,  0},
    { 2, 12, 14,  0,  0,  0,  0,  0},
    { 0,  2, 12, 14,  0,  0,  0,  0},
    { 4, 12, 14,  0,  0,  0,  0,  0},
    { 0,  4, 12, 14,  0,  0,  0,  0},
    { 2,  4, 12, 14,  0,  0,  0,  0},
    { 0,  2,  4, 12, 14,  0,  0,  0},
    { 6, 12, 14,  0,  0,  0,  0,  0},
    { 0,  6, 12, 14,  0,  0,  0,  0},
    { 2,  6, 12, 14,  0,  0,  0,  0},
    { 0,  2,  6, 12, 14,  0,  0,  0},
    { 4,  6, 12, 14,  0,  0,  0,  0},
    { 0,  4,  6, 12, 14,  0,  0,  0},
    { 2,  4,  6, 12, 14,  0,  0,  0},
    { 0,  2,  4,  6, 12, 14,  0,  0},
    { 8, 12, 14,  0,  0,  0,  0,  0},
    { 0,  8, 12, 14,  0,  0,  0,  0},
    { 2,  8, 12, 14,  0,  0,  0,  0},
    { 0,  2,  8, 12, 14,  0,  0,  0},
    { 4,  8, 12, 14,  0,  0,  0,  0},
    { 0,  4,  8, 12, 14,  0,  0,  0},
    { 2,  4,  8, 12, 14,  0,  0,  0},
    { 0,  2,  4,  8, 12, 14,  0,  0},
    { 6,  8, 12, 14,  0,  0,  0,  0},
    { 0,  6,  8, 12, 14,  0,  0,  0},
    { 2,  6,  8, 12, 14,  0,  0,  0},
    { 0,  2,  6,  8, 12, 14,  0,  0},
    { 4,  6,  8, 12, 14,  0,  0,  0},
    { 0,  4,  6,  8, 12, 14,  0,  0},
    { 2,  4,  6,  8, 12, 14,  0,  0},
    { 0,  2,  4,  6,  8, 12, 14,  0},
    {10, 12, 14,  0,  0,  0,  0,  0},
    { 0, 10, 12, 14,  0,  0,  0,  0},
    { 2, 10, 12, 14,  0,  0,  0,  0},
    { 0,  2, 10, 12, 14,  0,  0,  0},
    { 4, 10, 12, 14,  0,  0,  0,  0},
    { 0,  4, 10, 12, 14,  0,  0,  0},
    { 2,  4, 10, 12, 14,  0,  0,  0},
    { 0,  2,  4, 10, 12, 14,  0,  0},
    { 6, 10, 12, 14,  0,  0,  0,  0},
    { 0,  6, 10, 12, 14,  0,  0,  0},
    { 2,  6, 10, 12, 14,  0,  0,  0},
    { 0,  2,  6, 10, 12, 14,  0,  0},
    { 4,  6, 10, 12, 14,  0,  0,  0},
    { 0,  4,  6, 10, 12, 14,  0,  0},
    { 2,  4,  6, 10, 12, 14,  0,  0},
    { 0,  2,  4,  6, 10, 12, 14,  0},
    { 8, 10, 12, 14,  0,  0,  0,  0},
    { 0,  8, 10, 12, 14,  0,  0,  0},
    { 2,  8, 10, 12, 14,  0,  0,  0},
    { 0,  2,  8, 10, 12, 14,  0,  0},
    { 4,  8, 10, 12, 14,  0,  0,  0},
    { 0,  4,  8, 10, 12, 14,  0,  0},
    { 2,  4,  8, 10, 12, 14,  0,  0},
    { 0,  2,  4,  8, 10, 12, 14,  0},
    { 6,  8, 10, 12, 14,  0,  0,  0},
    { 0,  6,  8, 10, 12, 14,  0,  0},
    { 2,  6,  8, 10, 12, 14,  0,  0},
    { 0,  2,  6,  8, 10, 12, 14,  0},
    { 4,  6,  8, 10, 12, 14,  0,  0},
    { 0,  4,  6,  8, 10, 12, 14,  0},
    { 2,  4,  6,  8, 10, 12, 14,  0},
    { 0,  2,  4,  6,  8, 10, 12, 14}
};

#define _mm256_cmpge_epu16(a, b)  _mm256_cmpeq_epi16(_mm256_max_epu16(a, b), a)
#define _mm_cmpge_epu16(a, b)  _mm_cmpeq_epi16(_mm_max_epu16(a, b), a)

size_t PQCLEAN_NAMESPACE_rej_uniform(int16_t *r,
        size_t len,
        const uint8_t *buf,
        size_t buflen) {
    size_t ctr, pos;
    uint16_t val;
    uint32_t good0, good1, good2;
    const __m256i bound  = _mm256_set1_epi16((int16_t)(19 * KYBER_Q - 1)); // -1 to use cheaper >= instead of > comparison
    const __m256i ones   = _mm256_set1_epi8(1);
    const __m256i kyberq = _mm256_load_si256(&PQCLEAN_NAMESPACE_16xq.as_vec);
    const __m256i v = _mm256_load_si256(&PQCLEAN_NAMESPACE_16xv.as_vec);
    __m256i d0, d1, d2, tmp0, tmp1, tmp2, pi0, pi1, pi2;
    __m128i d, tmp, pilo, pihi;

    ctr = pos = 0;
    while (ctr + 48 <= len && pos + 96 <= buflen) {
        d0 = _mm256_loadu_si256((__m256i *)&buf[pos + 0]);
        d1 = _mm256_loadu_si256((__m256i *)&buf[pos + 32]);
        d2 = _mm256_loadu_si256((__m256i *)&buf[pos + 64]);

        tmp0 = _mm256_cmpge_epu16(bound, d0);
        tmp1 = _mm256_cmpge_epu16(bound, d1);
        tmp2 = _mm256_cmpge_epu16(bound, d2);
        good0 = (uint32_t)_mm256_movemask_epi8(tmp0);
        good1 = (uint32_t)_mm256_movemask_epi8(tmp1);
        good2 = (uint32_t)_mm256_movemask_epi8(tmp2);
        good0 = _pext_u32(good0, 0x55555555);
        good1 = _pext_u32(good1, 0x55555555);
        good2 = _pext_u32(good2, 0x55555555);

        pilo = _mm_loadl_epi64((__m128i *)&idx[good0 & 0xFF]);
        pihi = _mm_loadl_epi64((__m128i *)&idx[(good0 >> 8) & 0xFF]);
        pi0 = _mm256_castsi128_si256(pilo);
        pi0 = _mm256_inserti128_si256(pi0, pihi, 1);

        pilo = _mm_loadl_epi64((__m128i *)&idx[good1 & 0xFF]);
        pihi = _mm_loadl_epi64((__m128i *)&idx[(good1 >> 8) & 0xFF]);
        pi1 = _mm256_castsi128_si256(pilo);
        pi1 = _mm256_inserti128_si256(pi1, pihi, 1);

        pilo = _mm_loadl_epi64((__m128i *)&idx[good2 & 0xFF]);
        pihi = _mm_loadl_epi64((__m128i *)&idx[(good2 >> 8) & 0xFF]);
        pi2 = _mm256_castsi128_si256(pilo);
        pi2 = _mm256_inserti128_si256(pi2, pihi, 1);

        tmp0 = _mm256_add_epi8(pi0, ones);
        tmp1 = _mm256_add_epi8(pi1, ones);
        tmp2 = _mm256_add_epi8(pi2, ones);
        pi0 = _mm256_unpacklo_epi8(pi0, tmp0);
        pi1 = _mm256_unpacklo_epi8(pi1, tmp1);
        pi2 = _mm256_unpacklo_epi8(pi2, tmp2);

        d0 = _mm256_shuffle_epi8(d0, pi0);
        d1 = _mm256_shuffle_epi8(d1, pi1);
        d2 = _mm256_shuffle_epi8(d2, pi2);

        /* Barrett reduction of (still unsigned) d values */
        tmp0 = _mm256_mulhi_epu16(d0, v);
        tmp1 = _mm256_mulhi_epu16(d1, v);
        tmp2 = _mm256_mulhi_epu16(d2, v);
        tmp0 = _mm256_srli_epi16(tmp0, 10);
        tmp1 = _mm256_srli_epi16(tmp1, 10);
        tmp2 = _mm256_srli_epi16(tmp2, 10);
        tmp0 = _mm256_mullo_epi16(tmp0, kyberq);
        tmp1 = _mm256_mullo_epi16(tmp1, kyberq);
        tmp2 = _mm256_mullo_epi16(tmp2, kyberq);
        d0   = _mm256_sub_epi16(d0, tmp0);
        d1   = _mm256_sub_epi16(d1, tmp1);
        d2   = _mm256_sub_epi16(d2, tmp2);

        _mm_storeu_si128((__m128i *)&r[ctr], _mm256_castsi256_si128(d0));
        ctr += (unsigned int)_mm_popcnt_u32(good0 & 0xFF);
        _mm_storeu_si128((__m128i *)&r[ctr], _mm256_extracti128_si256(d0, 1));
        ctr += (unsigned int)_mm_popcnt_u32((good0 >> 8) & 0xFF);
        _mm_storeu_si128((__m128i *)&r[ctr], _mm256_castsi256_si128(d1));
        ctr += (unsigned int)_mm_popcnt_u32(good1 & 0xFF);
        _mm_storeu_si128((__m128i *)&r[ctr], _mm256_extracti128_si256(d1, 1));
        ctr += (unsigned int)_mm_popcnt_u32((good1 >> 8) & 0xFF);
        _mm_storeu_si128((__m128i *)&r[ctr], _mm256_castsi256_si128(d2));
        ctr += (unsigned int)_mm_popcnt_u32(good2 & 0xFF);
        _mm_storeu_si128((__m128i *)&r[ctr], _mm256_extracti128_si256(d2, 1));
        ctr += (unsigned int)_mm_popcnt_u32((good2 >> 8) & 0xFF);
        pos += 96;
    }

    while (ctr + 8 <= len && pos + 16 <= buflen) {
        d = _mm_loadu_si128((__m128i *)&buf[pos]);
        tmp = _mm_cmpge_epu16(_mm256_castsi256_si128(bound), d);
        good0 = (uint32_t)_mm_movemask_epi8(tmp);
        good0 = _pext_u32(good0, 0x55555555);
        pilo = _mm_loadl_epi64((__m128i *)&idx[good0]);
        pihi = _mm_add_epi8(pilo, _mm256_castsi256_si128(ones));
        pilo = _mm_unpacklo_epi8(pilo, pihi);
        d = _mm_shuffle_epi8(d, pilo);

        /* Barrett reduction */
        tmp = _mm_mulhi_epu16(d, _mm256_castsi256_si128(v));
        tmp = _mm_srli_epi16(tmp, 10);
        tmp = _mm_mullo_epi16(tmp, _mm256_castsi256_si128(kyberq));
        d   = _mm_sub_epi16(d, tmp);

        _mm_storeu_si128((__m128i *)&r[ctr], d);
        ctr += (unsigned int)_mm_popcnt_u32(good0);
        pos += 16;
    }

    while (ctr < len && pos + 2 <= buflen) {
        val = (uint16_t)(buf[pos] | ((uint16_t)buf[pos + 1] << 8));
        pos += 2;

        if (val < 19 * KYBER_Q) {
            val -= ((int32_t)val * 20159 >> 26) * KYBER_Q;
            r[ctr++] = (int16_t)val;
        }
    }

    return ctr;
}
