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
        QWidget* createEditor(const TagNameInfo &, QWidget* parent) override;

        QByteArray valuePropertyName(const TagNameInfo &) const;

    private:
        ICompleterFactory* m_completerFactory;
};


struct ListEditor: QTableWidget
{
        Q_OBJECT
        Q_PROPERTY(QStringList value READ getValues WRITE setValues USER true)

    public:
        explicit ListEditor(QWidget* parent = 0);
        ListEditor(const ListEditor &) = delete;
        virtual ~ListEditor();

        ListEditor& operator=(const ListEditor &) = delete;

        QStringList getValues() const;
        void setValues(const QStringList &);

        void setCompleter(QCompleter *);

    private:
        std::set<QLineEdit *> m_editors;
        QCompleter* m_completer;

        void addRow(int);
        QString value(int) const;

        void setValue(int, const QString &);

        // QWidget overrides
        QSize minimumSizeHint() const override;

        //
        void review();
        void editorDestroyed(QObject *) override;
};

#endif // EDITORFACTORY_HPP
