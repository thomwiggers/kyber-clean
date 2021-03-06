#ifndef PQCLEAN_NAMESPACE_API_H
#define PQCLEAN_NAMESPACE_API_H

#include <stdint.h>

#if   (KYBER_K == 2)
#define PQCLEAN_NAMESPACE_CRYPTO_SECRETKEYBYTES  1632
#define PQCLEAN_NAMESPACE_CRYPTO_PUBLICKEYBYTES  800
#define PQCLEAN_NAMESPACE_CRYPTO_CIPHERTEXTBYTES 736
#define PQCLEAN_NAMESPACE_CRYPTO_BYTES           32
#ifdef KYBER_90S
#define PQCLEAN_NAMESPACE_CRYPTO_ALGNAME "Kyber512-90s"
#else
#define PQCLEAN_NAMESPACE_CRYPTO_ALGNAME "Kyber512"
#endif
#elif (KYBER_K == 3)
#define PQCLEAN_NAMESPACE_CRYPTO_SECRETKEYBYTES  2400
#define PQCLEAN_NAMESPACE_CRYPTO_PUBLICKEYBYTES  1184
#define PQCLEAN_NAMESPACE_CRYPTO_CIPHERTEXTBYTES 1088
#define PQCLEAN_NAMESPACE_CRYPTO_BYTES           32
#ifdef KYBER_90S
#define PQCLEAN_NAMESPACE_CRYPTO_ALGNAME "Kyber768-90s"
#else
#define PQCLEAN_NAMESPACE_CRYPTO_ALGNAME "Kyber768"
#endif
#elif (KYBER_K == 4)
#define PQCLEAN_NAMESPACE_CRYPTO_SECRETKEYBYTES  3168
#define PQCLEAN_NAMESPACE_CRYPTO_PUBLICKEYBYTES  1568
#define PQCLEAN_NAMESPACE_CRYPTO_CIPHERTEXTBYTES 1568
#define PQCLEAN_NAMESPACE_CRYPTO_BYTES           32
#ifdef KYBER_90S
#define PQCLEAN_NAMESPACE_CRYPTO_ALGNAME "Kyber1024-90s"
#else
#define PQCLEAN_NAMESPACE_CRYPTO_ALGNAME "Kyber1024"
#endif
#else
#error "KYBER_K must be in {2,3,4}"
#endif

int PQCLEAN_NAMESPACE_crypto_kem_keypair(uint8_t *pk, uint8_t *sk);

int PQCLEAN_NAMESPACE_crypto_kem_enc(uint8_t *ct, uint8_t *ss, const uint8_t *pk);

int PQCLEAN_NAMESPACE_crypto_kem_dec(uint8_t *ss, const uint8_t *ct, const uint8_t *sk);


#endif
