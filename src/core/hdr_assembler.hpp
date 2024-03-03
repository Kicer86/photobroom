
#ifndef HDR_ASSEMBLER_HPP_INCLUDED
#define HDR_ASSEMBLER_HPP_INCLUDED

#include <QStringList>

#include <core_export.h>


namespace HDR
{
    CORE_EXPORT bool assemble(const QStringList& photos, const QString& outputFile);
}

#endif
