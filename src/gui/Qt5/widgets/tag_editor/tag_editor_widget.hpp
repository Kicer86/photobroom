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
class TagNameInfo;

class QString;
class QLineEdit;
class QComboBox;


class TagsManagerSlots: public QObject
{
    Q_OBJECT
    
    protected:
        TagsManagerSlots(QObject *p): QObject(p) {}
    
    protected slots:
        virtual void tagEdited() = 0;
        virtual void addLine(const TagNameInfo& name) = 0;
};


class TagEditorWidget: public QWidget
{
    public:
        explicit TagEditorWidget(QWidget* parent = 0, Qt::WindowFlags f = 0);
        virtual ~TagEditorWidget();
        
        TagEditorWidget(const TagEditorWidget& other) = delete;
        virtual TagEditorWidget& operator=(const TagEditorWidget& other) = delete;
        
        void setTags(const std::shared_ptr<ITagData> &);          //widget will work directly on provided set of data
        
    private:
        struct TagsManager;
        std::unique_ptr<TagsManager> m_manager;
        
        
};

#endif // TAG_EDITOR_WIDGET_HPP
