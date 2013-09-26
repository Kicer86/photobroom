
#define CPPUTEST_MEM_LEAK_DETECTION_DISABLED

#include <CppUTest/CommandLineTestRunner.h>

IMPORT_TEST_GROUP(Ext_DefaultAnalyzerShould);
IMPORT_TEST_GROUP(FileAnalyzerShould);

int main(int argc, char** argv)
{
    return RUN_ALL_TESTS(argc, argv);
}
