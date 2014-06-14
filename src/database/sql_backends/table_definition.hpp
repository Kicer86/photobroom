
#ifndef DATABASE_TABLE_DEFINITION_HPP
#define DATABASE_TABLE_DEFINITION_HPP


#include <vector>

#include <QString>

#include "sql_backend_base_export.h"

namespace Database
{

    struct SQL_BACKEND_BASE_EXPORT ColDefinition
    {
        QString name;
        enum class Type
        {
            ID,
            Regular,
        } type;

        ColDefinition(const char *, Type t = Type::Regular);
        ColDefinition(const QString &, Type t = Type::Regular);
    };

    struct SQL_BACKEND_BASE_EXPORT TableDefinition
    {
        struct KeyDefinition
        {
            QString name;
            QString type;
            QString def;
        };

        const QString name;
        const std::vector<ColDefinition> columns;
        const std::vector<KeyDefinition> keys;

        TableDefinition(const QString& n,
                        const std::initializer_list<ColDefinition>& c,
                        const std::initializer_list<KeyDefinition>& k = { }
                       );

		TableDefinition(const TableDefinition &) = default;

		TableDefinition& operator=(const TableDefinition &) = default;
    };

}

#endif
