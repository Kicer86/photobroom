
#include <QApplication>

#include <gmock/gmock.h>

int main(int argc, char** argv)
{
    int result = 0;
    QScreen* screen = QGuiApplication::primaryScreen();

    if (screen != nullptr)
    {
        QGuiApplication app(argc, argv);
        ::testing::InitGoogleMock(&argc, argv);

        result = RUN_ALL_TESTS();
    }

    return result;
}
