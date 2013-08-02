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

#include <assert.h>

#include <vector>

#include <QString>
#include <QComboBox>
#include <QLineEdit>
#include <QPushButton>
#include <QHBoxLayout>
#include <QVBoxLayout>

#include "core/types.hpp"

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


TagEntry::TagEntry(QWidget *p, Qt::WindowFlags f):
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
        m_avail_tags( {"Event", "Place", "Date", "Time", "People"}),
        m_tagWidget(tagWidget)
    {
    }

    TagsManager(const TagsManager &) = delete;
    void operator=(const TagsManager &) = delete;
    
    void setTags(ITagData *tagData)
    {
        removeAll();
        std::vector<ITagData::TagInfo> tags = tagData->getTags();
        
        for (auto &tag: tags)
            addLine(tag.name, tag.values);
    }
    
    void addLine(const QString &name, const QString &value)
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
        QLayout *lay = m_tagWidget->layout();

        //analyze each "TagEntry"

        auto getTagEntry = [&] (int i) -> TagEntry *
        {
            QLayoutItem *item = lay->itemAt(i);
            QWidget *widget = item->widget();

            assert(dynamic_cast<TagEntry *>(widget) != nullptr);
            TagEntry *entry = static_cast<TagEntry *>(widget);

            return entry;
        };

        auto isEmpty = [&] (int i) -> bool
        {
            TagEntry *entry = getTagEntry(i);

            const bool empty = entry->m_tagsList->text().isEmpty();

            return empty;
        };

        while ( int items = lay->count() >= 2 ? lay->count() : 0)
        {
            if ( isEmpty(items - 1) && isEmpty(items - 2) )
                delete lay->takeAt(items - 1);
            else
                break;
        }

        if (getTagEntry(lay->count() - 1)->m_tagsList->text().isEmpty() == false)
            addEmptyLine();
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
    m_manager->setTags(tagData);
}
