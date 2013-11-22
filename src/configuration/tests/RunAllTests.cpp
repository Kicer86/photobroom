
#define CPPUTEST_MEM_LEAK_DETECTION_DISABLED

#include <CppUTest/CommandLineTestRunner.h>

IMPORT_TEST_GROUP(EntryDataTests);
IMPORT_TEST_GROUP(ConfigurationKeyShould);

int main(int argc, char** argv)
{
    return RUN_ALL_TESTS(argc, argv);
}
