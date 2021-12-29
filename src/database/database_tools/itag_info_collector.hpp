
#ifndef ITAG_INFO_COLLECTOR_HPP
#define ITAG_INFO_COLLECTOR_HPP

#include <functional>

#include <QObject>

#include <core/tag.hpp>

#include "database_export.h"


struct DATABASE_EXPORT ITagInfoCollector: public QObject
{
    virtual ~ITagInfoCollector() = default;

    virtual const std::vector<TagValue>& get(const Tag::Types &) const = 0;

    Q_OBJECT

signals:
    void setOfValuesChanged(const Tag::Types &);
};

#endif
