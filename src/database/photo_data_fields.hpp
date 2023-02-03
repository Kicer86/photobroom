
#ifndef PHOTO_DATA_FIELDS_HPP_INCLUDED
#define PHOTO_DATA_FIELDS_HPP_INCLUDED

#include "photo_types.hpp"
#include "person_data.hpp"


namespace Photo
{
    enum class Field
    {
        Tags,
        Flags,
        Path,
        Geometry,
        GroupInfo,
        PHash,
        People,
    };

    template<Field> struct DeltaTypes {};
    template<> struct DeltaTypes<Field::Tags>      { using Storage = Tag::TagsList;                 };
    template<> struct DeltaTypes<Field::Flags>     { using Storage = Photo::FlagValues;             };
    template<> struct DeltaTypes<Field::Path>      { using Storage = QString;                       };
    template<> struct DeltaTypes<Field::Geometry>  { using Storage = QSize;                         };
    template<> struct DeltaTypes<Field::GroupInfo> { using Storage = GroupInfo;                     };
    template<> struct DeltaTypes<Field::PHash>     { using Storage = Photo::PHashT;                 };
    template<> struct DeltaTypes<Field::People>    { using Storage = std::vector<PersonFullInfo>;   };
}

#endif
