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
#include <QComboBox>
#include <QLineEdit>
#include <QPushButton>
#include <QHBoxLayout>
#include <QVBoxLayout>

ITagData::~ITagData()
{

}


/*****************************************************************************/


struct TagEditorWidget::Data
{
    //TODO: read tags from config
    Data(): m_tags({"Event", "Place", "Date", "Time", "People"}) {}
    ~Data() {}
    
    const std::vector<QString>& getTags() const
    {
        return m_tags;
    }    
    
    private:
        std::vector<QString> m_tags;
};


/*****************************************************************************/


TagEntry::TagEntry(QWidget* p, Qt::WindowFlags f): 
    QWidget(p, f), 
    m_tagsCombo(nullptr),
    m_tagsList(nullptr)
{
    m_tagsCombo = new QComboBox(this);
    m_tagsList  = new QLineEdit(this);
    
    QHBoxLayout *mainLayout = new QHBoxLayout(this);
    mainLayout->addWidget(m_tagsCombo);
    mainLayout->addWidget(m_tagsList);
}


TagEntry::~TagEntry()
{

}


void TagEntry::setTags(const std::vector<QString> &tags)
{
    m_tagsCombo->clear();
    
    for (const QString &tag: tags)
        m_tagsCombo->addItem(tag);
}


/*****************************************************************************/


TagEditorWidget::TagEditorWidget(QWidget *p, Qt::WindowFlags f): QWidget(p, f), m_data(new Data)
{
    TagEntry *tagEntry = new TagEntry(this);
    tagEntry->setTags(m_data->getTags());
    
    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    mainLayout->addWidget(tagEntry);
}


TagEditorWidget::~TagEditorWidget()
{

}
