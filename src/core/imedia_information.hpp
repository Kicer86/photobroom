
#ifndef IMEDIAINFORMATION_HPP
#define IMEDIAINFORMATION_HPP

#include <optional>

#include <QSize>

struct IMediaInformation
{
    virtual ~IMediaInformation() = default;

    virtual std::optional<QSize> size(const QString &) const = 0;
};

#endif
