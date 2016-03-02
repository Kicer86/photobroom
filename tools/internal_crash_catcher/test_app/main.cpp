
#include <unistd.h>

#ifdef OS_UNIX
    #include <sys/prctl.h>
#endif

int main()
{    
#ifdef OS_UNIX
    prctl(PR_SET_PTRACER, PR_SET_PTRACER_ANY, 0, 0, 0);
#endif

    for(;;)
        sleep(1);

    return 0;
}
