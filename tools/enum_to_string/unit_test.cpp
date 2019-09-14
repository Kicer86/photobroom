
#include <gmock/gmock.h>
#include <sstream>

#include "logic.cpp"

TEST(WordReaderTest, aFewWords)
{
    std::stringstream str("a ab abc");

    const std::string w1 = read_word(str);
    const std::string w2 = read_word(str);
    const std::string w3 = read_word(str);

    EXPECT_EQ(w1, "a");
    EXPECT_EQ(w2, "ab");
    EXPECT_EQ(w3, "abc");
}


TEST(WordReaderTest, emptyInput)
{
    std::stringstream str;

    const std::string w1 = read_word(str);

    EXPECT_TRUE(w1.empty());
}
