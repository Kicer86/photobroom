
#include <unistd.h>
#include <sys/prctl.h>

int main()
{
    prctl(PR_SET_PTRACER, PR_SET_PTRACER_ANY, 0, 0, 0);

    for(;;)
        sleep(1);

    return 0;
}
