
if "%platform%"=="Win32"
(
    set generator="Visual Studio 14"
    set qt_arch=msvc2015)
else
(
    set generator="Visual Studio 14 Win64"
    set qt_arch=msvc2015_64
)

set PATH=C:\Qt\5.6\%qt_arch%\bin;%PATH%
set CMAKE_PREFIX_PATH=C:/Qt/5.6/%qt_arch%;C:\Libraries\boost;c:/projects/install

if EXIST c:/projects/install goto :gtest

rem third party stuff
mkdir build_third_party
cd build_third_party
cmake -G%generator% -DBUILD_OPENLIBRARY=ON -DBUILD_EASYEXIF=ON -DBUILD_JSONCPP=ON -DINSTALL_LOCATION=c:/projects/install/ ../cmake/third_party/
cmake --build . --config %Configuration%
cd ..

rem setup gmock and gtest
:gtest
git clone https://github.com/google/googletest.git
set GTEST_PATH=%cd%/googletest

rem photo broom
mkdir build
cd build
cmake -G%generator% -DGTEST_DIR=$GTEST_PATH/googletest -DGMOCK_DIR=$GTEST_PATH/googlemock -DBUILD_TESTS=OFF -DBUILD_SHARED_LIBS=FALSE -DCMAKE_INSTALL_PREFIX=c:/projects/install/ ..
