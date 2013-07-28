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


struct TagEntry: public QWidget
{
        explicit TagEntry(QWidget *parent, Qt::WindowFlags f = 0);
        virtual ~TagEntry();
        
        TagEntry(const TagEntry &) = delete;
        void operator=(const TagEntry &) = delete;
        
        void setTags(const std::vector<QString> &);
        
        QComboBox   *m_tagsCombo;
        QLineEdit   *m_tagsList;
};


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


struct TagEditorWidget::TagsManager: public TagsManagerSlots
{
    TagsManager(TagEditorWidget *tagWidget): 
        m_avail_tags({"Event", "Place", "Date", "Time", "People"}),
        m_tagWidget(tagWidget)
    {
    }
    
    
    void addEmptyLine()
    {
        TagEntry *tagEntry = new TagEntry(m_tagWidget);
        connect( tagEntry->m_tagsList, SIGNAL(textChanged(QString)), this, SLOT(tagEdited()) );
        
        QLayout *lay = m_tagWidget->layout();
        lay->addWidget(tagEntry);
    }
    
    
    void removeAll()
    {
        QLayout *lay = m_tagWidget->layout();
        
        while ( QLayoutItem *item = lay->takeAt(0) )
            delete item;
    }
    
    virtual void tagEdited()
    {
    }
        
    std::vector<QString> m_avail_tags;
    TagEditorWidget *m_tagWidget;
};


/*****************************************************************************/


TagEditorWidget::TagEditorWidget(QWidget *p, Qt::WindowFlags f): 
    QWidget(p, f),
    m_manager(new TagsManager(this))
{   
    new QVBoxLayout(this);
    
    m_manager->addEmptyLine();
}


TagEditorWidget::~TagEditorWidget()
{

}


void TagEditorWidget::setTags(ITagData *tagData)
{
    
}

