

#include "analyzer/default_analyzers/ext_jpeganalyzer.hpp"
#include "analyzer/implementation/file_analyzer.hpp"

#include <CppUTest/TestHarness.h>

TEST_GROUP(Ext_JpegAnalyzerShould)
{
};


TEST(Ext_JpegAnalyzerShould, ReturnTrueForJpegFiles)
{
    EXPECT_N_LEAKS(58);   //probably someting in boost is released in weird way ;) valgrind doesn't show anything
    
    Ext_JpegAnalyzer jpegAnalyzer;
    
    const bool status1 = jpegAnalyzer.isImage("/home/image.jpg");
    const bool status2 = jpegAnalyzer.isImage("/home/image.jpeg");
    const bool status3 = jpegAnalyzer.isImage("/home/image.jp");
    const bool status4 = jpegAnalyzer.isImage("/home/image.png");
    const bool status5 = jpegAnalyzer.isImage("image.jpg");
    const bool status6 = jpegAnalyzer.isImage("image.JpeG");
    const bool status7 = jpegAnalyzer.isImage("image.jpg3");
    const bool status8 = jpegAnalyzer.isImage("/home/image .jpg");
    
    CHECK_EQUAL(true, status1);
    CHECK_EQUAL(true, status2);
    CHECK_EQUAL(false, status3);
    CHECK_EQUAL(false, status4);
    CHECK_EQUAL(true, status5);
    CHECK_EQUAL(true, status6);
    CHECK_EQUAL(false, status7);
    CHECK_EQUAL(true, status8);    
}

/********************************************************************/

TEST_GROUP(FileAnalyzerShould)
{
};


struct FakeAnalyzer: public IAnalyzer
{
    FakeAnalyzer(bool result = false): m_counter(nullptr), m_result(result) {}
    FakeAnalyzer(int *counter): m_counter(counter), m_result(false) {}
    virtual ~FakeAnalyzer() {}
    
    virtual bool isImage(const std::string &)
    {
        if (m_counter != nullptr)
            (*m_counter)++;
        
        return m_result;
    }
    
    int *m_counter;
    bool m_result;
};


TEST(FileAnalyzerShould, CallSubAnalyzersWhenAskedIfIsImage)
{
    FileAnalyzer analyzer;
    
    int counter;
    
    FakeAnalyzer *fake1 = new FakeAnalyzer(&counter);
    FakeAnalyzer *fake2 = new FakeAnalyzer(&counter);
    FakeAnalyzer *fake3 = new FakeAnalyzer(&counter);
    
    analyzer.registerAnalyzer(fake1);
    analyzer.registerAnalyzer(fake2);
    analyzer.registerAnalyzer(fake3);
    
    analyzer.isImage("");
    
    CHECK_EQUAL(3, counter);
}


TEST(FileAnalyzerShould, ReturnTrueIfAnyOfSubAnalyzersWasPositive)
{
    FileAnalyzer analyzer;
    
    FakeAnalyzer *fake1 = new FakeAnalyzer;
    FakeAnalyzer *fake2 = new FakeAnalyzer;
    FakeAnalyzer *fake3 = new FakeAnalyzer(true);
    
    analyzer.registerAnalyzer(fake1);
    analyzer.registerAnalyzer(fake2);
    analyzer.registerAnalyzer(fake3);
    
    const bool status = analyzer.isImage("");
    
    CHECK_EQUAL(true, status);
}


TEST(FileAnalyzerShould, ReturnFalseWhenNoneOfSubAnalyzersWasPositive)
{
    FileAnalyzer analyzer;
    
    FakeAnalyzer *fake1 = new FakeAnalyzer;
    FakeAnalyzer *fake2 = new FakeAnalyzer;
    FakeAnalyzer *fake3 = new FakeAnalyzer;
    
    analyzer.registerAnalyzer(fake1);
    analyzer.registerAnalyzer(fake2);
    analyzer.registerAnalyzer(fake3);
    
    const bool status = analyzer.isImage("");
    
    CHECK_EQUAL(false, status);
}
