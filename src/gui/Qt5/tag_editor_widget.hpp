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

#include <memory>
#include <vector>
#include <string>

#include <QWidget>

struct ITagData;
class QString;
class QLineEdit;
class QComboBox;


class TagEntry: public QWidget
{
    public:
        explicit TagEntry(QWidget* parent = 0, Qt::WindowFlags f = 0);
        virtual ~TagEntry();
        
        TagEntry(const TagEntry &) = delete;
        void operator=(const TagEntry &) = delete;
        
        void setTags(const std::vector<QString> &);
        
    private:
        QComboBox   *m_tagsCombo;
        QLineEdit   *m_tagsList;
};


class TagEditorWidget: public QWidget
{
    public:
        explicit TagEditorWidget(QWidget* parent = 0, Qt::WindowFlags f = 0);
        virtual ~TagEditorWidget();
        
        TagEditorWidget(const TagEditorWidget& other) = delete;
        virtual TagEditorWidget& operator=(const TagEditorWidget& other) = delete;
        
        void setTags(ITagData *);          //widget will work directly on provided set of data
        
    private:
        struct Data;
        std::unique_ptr<Data> m_data;        
};

#endif // TAG_EDITOR_WIDGET_HPP
