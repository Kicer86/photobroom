
#ifndef IEDITOR_FACTORY_HPP
#define IEDITOR_FACTORY_HPP

#include <core/tag.hpp>

struct IEditorFactory
{
    virtual ~IEditorFactory() {}

    virtual QWidget* createEditor(const QModelIndex &, QWidget* parent) = 0;
    virtual QWidget* createEditor(const TagNameInfo::Type &, QWidget* parent) = 0;
};

#endif
