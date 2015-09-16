
#include "table_definition.hpp"

namespace Database
{

    ColDefinition::ColDefinition(const char* n, const QString& typeDefinition, Purpose t):
        name(n), type_definition(typeDefinition), purpose(t)
    {

    }


    TableDefinition::TableDefinition(const QString &n,
                                     const std::initializer_list<ColDefinition> &c,
                                     const std::initializer_list<KeyDefinition>& k):
        name(n), columns(c), keys(k)
    {

    }

}
