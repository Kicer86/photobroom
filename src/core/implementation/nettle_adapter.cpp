
// Adaptors for Nettle library


#define SHA256_DIGEST_LENGTH SHA256_DIGEST_SIZE
#define SHA256_CTX sha256_ctx
#define SHA256_Init sha256_init
#define SHA256_Update(ctx,data,len) sha256_update(ctx,len,data)
#define SHA256_Final(digest,ctx) sha256_digest(ctx,SHA256_DIGEST_SIZE,digest)

#define MD5_DIGEST_LENGTH MD5_DIGEST_SIZE
#define MD5_CTX md5_ctx
#define MD5_Init md5_init
#define MD5_Update(ctx,data,len) md5_update(ctx,len,data)
#define MD5_Final(digest,ctx) md5_digest(ctx,SHA256_DIGEST_SIZE,digest)

void SHA256(const unsigned char* data, unsigned int len, unsigned char* hash)
{
    sha256_ctx sha256;
    sha256_init(&sha256);
    sha256_update(&sha256, len, data);
    sha256_digest(&sha256, SHA256_DIGEST_SIZE, hash);
}


void MD5(const unsigned char* data, unsigned int len, unsigned char* hash)
{
    md5_ctx md5;
    md5_init(&md5);
    md5_update(&md5, len, data);
    md5_digest(&md5, MD5_DIGEST_SIZE, hash);
}
