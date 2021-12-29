
#ifndef IEDITOR_FACTORY_HPP
#define IEDITOR_FACTORY_HPP

struct IEditorFactory
{
    virtual ~IEditorFactory() = default;

    virtual QWidget* createEditor(const QModelIndex &, QWidget* parent) = 0;
    virtual QWidget* createEditor(const Tag::Types &, QWidget* parent) = 0;
    virtual QByteArray valuePropertyName(const Tag::Types &) const = 0;
};

#endif
