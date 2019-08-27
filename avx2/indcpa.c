#include "cbd.h"
#include "indcpa.h"
#include "ntt.h"
#include "poly.h"
#include "polyvec.h"
#include "randombytes.h"
#include "rejsample.h"
#include "symmetric.h"

/*************************************************
* Name:        pack_pk
*
* Description: Serialize the public key as concatenation of the
*              compressed and serialized vector of polynomials pk
*              and the public seed used to generate the matrix A.
*
* Arguments:   uint8_t *r:          pointer to the output serialized public key
*              const poly *pk:            pointer to the input public-key polynomial
*              const uint8_t *seed: pointer to the input public seed
**************************************************/
static void pack_pk(uint8_t *r, polyvec *pk, const uint8_t *seed)
{
  int i;
  PQCLEAN_NAMESPACE_polyvec_tobytes(r, pk);
  for(i=0;i<KYBER_SYMBYTES;i++)
    r[i+KYBER_POLYVECBYTES] = seed[i];
}

/*************************************************
* Name:        unpack_pk
*
* Description: De-serialize and decompress public key from a byte array;
*              approximate inverse of pack_pk
*
* Arguments:   - polyvec *pk:                   pointer to output public-key vector of polynomials
*              - uint8_t *seed:           pointer to output seed to generate matrix A
*              - const uint8_t *packedpk: pointer to input serialized public key
**************************************************/
static void unpack_pk(polyvec *pk, uint8_t *seed, const uint8_t *packedpk)
{
  int i;
  PQCLEAN_NAMESPACE_polyvec_frombytes(pk, packedpk);
  for(i=0;i<KYBER_SYMBYTES;i++)
    seed[i] = packedpk[i+KYBER_POLYVECBYTES];
}

/*************************************************
* Name:        pack_sk
*
* Description: Serialize the secret key
*
* Arguments:   - uint8_t *r:  pointer to output serialized secret key
*              - const polyvec *sk: pointer to input vector of polynomials (secret key)
**************************************************/
static void pack_sk(uint8_t *r, polyvec *sk)
{
  PQCLEAN_NAMESPACE_polyvec_tobytes(r, sk);
}

/*************************************************
* Name:        unpack_sk
*
* Description: De-serialize the secret key;
*              inverse of pack_sk
*
* Arguments:   - polyvec *sk:                   pointer to output vector of polynomials (secret key)
*              - const uint8_t *packedsk: pointer to input serialized secret key
**************************************************/
static void unpack_sk(polyvec *sk, const uint8_t *packedsk)
{
  PQCLEAN_NAMESPACE_polyvec_frombytes(sk, packedsk);
}

/*************************************************
* Name:        pack_ciphertext
*
* Description: Serialize the ciphertext as concatenation of the
*              compressed and serialized vector of polynomials b
*              and the compressed and serialized polynomial v
*
* Arguments:   uint8_t *r:          pointer to the output serialized ciphertext
*              const poly *pk:            pointer to the input vector of polynomials b
*              const uint8_t *seed: pointer to the input polynomial v
**************************************************/
static void pack_ciphertext(uint8_t *r, polyvec *b, poly *v)
{
  PQCLEAN_NAMESPACE_polyvec_compress(r, b);
  PQCLEAN_NAMESPACE_poly_compress(r+KYBER_POLYVECCOMPRESSEDBYTES, v);
}

/*************************************************
* Name:        unpack_ciphertext
*
* Description: De-serialize and decompress ciphertext from a byte array;
*              approximate inverse of pack_ciphertext
*
* Arguments:   - polyvec *b:             pointer to the output vector of polynomials b
*              - poly *v:                pointer to the output polynomial v
*              - const uint8_t *c: pointer to the input serialized ciphertext
**************************************************/
static void unpack_ciphertext(polyvec *b, poly *v, const uint8_t *c)
{
  PQCLEAN_NAMESPACE_polyvec_decompress(b, c);
  PQCLEAN_NAMESPACE_poly_decompress(v, c+KYBER_POLYVECCOMPRESSEDBYTES);
}

static unsigned int rej_uniform_ref(int16_t *r, unsigned int len, const uint8_t *buf, unsigned int buflen)
{
  unsigned int ctr, pos;
  uint16_t val;

  ctr = pos = 0;
  while(ctr < len && pos + 2 <= buflen)
  {
    val = (uint16_t)(buf[pos] | ((uint16_t)buf[pos+1] << 8));
    pos += 2;

    if(val < 19*KYBER_Q)
    {
      val -= ((uint32_t)val*20159 >> 26)*KYBER_Q; // Barrett reduction
      r[ctr++] = (int16_t)val;
    }
  }

  return ctr;
}

#define gen_a(A,B)  gen_matrix(A,B,0)
#define gen_at(A,B) gen_matrix(A,B,1)

/*************************************************
* Name:        gen_matrix
*
* Description: Deterministically generate matrix A (or the transpose of A)
*              from a seed. Entries of the matrix are polynomials that look
*              uniformly random. Performs rejection sampling on output of
*              a XOF
*
* Arguments:   - polyvec *a:                pointer to ouptput matrix A
*              - const uint8_t *seed: pointer to input seed
*              - int transposed:            boolean deciding whether A or A^T is generated
**************************************************/
#define  GEN_MATRIX_MAXNBLOCKS ((530+XOF_BLOCKBYTES)/XOF_BLOCKBYTES)    /* 530 is expected number of required bytes */
#ifdef KYBER_90S
static void gen_matrix(polyvec *a, const uint8_t *seed, int transposed)
{
  unsigned int i, j, ctr;
  union {
      uint8_t x[XOF_BLOCKBYTES*GEN_MATRIX_MAXNBLOCKS];
      __m256i _dummy;
  } buf;
  aes256ctr_ctx state;

  PQCLEAN_NAMESPACE_aes256ctr_init(&state, seed, 0);

  for(i=0;i<KYBER_K;i++)
  {
    for(j=0;j<KYBER_K;j++)
    {
      if(transposed)
        PQCLEAN_NAMESPACE_aes256ctr_select(&state, (i << 8) + j);  // FIXME: Also in spec?
      else
        PQCLEAN_NAMESPACE_aes256ctr_select(&state, (j << 8) + i);

      PQCLEAN_NAMESPACE_aes256ctr_squeezeblocks(buf.x, GEN_MATRIX_MAXNBLOCKS, &state);
      ctr = PQCLEAN_NAMESPACE_rej_uniform(a[i].vec[j].coeffs, KYBER_N, buf.x, GEN_MATRIX_MAXNBLOCKS*XOF_BLOCKBYTES);

      while(ctr < KYBER_N)
      {
        PQCLEAN_NAMESPACE_aes256ctr_squeezeblocks(buf.x, 1, &state);
        ctr += rej_uniform_ref(a[i].vec[j].coeffs + ctr, KYBER_N - ctr, buf.x, XOF_BLOCKBYTES);
      }

      PQCLEAN_NAMESPACE_poly_nttunpack(&a[i].vec[j]);
    }
  }
}
#else
#if KYBER_K == 2
static void gen_matrix(polyvec *a, const uint8_t *seed, int transposed)
{
  unsigned int ctr0, ctr1, ctr2, ctr3, bufbytes;
  union {
      uint8_t x[4][XOF_BLOCKBYTES*GEN_MATRIX_MAXNBLOCKS];
      __m256i _dummy;
  } buf;
  keccak4x_state state;

  if(transposed)
    PQCLEAN_NAMESPACE_kyber_shake128x4_absorb(&state, seed, 0, 256, 1, 257);
  else
    PQCLEAN_NAMESPACE_kyber_shake128x4_absorb(&state, seed, 0, 1, 256, 257);

  PQCLEAN_NAMESPACE_shake128x4_squeezeblocks(buf.x[0], buf.x[1], buf.x[2], buf.x[3], GEN_MATRIX_MAXNBLOCKS, &state);
  bufbytes = GEN_MATRIX_MAXNBLOCKS*XOF_BLOCKBYTES;

  ctr0 = PQCLEAN_NAMESPACE_rej_uniform(a[0].vec[0].coeffs, KYBER_N, buf.x[0], bufbytes);
  ctr1 = PQCLEAN_NAMESPACE_rej_uniform(a[0].vec[1].coeffs, KYBER_N, buf.x[1], bufbytes);
  ctr2 = PQCLEAN_NAMESPACE_rej_uniform(a[1].vec[0].coeffs, KYBER_N, buf.x[2], bufbytes);
  ctr3 = PQCLEAN_NAMESPACE_rej_uniform(a[1].vec[1].coeffs, KYBER_N, buf.x[3], bufbytes);

  while(ctr0 < KYBER_N || ctr1 < KYBER_N || ctr2 < KYBER_N || ctr3 < KYBER_N)
  {
    PQCLEAN_NAMESPACE_shake128x4_squeezeblocks(buf.x[0], buf.x[1], buf.x[2], buf.x[3], 1, &state);
    bufbytes = XOF_BLOCKBYTES;

    ctr0 += rej_uniform_ref(a[0].vec[0].coeffs + ctr0, KYBER_N - ctr0, buf.x[0], bufbytes);
    ctr1 += rej_uniform_ref(a[0].vec[1].coeffs + ctr1, KYBER_N - ctr1, buf.x[1], bufbytes);
    ctr2 += rej_uniform_ref(a[1].vec[0].coeffs + ctr2, KYBER_N - ctr2, buf.x[2], bufbytes);
    ctr3 += rej_uniform_ref(a[1].vec[1].coeffs + ctr3, KYBER_N - ctr3, buf.x[3], bufbytes);
  }

  PQCLEAN_NAMESPACE_poly_nttunpack(&a[0].vec[0]);
  PQCLEAN_NAMESPACE_poly_nttunpack(&a[0].vec[1]);
  PQCLEAN_NAMESPACE_poly_nttunpack(&a[1].vec[0]);
  PQCLEAN_NAMESPACE_poly_nttunpack(&a[1].vec[1]);
}
#elif KYBER_K == 3
static void gen_matrix(polyvec *a, const uint8_t *seed, int transposed)
{
  unsigned int ctr0, ctr1, ctr2, ctr3, bufbytes;
  union {
      uint8_t x[4][XOF_BLOCKBYTES*GEN_MATRIX_MAXNBLOCKS];
      __m256i _dummy;
  } buf;
  keccak4x_state state;
  keccak_state state1x;

  if(transposed)
    PQCLEAN_NAMESPACE_kyber_shake128x4_absorb(&state, seed, 0, 256, 512, 1);
  else
    PQCLEAN_NAMESPACE_kyber_shake128x4_absorb(&state, seed, 0, 1, 2, 256);

  PQCLEAN_NAMESPACE_shake128x4_squeezeblocks(buf.x[0], buf.x[1], buf.x[2], buf.x[3], GEN_MATRIX_MAXNBLOCKS, &state);
  bufbytes = GEN_MATRIX_MAXNBLOCKS*XOF_BLOCKBYTES;

  ctr0 = PQCLEAN_NAMESPACE_rej_uniform(a[0].vec[0].coeffs, KYBER_N, buf.x[0], bufbytes);
  ctr1 = PQCLEAN_NAMESPACE_rej_uniform(a[0].vec[1].coeffs, KYBER_N, buf.x[1], bufbytes);
  ctr2 = PQCLEAN_NAMESPACE_rej_uniform(a[0].vec[2].coeffs, KYBER_N, buf.x[2], bufbytes);
  ctr3 = PQCLEAN_NAMESPACE_rej_uniform(a[1].vec[0].coeffs, KYBER_N, buf.x[3], bufbytes);

  while(ctr0 < KYBER_N || ctr1 < KYBER_N || ctr2 < KYBER_N || ctr3 < KYBER_N)
  {
    PQCLEAN_NAMESPACE_shake128x4_squeezeblocks(buf.x[0], buf.x[1], buf.x[2], buf.x[3], 1, &state);
    bufbytes = XOF_BLOCKBYTES;

    ctr0 += rej_uniform_ref(a[0].vec[0].coeffs + ctr0, KYBER_N - ctr0, buf.x[0], bufbytes);
    ctr1 += rej_uniform_ref(a[0].vec[1].coeffs + ctr1, KYBER_N - ctr1, buf.x[1], bufbytes);
    ctr2 += rej_uniform_ref(a[0].vec[2].coeffs + ctr2, KYBER_N - ctr2, buf.x[2], bufbytes);
    ctr3 += rej_uniform_ref(a[1].vec[0].coeffs + ctr3, KYBER_N - ctr3, buf.x[3], bufbytes);
  }

  PQCLEAN_NAMESPACE_poly_nttunpack(&a[0].vec[0]);
  PQCLEAN_NAMESPACE_poly_nttunpack(&a[0].vec[1]);
  PQCLEAN_NAMESPACE_poly_nttunpack(&a[0].vec[2]);
  PQCLEAN_NAMESPACE_poly_nttunpack(&a[1].vec[0]);

  if(transposed)
    PQCLEAN_NAMESPACE_kyber_shake128x4_absorb(&state, seed, 257, 513, 2, 258);
  else
    PQCLEAN_NAMESPACE_kyber_shake128x4_absorb(&state, seed, 257, 258, 512, 513);

  PQCLEAN_NAMESPACE_shake128x4_squeezeblocks(buf.x[0], buf.x[1], buf.x[2], buf.x[3], GEN_MATRIX_MAXNBLOCKS, &state);
  bufbytes = GEN_MATRIX_MAXNBLOCKS*XOF_BLOCKBYTES;

  ctr0 = PQCLEAN_NAMESPACE_rej_uniform(a[1].vec[1].coeffs, KYBER_N, buf.x[0], bufbytes);
  ctr1 = PQCLEAN_NAMESPACE_rej_uniform(a[1].vec[2].coeffs, KYBER_N, buf.x[1], bufbytes);
  ctr2 = PQCLEAN_NAMESPACE_rej_uniform(a[2].vec[0].coeffs, KYBER_N, buf.x[2], bufbytes);
  ctr3 = PQCLEAN_NAMESPACE_rej_uniform(a[2].vec[1].coeffs, KYBER_N, buf.x[3], bufbytes);

  while(ctr0 < KYBER_N || ctr1 < KYBER_N || ctr2 < KYBER_N || ctr3 < KYBER_N)
  {
    PQCLEAN_NAMESPACE_shake128x4_squeezeblocks(buf.x[0], buf.x[1], buf.x[2], buf.x[3], 1, &state);
    bufbytes = XOF_BLOCKBYTES;

    ctr0 += rej_uniform_ref(a[1].vec[1].coeffs + ctr0, KYBER_N - ctr0, buf.x[0], bufbytes);
    ctr1 += rej_uniform_ref(a[1].vec[2].coeffs + ctr1, KYBER_N - ctr1, buf.x[1], bufbytes);
    ctr2 += rej_uniform_ref(a[2].vec[0].coeffs + ctr2, KYBER_N - ctr2, buf.x[2], bufbytes);
    ctr3 += rej_uniform_ref(a[2].vec[1].coeffs + ctr3, KYBER_N - ctr3, buf.x[3], bufbytes);
  }

  PQCLEAN_NAMESPACE_poly_nttunpack(&a[1].vec[1]);
  PQCLEAN_NAMESPACE_poly_nttunpack(&a[1].vec[2]);
  PQCLEAN_NAMESPACE_poly_nttunpack(&a[2].vec[0]);
  PQCLEAN_NAMESPACE_poly_nttunpack(&a[2].vec[1]);

  if(transposed)
    PQCLEAN_NAMESPACE_kyber_shake128_absorb(&state1x, seed, 2, 2);
  else
    PQCLEAN_NAMESPACE_kyber_shake128_absorb(&state1x, seed, 2, 2);

  PQCLEAN_NAMESPACE_kyber_shake128_squeezeblocks(buf.x[0], GEN_MATRIX_MAXNBLOCKS, &state1x);
  bufbytes = GEN_MATRIX_MAXNBLOCKS*XOF_BLOCKBYTES;

  ctr0 = PQCLEAN_NAMESPACE_rej_uniform(a[2].vec[2].coeffs, KYBER_N, buf.x[0], bufbytes);

  while(ctr0 < KYBER_N)
  {
    PQCLEAN_NAMESPACE_kyber_shake128_squeezeblocks(buf.x[0], 1, &state1x);
    bufbytes = XOF_BLOCKBYTES;

    ctr0 += rej_uniform_ref(a[2].vec[2].coeffs + ctr0, KYBER_N - ctr0, buf.x[0], bufbytes);
  }

  PQCLEAN_NAMESPACE_poly_nttunpack(&a[2].vec[2]);
}
#elif KYBER_K == 4
static void gen_matrix(polyvec *a, const uint8_t *seed, int transposed)
{
  uint16_t i;
  unsigned int ctr0, ctr1, ctr2, ctr3, bufbytes;
  union {
      uint8_t x[4][XOF_BLOCKBYTES*GEN_MATRIX_MAXNBLOCKS];
      __m256i _dummy;
  } buf;
  keccak4x_state state;

  for(i = 0; i < 4; i++)
  {
    if(transposed)
      PQCLEAN_NAMESPACE_kyber_shake128x4_absorb(&state, seed, i+0, i+256, i+512, i+768);
    else
      PQCLEAN_NAMESPACE_kyber_shake128x4_absorb(&state, seed, 256*i+0, 256*i+1, 256*i+2, 256*i+3);

    PQCLEAN_NAMESPACE_shake128x4_squeezeblocks(buf.x[0], buf.x[1], buf.x[2], buf.x[3], GEN_MATRIX_MAXNBLOCKS, &state);
    bufbytes = GEN_MATRIX_MAXNBLOCKS*XOF_BLOCKBYTES;

    ctr0 = PQCLEAN_NAMESPACE_rej_uniform(a[i].vec[0].coeffs, KYBER_N, buf.x[0], bufbytes);
    ctr1 = PQCLEAN_NAMESPACE_rej_uniform(a[i].vec[1].coeffs, KYBER_N, buf.x[1], bufbytes);
    ctr2 = PQCLEAN_NAMESPACE_rej_uniform(a[i].vec[2].coeffs, KYBER_N, buf.x[2], bufbytes);
    ctr3 = PQCLEAN_NAMESPACE_rej_uniform(a[i].vec[3].coeffs, KYBER_N, buf.x[3], bufbytes);

    while(ctr0 < KYBER_N || ctr1 < KYBER_N || ctr2 < KYBER_N || ctr3 < KYBER_N)
    {
      PQCLEAN_NAMESPACE_shake128x4_squeezeblocks(buf.x[0], buf.x[1], buf.x[2], buf.x[3], 1, &state);
      bufbytes = XOF_BLOCKBYTES;

      ctr0 += rej_uniform_ref(a[i].vec[0].coeffs + ctr0, KYBER_N - ctr0, buf.x[0], bufbytes);
      ctr1 += rej_uniform_ref(a[i].vec[1].coeffs + ctr1, KYBER_N - ctr1, buf.x[1], bufbytes);
      ctr2 += rej_uniform_ref(a[i].vec[2].coeffs + ctr2, KYBER_N - ctr2, buf.x[2], bufbytes);
      ctr3 += rej_uniform_ref(a[i].vec[3].coeffs + ctr3, KYBER_N - ctr3, buf.x[3], bufbytes);
    }

    PQCLEAN_NAMESPACE_poly_nttunpack(&a[i].vec[0]);
    PQCLEAN_NAMESPACE_poly_nttunpack(&a[i].vec[1]);
    PQCLEAN_NAMESPACE_poly_nttunpack(&a[i].vec[2]);
    PQCLEAN_NAMESPACE_poly_nttunpack(&a[i].vec[3]);
    }
}
#endif
#endif

/*************************************************
* Name:        indcpa_keypair
*
* Description: Generates public and private key for the CPA-secure
*              public-key encryption scheme underlying Kyber
*
* Arguments:   - uint8_t *pk: pointer to output public key (of length KYBER_INDCPA_PUBLICKEYBYTES bytes)
*              - uint8_t *sk: pointer to output private key (of length KYBER_INDCPA_SECRETKEYBYTES bytes)
**************************************************/
void PQCLEAN_NAMESPACE_indcpa_keypair(uint8_t *pk, uint8_t *sk)
{
  int i;
  polyvec a[KYBER_K], skpv, e, pkpv;
  uint8_t buf[2*KYBER_SYMBYTES];
  const uint8_t *publicseed = buf;
  const uint8_t *noiseseed = buf+KYBER_SYMBYTES;
  uint8_t nonce=0;

  randombytes(buf, KYBER_SYMBYTES);
  hash_g(buf, buf, KYBER_SYMBYTES);

  gen_a(a, publicseed);

#if KYBER_90S
  aes256ctr_ctx state;
  uint8_t coins[128];
  PQCLEAN_NAMESPACE_aes256ctr_init(&state, noiseseed, 0);
  for(i=0;i<KYBER_K;i++) {
    PQCLEAN_NAMESPACE_aes256ctr_select(&state, (uint16_t)nonce++ << 8);
    PQCLEAN_NAMESPACE_aes256ctr_squeezeblocks(coins, 1, &state);
    PQCLEAN_NAMESPACE_cbd(skpv.vec+i, coins);
  }
  for(i=0;i<KYBER_K;i++) {
    PQCLEAN_NAMESPACE_aes256ctr_select(&state, (uint16_t)nonce++ << 8);
    PQCLEAN_NAMESPACE_aes256ctr_squeezeblocks(coins, 1, &state);
    PQCLEAN_NAMESPACE_cbd(e.vec+i, coins);
  }
#else
#if KYBER_K == 2
  PQCLEAN_NAMESPACE_poly_getnoise4x(skpv.vec+0, skpv.vec+1, e.vec+0, e.vec+1, noiseseed, nonce+0, nonce+1, nonce+2, nonce+3);
#elif KYBER_K == 3
  PQCLEAN_NAMESPACE_poly_getnoise4x(skpv.vec+0, skpv.vec+1, skpv.vec+2, e.vec+0, noiseseed, nonce+0, nonce+1, nonce+2, nonce+3);
  PQCLEAN_NAMESPACE_poly_getnoise4x(e.vec+1, e.vec+2, pkpv.vec+0, pkpv.vec+1, noiseseed, nonce+4, nonce+5, 0, 0);
#elif KYBER_K == 4
  PQCLEAN_NAMESPACE_poly_getnoise4x(skpv.vec+0, skpv.vec+1, skpv.vec+2, skpv.vec+3, noiseseed, nonce+0, nonce+1, nonce+2, nonce+3);
  PQCLEAN_NAMESPACE_poly_getnoise4x(e.vec+0, e.vec+1, e.vec+2, e.vec+3, noiseseed, nonce+4, nonce+5, nonce+6, nonce+7);
#endif
#endif

  PQCLEAN_NAMESPACE_polyvec_ntt(&skpv);
  PQCLEAN_NAMESPACE_polyvec_ntt(&e);

  // matrix-vector multiplication
  for(i=0;i<KYBER_K;i++) {
    PQCLEAN_NAMESPACE_polyvec_pointwise_acc(pkpv.vec+i, a+i, &skpv);
    PQCLEAN_NAMESPACE_poly_frommont(pkpv.vec+i);
  }

  PQCLEAN_NAMESPACE_polyvec_add(&pkpv, &pkpv, &e);
  PQCLEAN_NAMESPACE_polyvec_reduce(&pkpv);

  pack_sk(sk, &skpv);
  pack_pk(pk, &pkpv, publicseed);
}

/*************************************************
* Name:        indcpa_enc
*
* Description: Encryption function of the CPA-secure
*              public-key encryption scheme underlying Kyber.
*
* Arguments:   - uint8_t *c:          pointer to output ciphertext (of length KYBER_INDCPA_BYTES bytes)
*              - const uint8_t *m:    pointer to input message (of length KYBER_INDCPA_MSGBYTES bytes)
*              - const uint8_t *pk:   pointer to input public key (of length KYBER_INDCPA_PUBLICKEYBYTES bytes)
*              - const uint8_t *coin: pointer to input random coins used as seed (of length KYBER_SYMBYTES bytes)
*                                           to deterministically generate all randomness
**************************************************/
void PQCLEAN_NAMESPACE_indcpa_enc(uint8_t *c,
                const uint8_t *m,
                const uint8_t *pk,
                const uint8_t *coins)
{
  int i;
  polyvec at[KYBER_K], pkpv, sp, ep, bp;
  poly k, v, epp;
  uint8_t seed[KYBER_SYMBYTES];
  uint8_t nonce=0;

  unpack_pk(&pkpv, seed, pk);
  PQCLEAN_NAMESPACE_poly_frommsg(&k, m);
  gen_at(at, seed);

#if KYBER_90S
  aes256ctr_ctx state;
  uint8_t buf[128];
  PQCLEAN_NAMESPACE_aes256ctr_init(&state, coins, 0);
  for(i=0;i<KYBER_K;i++) {
    PQCLEAN_NAMESPACE_aes256ctr_select(&state, (uint16_t)nonce++ << 8);
    PQCLEAN_NAMESPACE_aes256ctr_squeezeblocks(buf, 1, &state);
    PQCLEAN_NAMESPACE_cbd(sp.vec+i, buf);
  }
  for(i=0;i<KYBER_K;i++) {
    PQCLEAN_NAMESPACE_aes256ctr_select(&state, (uint16_t)nonce++ << 8);
    PQCLEAN_NAMESPACE_aes256ctr_squeezeblocks(buf, 1, &state);
    PQCLEAN_NAMESPACE_cbd(ep.vec+i, buf);
  }
  PQCLEAN_NAMESPACE_aes256ctr_select(&state, (uint16_t)nonce++ << 8);
  PQCLEAN_NAMESPACE_aes256ctr_squeezeblocks(buf, 1, &state);
  PQCLEAN_NAMESPACE_cbd(&epp, buf);
#else
#if KYBER_K == 2
  PQCLEAN_NAMESPACE_poly_getnoise4x(sp.vec+0, sp.vec+1, ep.vec+0, ep.vec+1, coins, nonce+0, nonce+1, nonce+2, nonce+3);
  PQCLEAN_NAMESPACE_poly_getnoise(&epp, coins, nonce+4);
#elif KYBER_K == 3
  PQCLEAN_NAMESPACE_poly_getnoise4x(sp.vec+0, sp.vec+1, sp.vec+2, ep.vec+0, coins, nonce+0, nonce+1, nonce+2, nonce+3);
  PQCLEAN_NAMESPACE_poly_getnoise4x(ep.vec+1, ep.vec+2, &epp, bp.vec+0, coins, nonce+4, nonce+5, nonce+6, 0);
#elif KYBER_K == 4
  PQCLEAN_NAMESPACE_poly_getnoise4x(sp.vec+0, sp.vec+1, sp.vec+2, sp.vec+3, coins, nonce+0, nonce+1, nonce+2, nonce+3);
  PQCLEAN_NAMESPACE_poly_getnoise4x(ep.vec+0, ep.vec+1, ep.vec+2, ep.vec+3, coins, nonce+4, nonce+5, nonce+6, nonce+7);
  PQCLEAN_NAMESPACE_poly_getnoise(&epp, coins, nonce+8);
#endif
#endif

  PQCLEAN_NAMESPACE_polyvec_ntt(&sp);

  // matrix-vector multiplication
  for(i=0;i<KYBER_K;i++)
    PQCLEAN_NAMESPACE_polyvec_pointwise_acc(bp.vec+i, at+i, &sp);

  PQCLEAN_NAMESPACE_polyvec_pointwise_acc(&v, &pkpv, &sp);

  PQCLEAN_NAMESPACE_polyvec_invntt(&bp);
  PQCLEAN_NAMESPACE_poly_invntt(&v);

  PQCLEAN_NAMESPACE_polyvec_add(&bp, &bp, &ep);
  PQCLEAN_NAMESPACE_poly_add(&v, &v, &epp);
  PQCLEAN_NAMESPACE_poly_add(&v, &v, &k);
  PQCLEAN_NAMESPACE_polyvec_reduce(&bp);
  PQCLEAN_NAMESPACE_poly_reduce(&v);

  pack_ciphertext(c, &bp, &v);
}

/*************************************************
* Name:        indcpa_dec
*
* Description: Decryption function of the CPA-secure
*              public-key encryption scheme underlying Kyber.
*
* Arguments:   - uint8_t *m:        pointer to output decrypted message (of length KYBER_INDCPA_MSGBYTES)
*              - const uint8_t *c:  pointer to input ciphertext (of length KYBER_INDCPA_BYTES)
*              - const uint8_t *sk: pointer to input secret key (of length KYBER_INDCPA_SECRETKEYBYTES)
**************************************************/
void PQCLEAN_NAMESPACE_indcpa_dec(uint8_t *m,
                const uint8_t *c,
                const uint8_t *sk)
{
  polyvec bp, skpv;
  poly v, mp;

  unpack_ciphertext(&bp, &v, c);
  unpack_sk(&skpv, sk);

  PQCLEAN_NAMESPACE_polyvec_ntt(&bp);
  PQCLEAN_NAMESPACE_polyvec_pointwise_acc(&mp, &skpv, &bp);
  PQCLEAN_NAMESPACE_poly_invntt(&mp);

  PQCLEAN_NAMESPACE_poly_sub(&mp, &v, &mp);
  PQCLEAN_NAMESPACE_poly_reduce(&mp);

  PQCLEAN_NAMESPACE_poly_tomsg(m, &mp);
}
