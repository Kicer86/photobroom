/*
 * Set of hash functions.
 * Copyright (C) 2014  Michał Walenciak <MichalWalenciak@gmail.com>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 */

#include "hash_functions.hpp"

#include "cryptolib/crypto.hpp"

std::string HashFunctions::sha256(const unsigned char* str, unsigned int len)
{
    return ::sha256(str, len);
}

/*
std::string HashFunctions::sha256(std::istream& stream)
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


std::string HashFunctions::md5(const unsigned char* str, unsigned int len)
{
    unsigned char hash[MD5_DIGEST_LENGTH];
    MD5(str, len, hash);

    return format(hash, MD5_DIGEST_LENGTH);
}


std::string HashFunctions::md5(std::istream& stream)
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


std::string HashFunctions::format(unsigned char* raw, int len)
{
    std::stringstream ss;

    for(int i = 0; i < len; i++)
        ss << std::hex << std::setw(2) << std::setfill('0') << static_cast<int>(raw[i]);

    return ss.str();
}
*/
