
#ifndef PROJECT_INFO_HPP
#define PROJECT_INFO_HPP

namespace Database
{

    struct ProjectInfo
    {
        QString databaseLocation;
        QString backendName;

        bool operator<(const ProjectInfo& other) const
        {
            bool status = false;

            if (databaseLocation < other.databaseLocation)
                status = true;
            else if (databaseLocation == other.databaseLocation && backendName < other.backendName)
                status = true;

            return status;
        }

        ProjectInfo(): databaseLocation(), backendName(){}
        ProjectInfo(const QString& location, const QString& backend):
            databaseLocation(location),
            backendName(backend)
        {

        }
    };
}

#endif
