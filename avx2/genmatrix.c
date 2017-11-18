#include <immintrin.h>
#include <string.h>
#include "genmatrix.h"
#include "polyvec.h"
#include "fips202.h"

#if 0

static const __m256i idx[256] = {
{0x800000008,0x800000008,0x800000008,0x800000008},
{0x800000000,0x800000008,0x800000008,0x800000008},
{0x800000001,0x800000008,0x800000008,0x800000008},
{0x100000000,0x800000008,0x800000008,0x800000008},
{0x800000002,0x800000008,0x800000008,0x800000008},
{0x200000000,0x800000008,0x800000008,0x800000008},
{0x200000001,0x800000008,0x800000008,0x800000008},
{0x100000000,0x800000002,0x800000008,0x800000008},
{0x800000003,0x800000008,0x800000008,0x800000008},
{0x300000000,0x800000008,0x800000008,0x800000008},
{0x300000001,0x800000008,0x800000008,0x800000008},
{0x100000000,0x800000003,0x800000008,0x800000008},
{0x300000002,0x800000008,0x800000008,0x800000008},
{0x200000000,0x800000003,0x800000008,0x800000008},
{0x200000001,0x800000003,0x800000008,0x800000008},
{0x100000000,0x300000002,0x800000008,0x800000008},
{0x800000004,0x800000008,0x800000008,0x800000008},
{0x400000000,0x800000008,0x800000008,0x800000008},
{0x400000001,0x800000008,0x800000008,0x800000008},
{0x100000000,0x800000004,0x800000008,0x800000008},
{0x400000002,0x800000008,0x800000008,0x800000008},
{0x200000000,0x800000004,0x800000008,0x800000008},
{0x200000001,0x800000004,0x800000008,0x800000008},
{0x100000000,0x400000002,0x800000008,0x800000008},
{0x400000003,0x800000008,0x800000008,0x800000008},
{0x300000000,0x800000004,0x800000008,0x800000008},
{0x300000001,0x800000004,0x800000008,0x800000008},
{0x100000000,0x400000003,0x800000008,0x800000008},
{0x300000002,0x800000004,0x800000008,0x800000008},
{0x200000000,0x400000003,0x800000008,0x800000008},
{0x200000001,0x400000003,0x800000008,0x800000008},
{0x100000000,0x300000002,0x800000004,0x800000008},
{0x800000005,0x800000008,0x800000008,0x800000008},
{0x500000000,0x800000008,0x800000008,0x800000008},
{0x500000001,0x800000008,0x800000008,0x800000008},
{0x100000000,0x800000005,0x800000008,0x800000008},
{0x500000002,0x800000008,0x800000008,0x800000008},
{0x200000000,0x800000005,0x800000008,0x800000008},
{0x200000001,0x800000005,0x800000008,0x800000008},
{0x100000000,0x500000002,0x800000008,0x800000008},
{0x500000003,0x800000008,0x800000008,0x800000008},
{0x300000000,0x800000005,0x800000008,0x800000008},
{0x300000001,0x800000005,0x800000008,0x800000008},
{0x100000000,0x500000003,0x800000008,0x800000008},
{0x300000002,0x800000005,0x800000008,0x800000008},
{0x200000000,0x500000003,0x800000008,0x800000008},
{0x200000001,0x500000003,0x800000008,0x800000008},
{0x100000000,0x300000002,0x800000005,0x800000008},
{0x500000004,0x800000008,0x800000008,0x800000008},
{0x400000000,0x800000005,0x800000008,0x800000008},
{0x400000001,0x800000005,0x800000008,0x800000008},
{0x100000000,0x500000004,0x800000008,0x800000008},
{0x400000002,0x800000005,0x800000008,0x800000008},
{0x200000000,0x500000004,0x800000008,0x800000008},
{0x200000001,0x500000004,0x800000008,0x800000008},
{0x100000000,0x400000002,0x800000005,0x800000008},
{0x400000003,0x800000005,0x800000008,0x800000008},
{0x300000000,0x500000004,0x800000008,0x800000008},
{0x300000001,0x500000004,0x800000008,0x800000008},
{0x100000000,0x400000003,0x800000005,0x800000008},
{0x300000002,0x500000004,0x800000008,0x800000008},
{0x200000000,0x400000003,0x800000005,0x800000008},
{0x200000001,0x400000003,0x800000005,0x800000008},
{0x100000000,0x300000002,0x500000004,0x800000008},
{0x800000006,0x800000008,0x800000008,0x800000008},
{0x600000000,0x800000008,0x800000008,0x800000008},
{0x600000001,0x800000008,0x800000008,0x800000008},
{0x100000000,0x800000006,0x800000008,0x800000008},
{0x600000002,0x800000008,0x800000008,0x800000008},
{0x200000000,0x800000006,0x800000008,0x800000008},
{0x200000001,0x800000006,0x800000008,0x800000008},
{0x100000000,0x600000002,0x800000008,0x800000008},
{0x600000003,0x800000008,0x800000008,0x800000008},
{0x300000000,0x800000006,0x800000008,0x800000008},
{0x300000001,0x800000006,0x800000008,0x800000008},
{0x100000000,0x600000003,0x800000008,0x800000008},
{0x300000002,0x800000006,0x800000008,0x800000008},
{0x200000000,0x600000003,0x800000008,0x800000008},
{0x200000001,0x600000003,0x800000008,0x800000008},
{0x100000000,0x300000002,0x800000006,0x800000008},
{0x600000004,0x800000008,0x800000008,0x800000008},
{0x400000000,0x800000006,0x800000008,0x800000008},
{0x400000001,0x800000006,0x800000008,0x800000008},
{0x100000000,0x600000004,0x800000008,0x800000008},
{0x400000002,0x800000006,0x800000008,0x800000008},
{0x200000000,0x600000004,0x800000008,0x800000008},
{0x200000001,0x600000004,0x800000008,0x800000008},
{0x100000000,0x400000002,0x800000006,0x800000008},
{0x400000003,0x800000006,0x800000008,0x800000008},
{0x300000000,0x600000004,0x800000008,0x800000008},
{0x300000001,0x600000004,0x800000008,0x800000008},
{0x100000000,0x400000003,0x800000006,0x800000008},
{0x300000002,0x600000004,0x800000008,0x800000008},
{0x200000000,0x400000003,0x800000006,0x800000008},
{0x200000001,0x400000003,0x800000006,0x800000008},
{0x100000000,0x300000002,0x600000004,0x800000008},
{0x600000005,0x800000008,0x800000008,0x800000008},
{0x500000000,0x800000006,0x800000008,0x800000008},
{0x500000001,0x800000006,0x800000008,0x800000008},
{0x100000000,0x600000005,0x800000008,0x800000008},
{0x500000002,0x800000006,0x800000008,0x800000008},
{0x200000000,0x600000005,0x800000008,0x800000008},
{0x200000001,0x600000005,0x800000008,0x800000008},
{0x100000000,0x500000002,0x800000006,0x800000008},
{0x500000003,0x800000006,0x800000008,0x800000008},
{0x300000000,0x600000005,0x800000008,0x800000008},
{0x300000001,0x600000005,0x800000008,0x800000008},
{0x100000000,0x500000003,0x800000006,0x800000008},
{0x300000002,0x600000005,0x800000008,0x800000008},
{0x200000000,0x500000003,0x800000006,0x800000008},
{0x200000001,0x500000003,0x800000006,0x800000008},
{0x100000000,0x300000002,0x600000005,0x800000008},
{0x500000004,0x800000006,0x800000008,0x800000008},
{0x400000000,0x600000005,0x800000008,0x800000008},
{0x400000001,0x600000005,0x800000008,0x800000008},
{0x100000000,0x500000004,0x800000006,0x800000008},
{0x400000002,0x600000005,0x800000008,0x800000008},
{0x200000000,0x500000004,0x800000006,0x800000008},
{0x200000001,0x500000004,0x800000006,0x800000008},
{0x100000000,0x400000002,0x600000005,0x800000008},
{0x400000003,0x600000005,0x800000008,0x800000008},
{0x300000000,0x500000004,0x800000006,0x800000008},
{0x300000001,0x500000004,0x800000006,0x800000008},
{0x100000000,0x400000003,0x600000005,0x800000008},
{0x300000002,0x500000004,0x800000006,0x800000008},
{0x200000000,0x400000003,0x600000005,0x800000008},
{0x200000001,0x400000003,0x600000005,0x800000008},
{0x100000000,0x300000002,0x500000004,0x800000006},
{0x800000007,0x800000008,0x800000008,0x800000008},
{0x700000000,0x800000008,0x800000008,0x800000008},
{0x700000001,0x800000008,0x800000008,0x800000008},
{0x100000000,0x800000007,0x800000008,0x800000008},
{0x700000002,0x800000008,0x800000008,0x800000008},
{0x200000000,0x800000007,0x800000008,0x800000008},
{0x200000001,0x800000007,0x800000008,0x800000008},
{0x100000000,0x700000002,0x800000008,0x800000008},
{0x700000003,0x800000008,0x800000008,0x800000008},
{0x300000000,0x800000007,0x800000008,0x800000008},
{0x300000001,0x800000007,0x800000008,0x800000008},
{0x100000000,0x700000003,0x800000008,0x800000008},
{0x300000002,0x800000007,0x800000008,0x800000008},
{0x200000000,0x700000003,0x800000008,0x800000008},
{0x200000001,0x700000003,0x800000008,0x800000008},
{0x100000000,0x300000002,0x800000007,0x800000008},
{0x700000004,0x800000008,0x800000008,0x800000008},
{0x400000000,0x800000007,0x800000008,0x800000008},
{0x400000001,0x800000007,0x800000008,0x800000008},
{0x100000000,0x700000004,0x800000008,0x800000008},
{0x400000002,0x800000007,0x800000008,0x800000008},
{0x200000000,0x700000004,0x800000008,0x800000008},
{0x200000001,0x700000004,0x800000008,0x800000008},
{0x100000000,0x400000002,0x800000007,0x800000008},
{0x400000003,0x800000007,0x800000008,0x800000008},
{0x300000000,0x700000004,0x800000008,0x800000008},
{0x300000001,0x700000004,0x800000008,0x800000008},
{0x100000000,0x400000003,0x800000007,0x800000008},
{0x300000002,0x700000004,0x800000008,0x800000008},
{0x200000000,0x400000003,0x800000007,0x800000008},
{0x200000001,0x400000003,0x800000007,0x800000008},
{0x100000000,0x300000002,0x700000004,0x800000008},
{0x700000005,0x800000008,0x800000008,0x800000008},
{0x500000000,0x800000007,0x800000008,0x800000008},
{0x500000001,0x800000007,0x800000008,0x800000008},
{0x100000000,0x700000005,0x800000008,0x800000008},
{0x500000002,0x800000007,0x800000008,0x800000008},
{0x200000000,0x700000005,0x800000008,0x800000008},
{0x200000001,0x700000005,0x800000008,0x800000008},
{0x100000000,0x500000002,0x800000007,0x800000008},
{0x500000003,0x800000007,0x800000008,0x800000008},
{0x300000000,0x700000005,0x800000008,0x800000008},
{0x300000001,0x700000005,0x800000008,0x800000008},
{0x100000000,0x500000003,0x800000007,0x800000008},
{0x300000002,0x700000005,0x800000008,0x800000008},
{0x200000000,0x500000003,0x800000007,0x800000008},
{0x200000001,0x500000003,0x800000007,0x800000008},
{0x100000000,0x300000002,0x700000005,0x800000008},
{0x500000004,0x800000007,0x800000008,0x800000008},
{0x400000000,0x700000005,0x800000008,0x800000008},
{0x400000001,0x700000005,0x800000008,0x800000008},
{0x100000000,0x500000004,0x800000007,0x800000008},
{0x400000002,0x700000005,0x800000008,0x800000008},
{0x200000000,0x500000004,0x800000007,0x800000008},
{0x200000001,0x500000004,0x800000007,0x800000008},
{0x100000000,0x400000002,0x700000005,0x800000008},
{0x400000003,0x700000005,0x800000008,0x800000008},
{0x300000000,0x500000004,0x800000007,0x800000008},
{0x300000001,0x500000004,0x800000007,0x800000008},
{0x100000000,0x400000003,0x700000005,0x800000008},
{0x300000002,0x500000004,0x800000007,0x800000008},
{0x200000000,0x400000003,0x700000005,0x800000008},
{0x200000001,0x400000003,0x700000005,0x800000008},
{0x100000000,0x300000002,0x500000004,0x800000007},
{0x700000006,0x800000008,0x800000008,0x800000008},
{0x600000000,0x800000007,0x800000008,0x800000008},
{0x600000001,0x800000007,0x800000008,0x800000008},
{0x100000000,0x700000006,0x800000008,0x800000008},
{0x600000002,0x800000007,0x800000008,0x800000008},
{0x200000000,0x700000006,0x800000008,0x800000008},
{0x200000001,0x700000006,0x800000008,0x800000008},
{0x100000000,0x600000002,0x800000007,0x800000008},
{0x600000003,0x800000007,0x800000008,0x800000008},
{0x300000000,0x700000006,0x800000008,0x800000008},
{0x300000001,0x700000006,0x800000008,0x800000008},
{0x100000000,0x600000003,0x800000007,0x800000008},
{0x300000002,0x700000006,0x800000008,0x800000008},
{0x200000000,0x600000003,0x800000007,0x800000008},
{0x200000001,0x600000003,0x800000007,0x800000008},
{0x100000000,0x300000002,0x700000006,0x800000008},
{0x600000004,0x800000007,0x800000008,0x800000008},
{0x400000000,0x700000006,0x800000008,0x800000008},
{0x400000001,0x700000006,0x800000008,0x800000008},
{0x100000000,0x600000004,0x800000007,0x800000008},
{0x400000002,0x700000006,0x800000008,0x800000008},
{0x200000000,0x600000004,0x800000007,0x800000008},
{0x200000001,0x600000004,0x800000007,0x800000008},
{0x100000000,0x400000002,0x700000006,0x800000008},
{0x400000003,0x700000006,0x800000008,0x800000008},
{0x300000000,0x600000004,0x800000007,0x800000008},
{0x300000001,0x600000004,0x800000007,0x800000008},
{0x100000000,0x400000003,0x700000006,0x800000008},
{0x300000002,0x600000004,0x800000007,0x800000008},
{0x200000000,0x400000003,0x700000006,0x800000008},
{0x200000001,0x400000003,0x700000006,0x800000008},
{0x100000000,0x300000002,0x600000004,0x800000007},
{0x600000005,0x800000007,0x800000008,0x800000008},
{0x500000000,0x700000006,0x800000008,0x800000008},
{0x500000001,0x700000006,0x800000008,0x800000008},
{0x100000000,0x600000005,0x800000007,0x800000008},
{0x500000002,0x700000006,0x800000008,0x800000008},
{0x200000000,0x600000005,0x800000007,0x800000008},
{0x200000001,0x600000005,0x800000007,0x800000008},
{0x100000000,0x500000002,0x700000006,0x800000008},
{0x500000003,0x700000006,0x800000008,0x800000008},
{0x300000000,0x600000005,0x800000007,0x800000008},
{0x300000001,0x600000005,0x800000007,0x800000008},
{0x100000000,0x500000003,0x700000006,0x800000008},
{0x300000002,0x600000005,0x800000007,0x800000008},
{0x200000000,0x500000003,0x700000006,0x800000008},
{0x200000001,0x500000003,0x700000006,0x800000008},
{0x100000000,0x300000002,0x600000005,0x800000007},
{0x500000004,0x700000006,0x800000008,0x800000008},
{0x400000000,0x600000005,0x800000007,0x800000008},
{0x400000001,0x600000005,0x800000007,0x800000008},
{0x100000000,0x500000004,0x700000006,0x800000008},
{0x400000002,0x600000005,0x800000007,0x800000008},
{0x200000000,0x500000004,0x700000006,0x800000008},
{0x200000001,0x500000004,0x700000006,0x800000008},
{0x100000000,0x400000002,0x600000005,0x800000007},
{0x400000003,0x600000005,0x800000007,0x800000008},
{0x300000000,0x500000004,0x700000006,0x800000008},
{0x300000001,0x500000004,0x700000006,0x800000008},
{0x100000000,0x400000003,0x600000005,0x800000007},
{0x300000002,0x500000004,0x700000006,0x800000008},
{0x200000000,0x400000003,0x600000005,0x800000007},
{0x200000001,0x400000003,0x600000005,0x800000007},
{0x100000000,0x300000002,0x500000004,0x700000006},

};

#endif

/* Generate entry a_{i,j} of matrix A as Parse(SHAKE128(seed|i|j)) */
static void genmatrix_ref(polyvec *a, const unsigned char *seed, int transposed)
{
  unsigned int pos=0, ctr;
  uint16_t val;
  unsigned int nblocks=4;
  uint8_t buf[SHAKE128_RATE*nblocks];
  int i,j;
  uint64_t state[25]; // CSHAKE state
  unsigned char extseed[KYBER_SEEDBYTES+2];

  for(i=0;i<KYBER_SEEDBYTES;i++)
    extseed[i] = seed[i];


  for(i=0;i<KYBER_K;i++)
  {
    for(j=0;j<KYBER_K;j++)
    {
      ctr = pos = 0;
      if(transposed) 
      {
        extseed[KYBER_SEEDBYTES]   = i;
        extseed[KYBER_SEEDBYTES+1] = j;
      }
      else
      {
        extseed[KYBER_SEEDBYTES]   = j;
        extseed[KYBER_SEEDBYTES+1] = i;
      }
        
      shake128_absorb(state,extseed,KYBER_SEEDBYTES+2);
      shake128_squeezeblocks(buf,nblocks,state);

      while(ctr < KYBER_N)
      {
        val = (buf[pos] | ((uint16_t) buf[pos+1] << 8)) & 0x1fff;
        if(val < KYBER_Q)
        {
            a[i].vec[j].coeffs[ctr++] = val;
        }
        pos += 2;

        if(pos > SHAKE128_RATE*nblocks-2)
        {
          nblocks = 1;
          shake128_squeezeblocks(buf,nblocks,state);
          pos = 0;
        }
      }
    }
  }
}

#if 0

static int rej_sample(poly *r, const unsigned char *buf, size_t buflen)
{
  unsigned int pos=0, ctr = 0;
  uint16_t val;

  __m256i tmp0, tmp1, tmp2;
  __m128i d;
  uint32_t good = 0;
  uint32_t offset = 0;

  const __m256i modulus8    = _mm256_set1_epi32(KYBER_Q);
  const __m256i mask16      = _mm256_set1_epi16(0x1fff);

  while(ctr < KYBER_N-16)
  {
    tmp0 = _mm256_loadu_si256((__m256i *)&buf[pos]);
    tmp0 = _mm256_and_si256(tmp0, mask16); // eliminate top-3 bits

    // First work with the lower half of tmp0
    d = _mm256_extractf128_si256(tmp0, 0);
    tmp1 = _mm256_cvtepi16_epi32 (d);

    tmp2 = _mm256_cmpgt_epi32(modulus8, tmp1); // compare to modulus

    good = _mm256_movemask_ps((__m256)tmp2);

    offset = __builtin_popcount(good); // we get this many good (< modulus) values

    tmp2 = _mm256_permutevar8x32_epi32(tmp1, idx[good]); // permute good values to the bottom of tmp2

    _mm256_storeu_si256((__m256i *)&r->coeffs[ctr], tmp2);
    ctr += offset;

    // Now work with the higher half of tmp0
    d = _mm256_extractf128_si256(tmp0, 1);
    tmp1 = _mm256_cvtepi16_epi32 (d);

    tmp2 = _mm256_cmpgt_epi32(modulus8, tmp1); // compare to modulus

    good = _mm256_movemask_ps((__m256)tmp2);

    offset = __builtin_popcount(good); // we get this many good (< modulus) values

    tmp2 = _mm256_permutevar8x32_epi32(tmp1, idx[good]); // permute good values to the bottom of tmp2

    _mm256_storeu_si256((__m256i *)&r->coeffs[ctr], tmp2);
    ctr += offset;
    
    pos += 32;

    if(pos > buflen - 32)
      return -1;
  }

  while(ctr < KYBER_N)
  {
    val = (buf[pos] | ((uint16_t) buf[pos+1] << 8)) & 0x1fff;
    if(val < KYBER_Q)
    {
      r->coeffs[ctr++] = val;
    }
    pos += 2;
    if(pos>buflen-2)
      return -1;
  }
  return 0;
}


/* Generate entry a_{i,j} of matrix A as Parse(SHAKE128(seed|i|j)) */
void gen_matrix(polyvec *a, const unsigned char *seed, int transposed) // Not static for benchmarking in test/speed.c
{
#if (KYBER_K != 3)
#error "gen_matrix is specialized for KYBER_K == 3"
#endif
  unsigned int nblocks=4;
  uint8_t buf[KYBER_K][KYBER_K][SHAKE128_RATE*nblocks];
  int i,j;

  if(transposed)
  {
    shake128_simple4x(buf[0][0], buf[0][1], buf[0][2], buf[1][0],SHAKE128_RATE*nblocks,
                       0x0000, 0x0001, 0x0002, 0x0100,
                       seed,KYBER_SEEDBYTES);
    shake128_simple4x(buf[1][1], buf[1][2], buf[2][0], buf[2][1],SHAKE128_RATE*nblocks,
                       0x0101, 0x0102, 0x0200, 0x0201,
                       seed,KYBER_SEEDBYTES);
    shake128_simple(buf[2][2],SHAKE128_RATE*nblocks,0x0202,seed,KYBER_SEEDBYTES);
  }
  else
  {    
    cshake128_simple4x(buf[0][0], buf[0][1], buf[0][2], buf[1][0],SHAKE128_RATE*nblocks,
                       0x0000, 0x0100, 0x0200, 0x0001,
                       seed,KYBER_SEEDBYTES);
    cshake128_simple4x(buf[1][1], buf[1][2], buf[2][0], buf[2][1],SHAKE128_RATE*nblocks,
                       0x0101, 0x0201, 0x0002, 0x0102,
                       seed,KYBER_SEEDBYTES);
    cshake128_simple(buf[2][2],SHAKE128_RATE*nblocks,0x0202,seed,KYBER_SEEDBYTES);
  }
  

  for(i=0;i<KYBER_K;i++)
  {
    for(j=0;j<KYBER_K;j++)
    {
      if(rej_sample(&a[i].vec[j], buf[i][j], SHAKE128_RATE*nblocks))
        gen_matrix_ref(a, seed, transposed); // slower, but also extremely unlikely
    }
  }
}

#else

void genmatrix(polyvec *a, const unsigned char *seed, int transposed)
{
  genmatrix_ref(a, seed, transposed);
}

        
#endif

