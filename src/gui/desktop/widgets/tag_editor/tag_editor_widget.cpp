/*
 * Photo Broom - photos management tool.
 * Copyright (C) 2013  Michał Walenciak <MichalWalenciak@gmail.com>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License along
 * with this program; if not, write to the Free Software Foundation, Inc.,
 * 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
 */


#include "tag_editor_widget.hpp"

#include <QVBoxLayout>
#include <QComboBox>
#include <QLineEdit>
#include <QPushButton>
#include <QItemEditorFactory>

#include <core/base_tags.hpp>

#include "helpers/tags_view.hpp"
#include "helpers/tags_model.hpp"


TagEditorWidget::TagEditorWidget(QWidget* p, Qt::WindowFlags f):
    QWidget(p, f),
    m_editorFactory(),
    m_view(nullptr),
    m_model(nullptr),
    m_tagsOperator(),
    m_tagValueProp()
{
    m_model = new TagsModel(this);
    m_model->set(&m_tagsOperator);

    m_view = new TagsView(m_editorFactory, this);
    m_view->setModel(m_model);

    QVBoxLayout* l = new QVBoxLayout(this);
    l->addWidget(m_view);
}


TagEditorWidget::~TagEditorWidget()
{

}


void TagEditorWidget::setDatabase(Database::IDatabase* db)
{
    m_model->set(db);
}


void TagEditorWidget::set(QItemSelectionModel* selectionModel)
{
    m_model->set(selectionModel);
}


void TagEditorWidget::set(DBDataModel* dbDataModel)
{
    m_model->set(dbDataModel);
}


void TagEditorWidget::set(ICompleterFactory* completerFactory)
{
    m_editorFactory.set(completerFactory);
}
