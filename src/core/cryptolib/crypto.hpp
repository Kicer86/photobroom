
#ifndef CRYPTO_HPP
#define CRYPTO_HPP

#include <string>

#include "crypto_export.h"

std::string CRYPTO_EXPORT sha256(const unsigned char* data, unsigned int len);

#endif
