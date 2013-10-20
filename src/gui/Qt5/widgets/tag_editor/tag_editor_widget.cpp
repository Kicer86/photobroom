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
#include <iostream>
#include <set>
#include <algorithm>
#include <iterator>

#include <QString>
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QStringListModel>
#include <QDebug>

#include "tag_definition.hpp"

#include "core/types.hpp"

struct TagEntry;

struct EntriesManager: public QObject
{
        explicit EntriesManager(QObject* parent = 0);
    
        TagEntry* constructEntry(const QString& name, QWidget* p);
        
        QString getDefaultValue();
        std::set<QString> getDefaultValues();
        
    private:
        std::vector<TagEntry *> m_entries;
        static std::set<QString> m_base_tags;        
        QStringListModel m_combosModel;
        QStringList  m_data;
        
        std::set<QString> usedValues() const;
        void registerEntry(TagEntry *);
};

struct TagEntry: public TagEntrySignals
{
        friend class EntriesManager;
        
        virtual ~TagEntry();

        TagEntry(const TagEntry &) = delete;
        void operator=(const TagEntry &) = delete;

        //void selectTag(const QString &name);
        void setTagValue(const QString &value);
        void clear();
        
        QString getTagName() const;
        QString getTagValue() const;

    private:
        QLabel    *m_tagName;
        QLineEdit *m_tagValue;
        
        explicit TagEntry(const QString &, QWidget *parent, Qt::WindowFlags f = 0);
};


/**************************************************************************/


std::set<QString> EntriesManager::m_base_tags( {"Event", "Place", "Date", "Time", "People"} );


EntriesManager::EntriesManager(QObject* parent): QObject(parent), m_entries(), m_combosModel(), m_data()
{

}


TagEntry* EntriesManager::constructEntry(const QString& name, QWidget *p)
{
    TagEntry* result = new TagEntry(name, p);
    registerEntry(result);
    
    return result;
}



void EntriesManager::registerEntry(TagEntry* entry)
{
    m_entries.push_back(entry);
}


QString EntriesManager::getDefaultValue()
{    
    std::set<QString> avail = m_base_tags;
    
    for (TagEntry* entry: m_entries)
    {
        const QString n = entry->getTagName();
        
        avail.erase(n);
    }
    
    QString result = "";
    
    if (avail.empty() == false)
        result = *(avail.begin());
    
    return result;
}


std::set<QString> EntriesManager::usedValues() const
{
    std::set<QString> used;
    for (TagEntry* entry: m_entries)
    {
        const QString n = entry->getTagName();
        
        used.insert(n);
    }
    
    return used;
}

/*
void EntriesManager::comboChanged()
{
    typedef std::set<QString> set_type;
    
    set_type usedNames;
    
    for(TagEntry* entry: m_entries)
    {
        QString name = entry->getTagName();
        
        const set_type::size_type usedNamesSize = usedNames.size();
        
        usedNames.insert(name);
        
        if (usedNamesSize == usedNames.size())
        {
            //TODO: mark comboBoxes with red
        }
    }
    
    //remove used names from model
    set_type availNames;
    
    std::set_difference(m_base_tags.begin(), m_base_tags.end(),
                        usedNames.begin(), usedNames.end(),
                        std::inserter(availNames, availNames.begin()));
    
    QStringList data;
    for(auto i: availNames)
        data << i;
    
    qDebug() << m_data << "\n" << data;
    
    if (m_data != data)
        m_combosModel.setStringList(m_data = data);
}
*/

/**************************************************************************/


TagEntry::TagEntry(const QString &name, QWidget *p, Qt::WindowFlags f):
    TagEntrySignals(p, f),
    m_tagName(nullptr),
    m_tagValue(nullptr)
{
    m_tagName = new QLabel(name, this);
    m_tagValue  = new QLineEdit(this);

    QHBoxLayout *mainLayout = new QHBoxLayout(this);
    mainLayout->addWidget(m_tagName);
    mainLayout->addWidget(m_tagValue);
    
    connect(m_tagValue, SIGNAL(textEdited(QString)), this, SIGNAL(tagEdited()));
}


TagEntry::~TagEntry()
{

}

/*
void TagEntry::selectTag(const QString &name)
{
    int idx = m_tagName->findText(name);

    if (idx == -1)
        m_tagName->addItem(name);
    else
        m_tagName->setCurrentIndex(idx);
}
*/

void TagEntry::setTagValue(const QString &value)
{
    m_tagValue->setText(value);
}


void TagEntry::clear()
{
    m_tagValue->clear();
    m_tagName->clear();

    //for (const QString &tag: m_baseTags)
    //    m_tagsCombo->addItem(tag);
}


QString TagEntry::getTagName() const
{
    const QString result = m_tagName->text();    
    return result;
}


QString TagEntry::getTagValue() const
{   
    const QString result = m_tagValue->text();
    
    return result;
}



/*****************************************************************************/


struct TagEditorWidget::TagsManager: public TagsManagerSlots
{
        TagsManager(TagEditorWidget* tagWidget):
            TagsManagerSlots(tagWidget),
            m_base_tags( {"Event", "Place", "Date", "Time", "People"}),
            m_tagWidget(tagWidget),
            m_tagEntries(),
            m_tagData(nullptr),
            m_entriesManager(new EntriesManager(this))
        {
            QLayout* layout = new QVBoxLayout(tagWidget);
            TagDefinition* tag = new TagDefinition(tagWidget);
            m_container = new QWidget(tagWidget);
            
            QLayout* containerLayout = new QVBoxLayout(m_container);
            
            layout->addWidget(tag);
            layout->addWidget(m_container);
        }

        TagsManager(const TagsManager&) = delete;
        void operator=(const TagsManager&) = delete;

        void setTags(const std::shared_ptr<ITagData>& tagData)
        {
            if (m_tagData.get() != nullptr)
                std::cout << "saving tags: " << (*m_tagData) << std::endl;
            
            m_tagData.reset();
            
            removeAll();
            const ITagData::TagsList& tags = tagData->getTags();

            for (auto tagIt: tags)
            {
                ITagData::TagInfo tag(tagIt);
                addLine(tag.name(), tag.valuesString());
            }
            
            //deleteRedundantLines();
                        
            m_tagData = tagData;
            
            /*
            if (m_tagData.get() != nullptr)   //no one empty line when no selection
                keepOneEmptyLine();
            */
            
            std::cout << "got tags: " << (*m_tagData) << std::endl;
        }

    private:
        void addLine(const QString& name, const QString& value = "")
        {
            TagEntry* tagEntry = m_entriesManager->constructEntry(name, m_tagWidget);
            
            connect(tagEntry, SIGNAL(tagEdited()), this, SLOT(tagEdited()));

            QLayout* lay = m_container->layout();
            lay->addWidget(tagEntry);
            m_tagEntries.push_back(tagEntry);

            tagEntry->setTagValue(value);
        }


        void removeAll()
        {
            QLayout* lay = m_container->layout();

            while ( QLayoutItem* item = lay->takeAt(0) )
                delete item;

            m_tagEntries.clear();
        }
        
        
        /*
        void deleteRedundantLines()
        {
            QLayout* lay = m_tagWidget->layout();

            auto isEmpty = [&] (int i) -> bool
            {
                TagEntry* entry = m_tagEntries[i];

                const bool empty = entry->getTagValue().isEmpty();

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
        */
        
        
        /*
        void keepOneEmptyLine()
        {
            QLayout* lay = m_tagWidget->layout();
            
            if ( lay->count() == 0 || m_tagEntries[lay->count() - 1]->getTagValue().isEmpty() == false)
                addEmptyLine();
        }
        */


        virtual void tagEdited()
        {
            //update gui
            //deleteRedundantLines();
            //keepOneEmptyLine();
            
            //save data
            //TagEntry* edit = getEditedTag();
            
            if (m_tagData != nullptr)
            {
                m_tagData->clear();
                for (TagEntry* tagEntry: m_tagEntries)
                {
                    const QString name = tagEntry->getTagName();
                    const QString value = tagEntry->getTagValue();
                    const QStringList values = value.split(";");  //use some constants                
                    const ITagData::ValuesSet vSet(values.begin(), values.end());
                    
                    m_tagData->setTag(name, vSet);
                }
                
                std::cout << *m_tagData << std::endl;
            }
        }
        
        
        TagEntry* getEditedTag() const
        {
            QObject* obj = QObject::sender();
            assert(qobject_cast<QLineEdit *>(obj) != nullptr);
            
            QLineEdit* lineEdit = static_cast<QLineEdit *>(obj);
            QWidget* lineEditParent = lineEdit->parentWidget();
            assert(dynamic_cast<TagEntry *>(lineEditParent) != nullptr);
            
            TagEntry* tagEntry = static_cast<TagEntry *>(lineEditParent);
            
            return tagEntry;
        }

/*
        virtual void setTagsAndValues(ITagData* data)
        {
            setTags(data);
        }
*/

        std::vector<QString> m_base_tags;
        TagEditorWidget* m_tagWidget;
        std::vector<TagEntry *> m_tagEntries;
        std::shared_ptr<ITagData> m_tagData;
        EntriesManager* m_entriesManager;
        QWidget* m_container;
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
