
// Adaptors for OpenSSL library

#include <sstream>

#include <openssl/sha.h>
#include <openssl/md5.h>

#include "utils.hpp"


std::string sha256(const unsigned char* str, unsigned int len)
{
    unsigned char hash[SHA256_DIGEST_LENGTH];

    SHA256(str, len, hash);

    return format(hash, SHA256_DIGEST_LENGTH);
}


std::string sha256(std::istream& stream)
{
    unsigned char hash[SHA256_DIGEST_LENGTH];
    SHA256_CTX sha256;
    SHA256_Init(&sha256);

    while (stream.eof() == false)
    {
        unsigned char buffer[65536];
        stream.read(reinterpret_cast<char *>(buffer), 65536);

        const std::streamsize got = stream.gcount();

        SHA256_Update(&sha256, buffer, static_cast<size_t>(got));
    }

    SHA256_Final(hash, &sha256);

    return format(hash, SHA256_DIGEST_LENGTH);
}


std::string md5(const unsigned char* str, unsigned int len)
{
    unsigned char hash[MD5_DIGEST_LENGTH];
    MD5(str, len, hash);

    return format(hash, MD5_DIGEST_LENGTH);
}


std::string md5(std::istream& stream)
{
    unsigned char hash[MD5_DIGEST_LENGTH];
    MD5_CTX md5;
    MD5_Init(&md5);

    while (stream.eof() == false)
    {
        unsigned char buffer[65536];
        stream.read(reinterpret_cast<char *>(buffer), 65536);

        const std::streamsize got = stream.gcount();

        MD5_Update(&md5, buffer, static_cast<size_t>(got));
    }

    MD5_Final(hash, &md5);

    return format(hash, MD5_DIGEST_LENGTH);
}

