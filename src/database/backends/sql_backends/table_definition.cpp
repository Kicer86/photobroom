
#include "table_definition.hpp"

namespace Database
{

    ColDefinition::ColDefinition(const char* n, ColDefinition::Type t):
        name(n), type(t)
    {

    }


    ColDefinition::ColDefinition(const QString& n, ColDefinition::Type t):
        name(n), type(t)
    {

    }



    TableDefinition::TableDefinition(const QString &n,
                                     const std::initializer_list<ColDefinition> &c,
                                     const std::initializer_list<KeyDefinition>& k):
        name(n), columns(c), keys(k)
    {

    }

}
