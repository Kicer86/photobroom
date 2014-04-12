
#include "table_definition.hpp"

namespace Database
{

    TableDefinition::TableDefinition(const QString &n,
                                     const std::initializer_list<QString> &c,
                                     const std::initializer_list<KeyDefinition>& k):
        name(n), columns(c), keys(k)
    {

    }

}
