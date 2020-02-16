
#include <gtest/gtest.h>
#include <QSize>

#include "utils.hpp"


TEST(DlibTests, PhotoSizes)
{
    const QImage img1(utils::photoPath(1));
    const QImage img2(utils::photoPath(2));

    ASSERT_EQ(img1.size(), QSize(1024, 1024));
    ASSERT_EQ(img2.size(), QSize(1024, 1024));
}
