
echo ON

rem setup variables
if "%platform%"=="Win32" (
    set generator="Visual Studio 15"
    set qt_arch=msvc2015
    set python_path=C:\Python35
) else (
    set generator="Visual Studio 15 Win64"
    set qt_arch=msvc2017_64
    set python_path=C:\Python35-x64
)

set USE_QT_VER=5.9
set PATH=%python_path%;C:\Qt\%USE_QT_VER%\%qt_arch%\bin;C:\Program Files\CMake\bin;%PATH%
set CMAKE_PREFIX_PATH=C:/Qt/%USE_QT_VER%/%qt_arch%;C:\Libraries\boost_1_64_0;c:/projects/install
set GTEST_PATH=c:\projects\googletest

rem setup gmock and gtest
:gtest
if EXIST c:/projects/googletest goto :photo_broom
git clone --branch release-1.8.1 --depth 1 https://github.com/google/googletest.git %GTEST_PATH%

rem photo broom
:photo_broom
mkdir build
cd build
conan install .. --build missing
cmake -G%generator% -DGTEST_DIR=%GTEST_PATH%/googletest -DGMOCK_DIR=%GTEST_PATH%/googlemock -DBUILD_UPDATER=ON -DBUILD_TESTS=OFF -DCMAKE_INSTALL_PREFIX=c:/projects/install/ ..
