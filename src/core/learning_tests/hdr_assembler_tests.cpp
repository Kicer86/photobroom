
#include <gtest/gtest.h>

#include <core/hdr_assembler.hpp>

// Images for tests:
//   https://en.wikipedia.org/wiki/Multi-exposure_HDR_capture
//   https://www.easyhdr.com/examples/wadi-rum-sunset/


TEST(HDRAssemblerTest, desert)
{
    const QStringList photos = {"wadi-rum-sunset1.jpg", "wadi-rum-sunset2.jpg", "wadi-rum-sunset3.jpg"};

    HDR::assemble(photos, "desert.jpg");
}


TEST(HDRAssemblerTest, city)
{
    const QStringList photos = {"StLouisArchMultExpEV-4.72.JPG", "StLouisArchMultExpEV-1.82.JPG", "StLouisArchMultExpEV+1.51.JPG", "StLouisArchMultExpEV+4.09.JPG"};

    HDR::assemble(photos, "city.jpg");
}
