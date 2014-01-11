
#include "table_definition.hpp"

namespace Database
{

    TableDefinition::TableDefinition(const QString &n, const std::initializer_list<QString> &c):
        name(n), columns(c)
    {

    }

}
