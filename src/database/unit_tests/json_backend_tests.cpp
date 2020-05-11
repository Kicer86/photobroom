
#include <gmock/gmock.h>
#include <QTemporaryFile>


class JsonBackendTest: public testing::Test
{
    public:
        JsonBackendTest()
            : testing::Test()
            , m_dbFile("db-XXXXXX.json")
        {

        }

    private:
        // Json backend will operate on a temporary file.
        // This isn't perfect solution in terms of unit tests
        // but there is no simple enough solution for this.
        QTemporaryFile m_dbFile;
};
