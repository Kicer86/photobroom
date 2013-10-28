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
#include <iterator>

#include <QString>
#include <QLineEdit>
#include <QPushButton>
#include <QVBoxLayout>
#include <QDebug>

#include "core/types.hpp"

#include "tag_definition.hpp"
#include "tag_entry.hpp"
#include "entries_manager.hpp"


struct TagEditorWidget::TagsManager: public TagsManagerSlots
{
        TagsManager(TagEditorWidget* tagWidget):
            TagsManagerSlots(tagWidget),
            m_base_tags( {"Event", "Place", "Date", "Time", "People"}),
            m_tagWidget(tagWidget),
            m_tagData(nullptr),
            m_entriesManager(new EntriesManager(this)),
            m_container(nullptr),
            m_tag(nullptr),
            m_model(new QStringListModel(this))
        {
            QLayout* layout = new QVBoxLayout(tagWidget);
            m_tag = new TagDefinition(tagWidget);
            m_container = new QWidget(tagWidget);
            
            new QVBoxLayout(m_container);
            
            layout->addWidget(m_tag);
            layout->addWidget(m_container);
            
            connect(m_tag, SIGNAL(tagChoosen(QString)), this, SLOT(addLine(QString)));
            
            updateAvailableTags();
            m_tag->setModel(m_model);
        }

        TagsManager(const TagsManager&) = delete;
        void operator=(const TagsManager&) = delete;

        void setTags(const std::shared_ptr<ITagData>& tagData)
        {
            if (m_tagData.get() != nullptr)
                std::cout << "saving tags: " << (*m_tagData) << std::endl;
            
            m_tagData.reset();
            
            m_entriesManager->removeAllEntries();
            const ITagData::TagsList& tags = tagData->getTags();

            for (auto tagIt: tags)
            {
                ITagData::TagInfo tag(tagIt);
                addLine(tag.name(), tag.valuesString());
            }
                                    
            m_tagData = tagData;
                   
            std::cout << "got tags: " << (*m_tagData) << std::endl;
        }

    private:
        virtual void addLine(const QString& name) override
        {
            addLine(name, "");
        }
        
        
        void addLine(const QString& name, const QString& value)
        {
            TagEntry* tagEntry = m_entriesManager->constructEntry(name, m_tagWidget);
            
            connect(tagEntry, SIGNAL(tagEdited()), this, SLOT(tagEdited()));

            storeTagEntry(tagEntry);
            tagEntry->setTagValue(value);
            
            updateAvailableTags();
        }

        void storeTagEntry(TagEntry* tagEntry)
        {
            QLayout* lay = m_container->layout();
            lay->addWidget(tagEntry);
        }
   
        virtual void tagEdited()
        {            
            if (m_tagData != nullptr)
            {
                m_tagData->clear();
                auto& entries = m_entriesManager->getTagEntries();
                for (const std::unique_ptr<TagEntry>& tagEntry: entries)
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
        
        void updateAvailableTags() const
        {
            const std::set<QString> tags = m_entriesManager->getDefaultValues();
            QStringList data;
            
            for (auto& t: tags)
                data << t;
            
            m_model->setStringList(data);
        }

        std::vector<QString> m_base_tags;
        TagEditorWidget* m_tagWidget;
        std::shared_ptr<ITagData> m_tagData;
        EntriesManager* m_entriesManager;
        QWidget* m_container;
        TagDefinition* m_tag;
        QStringListModel* m_model;
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
