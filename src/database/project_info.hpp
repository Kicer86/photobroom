
#ifndef PROJECT_INFO_HPP
#define PROJECT_INFO_HPP

namespace Database
{

    struct ProjectInfo
    {
        QString databaseLocation;
        QString backendName;
        QString projectDir;

        bool operator<(const ProjectInfo& other) const
        {
            bool status = false;

            if (databaseLocation < other.databaseLocation)
                status = true;
            else if (databaseLocation == other.databaseLocation && backendName < other.backendName)
                status = true;

            return status;
        }

        ProjectInfo(): databaseLocation(), backendName(), projectDir() {}
        ProjectInfo(const QString& location, const QString& backend, const QString& prjDir):
        databaseLocation(location),
        backendName(backend),
        projectDir(prjDir)
        {

        }
    };
}

#endif
