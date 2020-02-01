
echo ON

rem setup variables
if "%platform%"=="Win32" (
    set generator="Visual Studio 16 2019"
    set qt_arch=msvc2017
    set arch=win32
) else (
    set generator="Visual Studio 16 2019"
    set qt_arch=msvc2017_64
    set arch=amd64
)

set USE_QT_VER=5.14
set PATH=C:\Qt\%USE_QT_VER%\%qt_arch%\bin;C:\Program Files\CMake\bin;%PATH%
set CMAKE_PREFIX_PATH=C:/Qt/%USE_QT_VER%/%qt_arch%;C:\Libraries\boost_1_64_0;c:/projects/install

mkdir build
cd build
conan install .. --build missing
cmake -G%generator% -A%platform% -DBUILD_UPDATER=ON -DBUILD_TESTING=OFF ..
