
#include "crash_dialog_launcher_win32.hpp"

namespace Catcher
{
    void launchDialog(PCONTEXT context)
    {
        // create shared memory to pass context of crash
        HANDLE hMapFile = CreateFileMapping(
                                   INVALID_HANDLE_VALUE,
                                   NULL,
                                   PAGE_READWRITE,
                                   0,
                                   sizeof(CONTEXT),
                                   TEXT("Local\\CrashCatherSharedMemory")
                          );

        LPVOID pBuf = MapViewOfFile(
                           hMapFile,
                           FILE_MAP_ALL_ACCESS,
                           0,
                           0,
                           sizeof(CONTEXT)
                      );

        CopyMemory(pBuf, context, sizeof(CONTEXT));

        // launch dialog

        CloseHandle(hMapFile);
    }
}
