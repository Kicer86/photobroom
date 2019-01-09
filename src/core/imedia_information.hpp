
#ifndef IMEDIAINFORMATION_HPP
#define IMEDIAINFORMATION_HPP

#include <QSize>

struct IMediaInformation
{
    virtual ~IMediaInformation() = default;

    virtual bool canHandle(const QString &) const = 0;
    virtual QSize size(const QString &) const = 0;
};

#endif
