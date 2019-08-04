
#include <gmock/gmock.h>

#include "database_tools/series_detector.hpp"


TEST(SeriesDetectorTest, constructor)
{
    EXPECT_NO_THROW({
        SeriesDetector sd;
    });
}
