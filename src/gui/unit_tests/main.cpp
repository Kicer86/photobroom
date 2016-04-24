
#include <QApplication>

#include <gmock/gmock.h>

int main(int argc, char** argv)
{
#ifdef Q_OS_LINUX
    // gui cannot be tested when there is no graphical environment

    const char* env_p = std::getenv("DISPLAY");
    if (strcmp(env_p, "") == 0)
        return 0;
#endif

    QGuiApplication app(argc, argv);
    ::testing::InitGoogleMock(&argc, argv);
    return RUN_ALL_TESTS();
}
