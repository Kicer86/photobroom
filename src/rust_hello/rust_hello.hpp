#ifndef RUST_HELLO_HPP
#define RUST_HELLO_HPP

#ifdef __cplusplus
extern "C" {
#endif

const char* rust_hello_world();
void rust_free_string(char* s);

#ifdef __cplusplus
}
#endif

#endif // RUST_HELLO_HPP
