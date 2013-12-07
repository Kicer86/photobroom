
#include "default_analyzers/ext_defaultanalyzer.hpp"
#include "implementation/file_analyzer.hpp"

#include <gtest/gtest.h>


TEST(Ext_DefaultAnalyzerShould, ReturnTrueForJpegFiles)
{
    Ext_DefaultAnalyzer jpegAnalyzer;
    
    const bool status1 = jpegAnalyzer.isImage("/home/image.jpg");
    const bool status2 = jpegAnalyzer.isImage("/home/image.jpeg");
    const bool status3 = jpegAnalyzer.isImage("/home/image.jp");
    const bool status4 = jpegAnalyzer.isImage("/home/image.png");
    const bool status5 = jpegAnalyzer.isImage("image.jpg");
    const bool status6 = jpegAnalyzer.isImage("image.JpeG");
    const bool status7 = jpegAnalyzer.isImage("image.jpg3");
    const bool status8 = jpegAnalyzer.isImage("/home/image .jpg");
    
    ASSERT_EQ(true, status1);
    ASSERT_EQ(true, status2);
    ASSERT_EQ(false, status3);
    ASSERT_EQ(false, status4);
    ASSERT_EQ(true, status5);
    ASSERT_EQ(true, status6);
    ASSERT_EQ(false, status7);
    ASSERT_EQ(true, status8);    
}

/********************************************************************/

struct FakeAnalyzer: public IAnalyzer
{
    FakeAnalyzer(bool result = false): m_counter(nullptr), m_result(result) {}
    FakeAnalyzer(int *counter): m_counter(counter), m_result(false) {}
    FakeAnalyzer(const FakeAnalyzer &) = delete;
    virtual ~FakeAnalyzer() {}
    
    virtual bool isImage(const std::string &)
    {
        if (m_counter != nullptr)
            (*m_counter)++;
        
        return m_result;
    }
    
    FakeAnalyzer& operator=(const FakeAnalyzer &) = delete;
    
    int *m_counter;
    bool m_result;
};


TEST(FileAnalyzerShould, CallSubAnalyzersWhenAskedIfIsImage)
{
    FileAnalyzer analyzer;
    
    int counter = 0;
    
    FakeAnalyzer *fake1 = new FakeAnalyzer(&counter);
    FakeAnalyzer *fake2 = new FakeAnalyzer(&counter);
    FakeAnalyzer *fake3 = new FakeAnalyzer(&counter);
    
    analyzer.registerAnalyzer(fake1);
    analyzer.registerAnalyzer(fake2);
    analyzer.registerAnalyzer(fake3);
    
    analyzer.isImage("");
    
    ASSERT_EQ(3, counter);
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
    
    ASSERT_EQ(true, status);
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
    
    ASSERT_EQ(false, status);
}
