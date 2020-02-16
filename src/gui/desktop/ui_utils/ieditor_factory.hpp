
#ifndef IEDITOR_FACTORY_HPP
#define IEDITOR_FACTORY_HPP

class TagTypeInfo;

struct IEditorFactory
{
    virtual ~IEditorFactory() = default;

    virtual QWidget* createEditor(const QModelIndex &, QWidget* parent) = 0;
    virtual QWidget* createEditor(const TagTypeInfo &, QWidget* parent) = 0;
    virtual QByteArray valuePropertyName(const TagTypeInfo &) const = 0;
};

#endif
