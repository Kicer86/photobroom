
#include <unistd.h>
#include <sys/prctl.h>

int main()
{
    const pid_t pid = getpid();
    prctl(PR_SET_PTRACER, pid, 0, 0, 0);

    for(;;)
        sleep(1);

    return 0;
}
