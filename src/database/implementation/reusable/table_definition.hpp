
#ifndef DATABASE_TABLE_DEFINITION_HPP
#define DATABASE_TABLE_DEFINITION_HPP

#include <QString>
#include <QList>

namespace Database
{

    struct TableDefinition
    {
        const QString name;
        const QList<QString> columns;

        TableDefinition(const QString& n, const std::initializer_list<QString>& c);
    };

}

#endif
