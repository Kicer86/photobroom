
echo ON

rem update cmake if needed
if EXIST "c:\program files\cmake\updated" goto :setup
choco upgrade cmake
copy /y nul "c:\program files\cmake\updated"

:setup

rem setup variables
if "%platform%"=="Win32" (
    set generator="Visual Studio 15"
    set qt_arch=msvc2017
) else (
    set generator="Visual Studio 15 Win64"
    set qt_arch=msvc2017_64
)

set USE_QT_VER=5.9
set PATH=C:\Qt\%USE_QT_VER%\%qt_arch%\bin;C:\Program Files\CMake\bin;%PATH%
set CMAKE_PREFIX_PATH=C:/Qt/%USE_QT_VER%/%qt_arch%;C:\Libraries\boost_1_64_0;c:/projects/install
set GTEST_PATH=c:\projects\googletest

if EXIST c:/projects/install goto :gtest

rem third party stuff
cd tools
c:/Python35/python ./prepare_dependencies.py -p jsoncpp -p openlibrary -p expat -p zlib -p exiv2 -g %generator% -c %Configuration% c:/projects/install
cd ..

rem setup gmock and gtest
:gtest
if EXIST c:/projects/googletest goto :photo_broom
git clone https://github.com/google/googletest.git %GTEST_PATH%

rem photo broom
:photo_broom
mkdir build
cd build
cmake -G%generator% -DGTEST_DIR=%GTEST_PATH%/googletest -DGMOCK_DIR=%GTEST_PATH%/googlemock -DBUILD_UPDATER=ON -DBUILD_TESTS=OFF -DBUILD_SHARED_LIBS=FALSE -DCMAKE_INSTALL_PREFIX=c:/projects/install/ ..
