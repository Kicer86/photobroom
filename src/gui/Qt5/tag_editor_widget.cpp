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
#include <QComboBox>
#include <QLineEdit>
#include <QPushButton>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QStringListModel>

#include "core/types.hpp"

struct TagEntry;

struct EntriesManager: public EntriesManagerSlots
{
        explicit EntriesManager(QObject* parent = 0);
    
        TagEntry* constructEntry(QWidget *);
        
        QString getDefaultValue();
        std::set<QString> getDefaultValues();
        
    private:
        std::vector<TagEntry *> m_entries;
        static std::set<QString> m_base_tags;        
        QStringListModel m_combosModel;
        
        std::set<QString> usedValues() const;
        void registerEmtry(TagEntry *);
        
    private slots:
        void comboChanged() override;
};

struct TagEntry: public QWidget
{
        friend class EntriesManager;
        
        virtual ~TagEntry();

        TagEntry(const TagEntry &) = delete;
        void operator=(const TagEntry &) = delete;

        void selectTag(const QString &name);
        void setTagValue(const QString &value);
        void clear();
        
        QString getTagName() const;
        QString getTagValue() const;

    private:
        QComboBox   *m_tagsCombo;
        QLineEdit   *m_tagValue;
        
        explicit TagEntry(QWidget *parent, Qt::WindowFlags f = 0);
        
    signals:
        void tagEdited();
};


/**************************************************************************/


std::set<QString> EntriesManager::m_base_tags( {"Event", "Place", "Date", "Time", "People"} );


EntriesManager::EntriesManager(QObject* parent): EntriesManagerSlots(parent), m_entries(), m_combosModel()
{
    comboChanged();
}


TagEntry* EntriesManager::constructEntry(QWidget *p)
{
    TagEntry* result = new TagEntry(p);
    registerEmtry(result);
    
    result->m_tagsCombo->setModel(&m_combosModel);
    
    return result;
}



void EntriesManager::registerEmtry(TagEntry* entry)
{
    m_entries.push_back(entry);
    
    connect(entry->m_tagsCombo, SIGNAL(currentIndexChanged(const QString &)), this, SLOT(comboChanged()));
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
    
    m_combosModel.setStringList(data);
}


/**************************************************************************/



TagEntry::TagEntry(QWidget *p, Qt::WindowFlags f):
    QWidget(p, f),
    m_tagsCombo(nullptr),
    m_tagValue(nullptr)
{
    m_tagsCombo = new QComboBox(this);
    m_tagValue  = new QLineEdit(this);

    QHBoxLayout *mainLayout = new QHBoxLayout(this);
    mainLayout->addWidget(m_tagsCombo);
    mainLayout->addWidget(m_tagValue);
    
    m_tagsCombo->setEditable(true);
    
    connect(m_tagValue, SIGNAL(textEdited(QString)), this, SIGNAL(tagEdited()));
}


TagEntry::~TagEntry()
{

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

    //for (const QString &tag: m_baseTags)
    //    m_tagsCombo->addItem(tag);
}


QString TagEntry::getTagName() const
{
    const QString result = m_tagsCombo->currentText();
    
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
            new QVBoxLayout(tagWidget);
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
            
            deleteRedundantLines();
                        
            m_tagData = tagData;
            
            if (m_tagData.get() != nullptr)   //no one empty line when no selection
                keepOneEmptyLine();
            
            std::cout << "got tags: " << (*m_tagData) << std::endl;
        }

    private:
        void addLine(const QString& name, const QString& value)
        {
            TagEntry* tagEntry = addEmptyLine();

            tagEntry->selectTag(name);
            tagEntry->setTagValue(value);
        }


        TagEntry* addEmptyLine()
        {
            TagEntry* tagEntry = m_entriesManager->constructEntry(m_tagWidget);
            //tagEntry->setListOfBaseTags(m_base_tags);
            connect( tagEntry, SIGNAL(tagEdited()), this, SLOT(tagEdited()) );

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
        
        
        void keepOneEmptyLine()
        {
            QLayout* lay = m_tagWidget->layout();
            
            if ( lay->count() == 0 || m_tagEntries[lay->count() - 1]->getTagValue().isEmpty() == false)
                addEmptyLine();
        }


        virtual void tagEdited()
        {
            //update gui
            deleteRedundantLines();
            keepOneEmptyLine();
            
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
