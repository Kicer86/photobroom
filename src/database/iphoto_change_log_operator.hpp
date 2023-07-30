
#ifndef IPHOTO_CHANGE_LOG_OPERATOR_HPP
#define IPHOTO_CHANGE_LOG_OPERATOR_HPP


#include <QStringList>

#include "group.hpp"
#include "explicit_photo_delta.hpp"


namespace Database
{
    struct IPhotoChangeLogOperator
    {
        virtual ~IPhotoChangeLogOperator() = default;

        virtual void storeDifference(const Photo::FullDelta &, const Photo::DataDelta &) = 0;
        virtual void groupCreated(const Group::Id &, const Group::Type &, const Photo::Id& representative) = 0;
        virtual void groupDeleted(const Group::Id &, const Photo::Id& representative, const std::vector<Photo::Id>& members) = 0;

        // for debug / tests
        virtual QStringList dumpChangeLog() = 0;
    };
}

#endif
