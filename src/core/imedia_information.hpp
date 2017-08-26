
#ifndef IMEDIAINFORMATION_HPP
#define IMEDIAINFORMATION_HPP

#include <QSize>

struct IMediaInformation
{
    virtual ~IMediaInformation() = default;

    virtual QSize size(const QString &) const = 0;
};

#endif
