
#include "table_definition.hpp"

namespace Database
{

    ColDefinition::ColDefinition(const char* n, const QString& typeDefinition, Type t):
        name(n), type_definition(typeDefinition), type(t)
    {

    }


    TableDefinition::TableDefinition(const QString &n,
                                     const std::initializer_list<ColDefinition> &c,
                                     const std::initializer_list<KeyDefinition>& k):
        name(n), columns(c), keys(k)
    {

    }

}
