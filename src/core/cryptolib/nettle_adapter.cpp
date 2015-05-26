
// Adaptors for Nettle library

#include <cstring>

#include <nettle/sha2.h>

#include "utils.hpp"


std::string sha256(const unsigned char* data, unsigned int len)
{
    unsigned char hash[SHA256_DIGEST_SIZE];

    sha256_ctx sha256;
    sha256_init(&sha256);
    sha256_update(&sha256, len, data);
    sha256_digest(&sha256, SHA256_DIGEST_SIZE, hash);

    return format(hash, SHA256_DIGEST_SIZE);
}
