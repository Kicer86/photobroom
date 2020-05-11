
#include <gmock/gmock.h>
#include <QTemporaryFile>

#include "backends/json_backend/json_backend.hpp"
#include "project_info.hpp"


using Database::ProjectInfo;
using Database::JsonBackend;

class JsonBackendTest: public testing::Test
{
    public:
        JsonBackendTest()
            : testing::Test()
            , m_dbFile("db-XXXXXX.json")
        {
            m_dbFile.open();
        }

    protected:
        ProjectInfo getProjectInfo() const
        {
            ProjectInfo prjInfo(m_dbFile.fileName(), "Json");

            return prjInfo;
        }

    private:
        // Json backend will operate on a temporary file.
        // This isn't perfect solution in terms of unit tests
        // but there is no simple enough solution for this.
        QTemporaryFile m_dbFile;
};


TEST_F(JsonBackendTest, opensSuccessfully)
{
    const ProjectInfo prjInfo = getProjectInfo();

    Database::JsonBackend jb;

    const auto status = jb.init(prjInfo);

    EXPECT_TRUE(status);

    jb.closeConnections();
}
