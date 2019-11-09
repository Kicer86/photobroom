/*
 * Editor factory for views
 * Copyright (C) 2015  Michał Walenciak <MichalWalenciak@gmail.com>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 */

#ifndef EDITORFACTORY_HPP
#define EDITORFACTORY_HPP

#include <QItemEditorFactory>
#include <QTableWidget>

#include <core/tag.hpp>
#include "ieditor_factory.hpp"

class QCompleter;

struct ICompleterFactory;

class EditorFactory: public IEditorFactory
{
    public:
        EditorFactory();
        EditorFactory(const EditorFactory &) = delete;
        ~EditorFactory();

        EditorFactory& operator=(const EditorFactory &) = delete;

        void set(ICompleterFactory *);

        QWidget* createEditor(const QModelIndex &, QWidget* parent) override;
        QWidget* createEditor(const TagTypeInfo &, QWidget* parent) override;
        QByteArray valuePropertyName(const TagTypeInfo &) const override;

    private:
        ICompleterFactory* m_completerFactory;
};

#endif // EDITORFACTORY_HPP
