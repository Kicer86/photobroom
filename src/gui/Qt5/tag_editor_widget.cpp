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


#include "tag_editor_widget.hpp"

#include <vector>

#include <QString>

struct TagEditorWidget::Data
{
    //TODO: read tags from config
    Data(): m_tags({"Event", "Place", "Date", "Time", "People"}) {}
    ~Data() {}
    
    std::vector<QString> m_tags;
};


TagEditorWidget::TagEditorWidget(QWidget *p): QWidget(p), m_data(new Data)
{
    
}


TagEditorWidget::~TagEditorWidget()
{

}
