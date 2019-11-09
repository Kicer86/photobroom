/*
 * Item delagate for tags view
 * Copyright (C) 2015  Michał Walenciak <MichalWalenciak@gmail.com>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 */

#ifndef TAGSITEMDELEGATE_HPP
#define TAGSITEMDELEGATE_HPP

#include <QStyledItemDelegate>

#include <core/tag.hpp>
#include "utils/tag_value_model.hpp"

struct IEditorFactory;


/**
 * \brief Delegate for tags editor.
 */
class TagsItemDelegate : public QStyledItemDelegate
{
    public:
        /**
         * \brief Constrguctor
         * \arg factory editor factory
         *
         * TagsItemDelegate does not use Qt's QItemEditorFactory as it is cell type oriented.
         * IEditorFactory provides editors basing of tag type.
         */
        explicit TagsItemDelegate(IEditorFactory& factory);
        TagsItemDelegate(const TagsItemDelegate &) = delete;
        ~TagsItemDelegate();

        TagsItemDelegate& operator=(const TagsItemDelegate &) = delete;

    private:
        IEditorFactory& m_editorFactory;

        QWidget* createEditor(QWidget* parent, const QStyleOptionViewItem& option, const QModelIndex& index) const override;
        QString displayText(const QVariant& value, const QLocale& locale) const override;
        void setModelData(QWidget *editor, QAbstractItemModel *model, const QModelIndex &index) const override;
        void setEditorData(QWidget *editor, const QModelIndex &index) const override;
        void paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const override;
};

#endif // TAGSITEMDELEGATE_HPP
