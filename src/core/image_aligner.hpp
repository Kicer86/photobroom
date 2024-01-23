
#ifndef IMAGE_ALIGNER_HPP_INCLUDED
#define IMAGE_ALIGNER_HPP_INCLUDED

#include <QStringList>

#include "core_export.h"


class CORE_EXPORT ImageAligner
{
public:
    bool align(const QString& outputDir, const QString& prefix, const QStringList& photos);
};

#endif
