
#include "default_analyzers/file_analyzer.hpp"

#include <gtest/gtest.h>

#include <QString>

typedef std::pair<Filesystem::Location, bool> AnalyzerExpectations;


struct FileAnalyzerTest: testing::TestWithParam<AnalyzerExpectations> {};

TEST_P(FileAnalyzerTest, ProperExtensionsAreRespected)
{
    FileAnalyzer analyzer;
    const auto [path, is] = GetParam();

    EXPECT_EQ(is, analyzer.isMediaFile(path));
}

INSTANTIATE_TEST_SUITE_P(ExtensionsTest,
                         FileAnalyzerTest,
                         testing::Values(
                             AnalyzerExpectations{"/home/image.jpg",  true  },
                             AnalyzerExpectations{"/home/image.jpeg", true  },
                             AnalyzerExpectations{"/home/image.jp",   false },
                             AnalyzerExpectations{"/home/image.png",  true  },
                             AnalyzerExpectations{"image.jpg",        true  },
                             AnalyzerExpectations{"image.JpeG",       true  },
                             AnalyzerExpectations{"image.jpg3",       false },
                             AnalyzerExpectations{"/home/image .jpg", true  },
                             AnalyzerExpectations{"/home/v .mp4",     true  },
                             AnalyzerExpectations{"a.avi",            true  },
                             AnalyzerExpectations{"mkv",              false },
                             AnalyzerExpectations{"/home/q.mkv",      true  }
                         )
);
