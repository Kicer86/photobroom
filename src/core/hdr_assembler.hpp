
#ifndef HDR_ASSEMBLER_HPP_INCLUDED
#define HDR_ASSEMBLER_HPP_INCLUDED

#include <QStringList>

#include <core/iexif_reader.hpp>
#include <core_export.h>


namespace HDR
{
    CORE_EXPORT bool assemble(IExifReader&, const QStringList& photos, const QString& outputFile);
}

#endif
