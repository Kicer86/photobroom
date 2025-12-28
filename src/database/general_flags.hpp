
#ifndef GENERAL_FLAGS_HPP_INCLUDED
#define GENERAL_FLAGS_HPP_INCLUDED

#include <QString>

// Various flags for photos.
// Use 0 as most common/basic state,
// so all photos without flag are considered 'normal/basic/unchecked'.
namespace Database::CommonGeneralFlags
{
    const QString State("state");               // photo state.
    enum class StateType
    {
        Normal      = 0,                        // 0 (or nonexistent entry) - photo is in fine state

        // flags:
        Broken      = 1,                        // 1 - one or more photo parameters could not be determined (dimension, thumbnail etc)
        Missing     = 2,                        // 2 - photo file is missing
        Delete      = 4,                        // 4 - photo marked for deletion
    };

    const QString PHashState("phash_state");    // phash info
    enum class PHashStateType
    {
        Normal        = 0,                      // 0 (or nonexistent entry) - photo is in fine state
        Incompatible  = 1,                      // 1 - could not generate phash. Not an image file.
    };

    const QString FacesAnalysisState("faces_analysis_state");
    enum class FacesAnalysisType
    {
        // We could have use 3 states here - 0 for not analyzed, 1 for analyzed and 2 for no faces
        // However that would cause many entries in db as each photo would eventualy went to state 1 or 2.
        // So a bit tricky solution is used here:
        // 0 (or nonexistent entry) means photo was not scaned for faces,
        // or faces were found (and database will return faces for this photo in such case)
        // 1 - no faces found.
        // that should allow to distinguish between all 3 states with less db usage.
        NotAnalysedOrAnalysedAndFound = 0,
        AnalysedAndNotFound = 1,
    };
}

#endif
