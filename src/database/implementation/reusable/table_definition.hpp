
#ifndef DATABASE_TABLE_DEFINITION_HPP
#define DATABASE_TABLE_DEFINITION_HPP

#include <QString>
#include <QList>

#include "sql_backend_base_export.h"

namespace Database
{

    struct SQL_BACKEND_BASE_EXPORT TableDefinition
    {
        const QString name;
        const QList<QString> columns;

        TableDefinition(const QString& n, const std::initializer_list<QString>& c);
    };

}

#endif
