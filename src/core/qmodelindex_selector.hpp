
#ifndef QMODELINDEX_SELECTOR_HPP
#define QMODELINDEX_SELECTOR_HPP

#include <QModelIndex>

#include "core_export.h"

namespace QModelIndexSelector
{
    CORE_EXPORT std::vector<QModelIndex> listAllBetween(const QModelIndex &, const QModelIndex &);
}

#endif // QMODELINDEX_SELECTOR_HPP
