/*
    Widget for tags manipulation
    Copyright (C) 2013  Michał Walenciak <MichalWalenciak@gmail.com>

    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License along
    with this program; if not, write to the Free Software Foundation, Inc.,
    51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
*/


#ifndef TAG_EDITOR_WIDGET_HPP
#define TAG_EDITOR_WIDGET_HPP

#include <QWidget>

#include "ui_utils/editor_factory.hpp"
#include "helpers/tags_operator.hpp"

class QItemSelectionModel;
class QComboBox;
class QLineEdit;
class QPushButton;

class InfoBaloonWidget;
class DBDataModel;
class TagsView;
class TagsModel;

namespace Database
{
    struct IDatabase;
}

struct ITagValueWidget;
struct ICompleterFactory;

class TagEditorWidget: public QWidget
{
        Q_OBJECT

    public:
        explicit TagEditorWidget(QWidget * = 0, Qt::WindowFlags = 0);
        virtual ~TagEditorWidget();

        TagEditorWidget(const TagEditorWidget &) = delete;
        virtual TagEditorWidget& operator=(const TagEditorWidget &) = delete;

        void set(QItemSelectionModel *);
        void set(DBDataModel *);
        void set(ICompleterFactory *);

    private:
        EditorFactory m_editorFactory;
        TagsView* m_view;
        TagsModel* m_model;
        TagsOperator m_tagsOperator;
        QComboBox* m_tagName;
        QPushButton* m_addButton;
        QWidget* m_tagValueContainer;
        QWidget* m_tagValueWidget;
        InfoBaloonWidget* m_hint;
        QByteArray m_tagValueProp;

        std::vector<TagNameInfo> m_tags;

        void setTagValueWidget(size_t);

    private slots:
        void refreshTagNamesList(bool);
        void addButtonPressed();
        void tagNameChanged(int);
        void emptyValueError();
};

#endif // TAG_EDITOR_WIDGET_HPP
