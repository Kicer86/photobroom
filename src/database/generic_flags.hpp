
#include <QString>

namespace Database::CommonGenericFlags
{
    const QString State("state");           // photo state.
    enum class StateType
    {
        Normal      = 0,                    // 0 (or nonexistent) - photo is in fine state
        Broken      = 1,                    // 1 - one or more photo parameters could not be determined (dimension, thumbnail etc)
        Missing     = 2,                    // 2 - photo file is missing
    };
}

