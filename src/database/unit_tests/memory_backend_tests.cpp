
#include <gmock/gmock.h>
#include <QTemporaryFile>

#include "backends/memory_backend/memory_backend.hpp"
#include "project_info.hpp"


using Database::ProjectInfo;
using Database::MemoryBackend;

class MemoryBackendTest: public testing::Test
{
    public:
        MemoryBackendTest()
            : testing::Test()
        {

        }

    protected:
        ProjectInfo getProjectInfo() const
        {
            ProjectInfo prjInfo("", "Memory");

            return prjInfo;
        }
};


TEST_F(MemoryBackendTest, opensSuccessfully)
{
    const ProjectInfo prjInfo = getProjectInfo();

    Database::MemoryBackend jb;

    const auto status = jb.init(prjInfo);

    EXPECT_TRUE(status);

    jb.closeConnections();
}
