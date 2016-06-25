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

#include "ieditor_factory.hpp"


class EditorFactory: public IEditorFactory
{
    public:
        EditorFactory();
        EditorFactory(const EditorFactory &) = delete;
        ~EditorFactory();

        EditorFactory& operator=(const EditorFactory &) = delete;

        QWidget* createEditor(const QModelIndex &, QWidget* parent) override;
        QWidget* createEditor(const TagNameInfo::Type &, QWidget* parent) override;

        QByteArray valuePropertyName(const TagNameInfo::Type &) const;
};


struct ListEditor: QTableWidget
{
        Q_OBJECT
        Q_PROPERTY(QStringList value READ getValues WRITE setValues USER true)

    public:
        explicit ListEditor(QWidget* parent = 0);

        QStringList getValues() const;
        void setValues(const QStringList &);

    private:
        void addRow(int);
        QString value(int) const;

        void setValue(int, const QString &);

        // QWidget overrides
        QSize minimumSizeHint() const override;

    private slots:
        void review();
};

#endif // EDITORFACTORY_HPP
