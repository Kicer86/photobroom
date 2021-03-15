
#ifndef PHOTOSDATACOMPLETER_HPP
#define PHOTOSDATACOMPLETER_HPP

#include "database_export.h"
#include "database/idatabase.hpp"


/**
 * @brief class provides photos with extra information guessed from filename, content etc.
 */
class DATABASE_EXPORT PhotosDataCompleter: public QObject
{
public:
    PhotosDataCompleter(Database::IDatabase &);


private:
    Database::IDatabase& m_db;

    void proces(Database::IBackend &);
    void photosFetched(const std::vector<Photo::Id> &);
};

#endif
