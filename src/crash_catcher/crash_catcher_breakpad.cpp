#include "crash_catcher.hpp"

#include <client/linux/handler/exception_handler.h>
#include <filesystem>
#include <memory>

namespace {
std::unique_ptr<google_breakpad::ExceptionHandler> g_handler;
}

bool CrashCatcher::internal_init()
{
    try
    {
        const auto dump_dir = std::filesystem::temp_directory_path() / CrashCatcher::name();
        std::filesystem::create_directories(dump_dir);
        google_breakpad::MinidumpDescriptor descriptor(dump_dir.string());
        g_handler = std::make_unique<google_breakpad::ExceptionHandler>(descriptor, nullptr, nullptr, nullptr, true, -1);
        return true;
    }
    catch(const std::exception &)
    {
        return false;
    }
}
