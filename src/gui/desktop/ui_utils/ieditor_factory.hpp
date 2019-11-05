
#ifndef IEDITOR_FACTORY_HPP
#define IEDITOR_FACTORY_HPP

struct TagTypeInfo;

struct IEditorFactory
{
    virtual ~IEditorFactory() {}

    virtual QWidget* createEditor(const QModelIndex &, QWidget* parent) = 0;
    virtual QWidget* createEditor(const TagTypeInfo &, QWidget* parent) = 0;
};

#endif
