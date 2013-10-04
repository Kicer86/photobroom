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

    void setListOfBaseTags(const std::vector<QString> &);
    void selectTag(const QString &name);
    void setTagValue(const QString &value);
    void clear();

    QComboBox   *m_tagsCombo;
    QLineEdit   *m_tagValue;
    std::vector<QString> m_baseTags;
};


TagEntry::TagEntry(QWidget *p, Qt::WindowFlags f):
    QWidget(p, f),
    m_tagsCombo(nullptr),
    m_tagValue(nullptr),
    m_baseTags()
{
    m_tagsCombo = new QComboBox(this);
    m_tagValue  = new QLineEdit(this);

    QHBoxLayout *mainLayout = new QHBoxLayout(this);
    mainLayout->addWidget(m_tagsCombo);
    mainLayout->addWidget(m_tagValue);
}


TagEntry::~TagEntry()
{

}


void TagEntry::setListOfBaseTags(const std::vector<QString> &tags)
{
    m_baseTags = tags;

    clear();
}


void TagEntry::selectTag(const QString &name)
{
    int idx = m_tagsCombo->findText(name);

    if (idx == -1)
        m_tagsCombo->addItem(name);
    else
        m_tagsCombo->setCurrentIndex(idx);
}


void TagEntry::setTagValue(const QString &value)
{
    m_tagValue->setText(value);
}


void TagEntry::clear()
{
    m_tagValue->clear();
    m_tagsCombo->clear();

    for (const QString &tag: m_baseTags)
        m_tagsCombo->addItem(tag);
}

/*****************************************************************************/


struct TagEditorWidget::TagsManager: public TagsManagerSlots
{
        TagsManager(TagEditorWidget* tagWidget):
            m_base_tags( {"Event", "Place", "Date", "Time", "People"}),
            m_tagWidget(tagWidget),
            m_tagEntries(),
            m_tagData(nullptr)
        {
            new QVBoxLayout(tagWidget);
            addEmptyLine();
        }

        TagsManager(const TagsManager&) = delete;
        void operator=(const TagsManager&) = delete;

        void setTags(const std::shared_ptr<ITagData>& tagData)
        {
            removeAll();
            const ITagData::TagsList& tags = tagData->getTags();

            for (auto tagIt: tags)
            {
                ITagData::TagInfo tag(tagIt);
                addLine(tag.name(), tag.valuesString());
            }
            
            m_tagData = tagData;
        }

    private:
        void addLine(const QString& name, const QString& value)
        {
            TagEntry* tagEntry = findEmptyLine();

            tagEntry->selectTag(name);
            tagEntry->setTagValue(value);
        }


        TagEntry* addEmptyLine()
        {
            TagEntry* tagEntry = new TagEntry(m_tagWidget);
            tagEntry->setListOfBaseTags(m_base_tags);
            connect( tagEntry->m_tagValue, SIGNAL(textChanged(QString)), this, SLOT(tagEdited()) );

            QLayout* lay = m_tagWidget->layout();
            lay->addWidget(tagEntry);
            m_tagEntries.push_back(tagEntry);

            return tagEntry;
        }


        void removeAll()
        {
            QLayout* lay = m_tagWidget->layout();

            while ( QLayoutItem* item = lay->takeAt(0) )
                delete item;

            m_tagEntries.clear();
        }


        TagEntry* findEmptyLine()
        {
            TagEntry* result = nullptr;

            for(TagEntry* item: m_tagEntries)
            {
                if (item->m_tagValue->text().isEmpty())
                {
                    result = item;
                    break;
                }
            }

            if (result == nullptr)
                result = addEmptyLine();

            return result;
        }
        
        
        void deleteEmptyLines()
        {
            QLayout* lay = m_tagWidget->layout();

            auto isEmpty = [&] (int i) -> bool
            {
                TagEntry* entry = m_tagEntries[i];

                const bool empty = entry->m_tagValue->text().isEmpty();

                return empty;
            };

            while ( int items = lay->count() >= 2 ? lay->count() : 0)
            {
                if ( isEmpty(items - 1) && isEmpty(items - 2) )
                    delete lay->takeAt(items - 1);
                else
                    break;
            }
        }
        
        
        void keepOneEmptyLine()
        {
            QLayout* lay = m_tagWidget->layout();
            
            if (m_tagEntries[lay->count() - 1]->m_tagValue->text().isEmpty() == false)
                addEmptyLine();
        }


        virtual void tagEdited()
        {
            //update gui
            deleteEmptyLines();
            keepOneEmptyLine();
            
            //save data
        }

/*
        virtual void setTagsAndValues(ITagData* data)
        {
            setTags(data);
        }
*/

        std::vector<QString> m_base_tags;
        TagEditorWidget* m_tagWidget;
        std::vector<TagEntry*> m_tagEntries;
        std::shared_ptr<ITagData> m_tagData;
};


/*****************************************************************************/


TagEditorWidget::TagEditorWidget(QWidget *p, Qt::WindowFlags f):
    QWidget(p, f),
    m_manager(new TagsManager(this))
{

}


TagEditorWidget::~TagEditorWidget()
{

}


void TagEditorWidget::setTags(const std::shared_ptr<ITagData>& tagData)
{
    m_manager->setTags(tagData);
}
