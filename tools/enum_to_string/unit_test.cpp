
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


TEST(WordReaderTest, oneWord)
{
    std::stringstream str("qwerty");

    const std::string w1 = read_word(str);

    EXPECT_EQ(w1, "qwerty");
}


TEST(WordReaderTest, manyWhitespaces)
{
    std::stringstream str("   \n\tabc\t\t\r\ndef\r\n\r\n \t");

    const std::string w1 = read_word(str);
    const std::string w2 = read_word(str);

    EXPECT_EQ(w1, "abc");
    EXPECT_EQ(w2, "def");
}


///////////////////////////////////////////////////////////////////////////////


TEST(EnumFinderTest, emptyEnum)
{
    std::stringstream str("enum E {};");

    auto r = find_enum(str);

    ASSERT_EQ(r.size(), 1);
    EXPECT_EQ(r.front().name, "E");
    EXPECT_TRUE(r.front().entries.empty());
}
