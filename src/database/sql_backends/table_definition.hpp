
#ifndef DATABASE_TABLE_DEFINITION_HPP
#define DATABASE_TABLE_DEFINITION_HPP

#include <QString>
#include <QList>
#include <QPair>

#include "sql_backend_base_export.h"

namespace Database
{

    struct SQL_BACKEND_BASE_EXPORT TableDefinition
    {
        struct KeyDefinition
        {
            QString name;
            QString type;
            QString def;
        };

        const QString name;
        const QList<QString> columns;
        const QList<KeyDefinition> keys;

        TableDefinition(const QString& n,
                        const std::initializer_list<QString>& c,
                        const std::initializer_list<KeyDefinition>& k = { }
                       );
    };

}

#endif
