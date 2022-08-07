
#ifndef GENERAL_FLAGS_HPP_INCLUDED
#define GENERAL_FLAGS_HPP_INCLUDED

#include <QString>

namespace Database::CommonGeneralFlags
{
    const QString State("state");           // photo state.
    enum class StateType
    {
        Normal      = 0,                    // 0 (or nonexistent entry) - photo is in fine state

        // flags:
        Broken      = 1,                    // 1 - one or more photo parameters could not be determined (dimension, thumbnail etc)
        Missing     = 2,                    // 2 - photo file is missing
        Delete      = 4,                    // 4 - photo marked for deletion
    };

    const QString PHashState("phash_state");    // phash info
    enum class PHashStateType
    {
        Normal        = 0,                      // 0 (or nonexistent entry) - photo is in fine state
        Incomaptible  = 1,                      // 1 - could not generate phash. Not an image file.
    };
}

#endif // GENERAL_FLAGS_HPP_INCLUDED
