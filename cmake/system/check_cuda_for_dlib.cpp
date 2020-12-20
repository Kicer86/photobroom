
#include <dlib/config.h>

#ifdef DLIB_USE_CUDA

int main() { return 0;}

#else
#error CUDA not enabled
#endif
