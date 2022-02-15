/*
 * Photo Broom - photos management tool.
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

#include "tasks_view_widget.hpp"

#include <cassert>

#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QScrollArea>
#include <QProgressBar>
#include <QLabel>

#include <core/iview_task.hpp>
#include <core/function_wrappers.hpp>
#include <core/qmodel_utils.hpp>


namespace
{

    enum Roles
    {
        MinValueRole = Qt::UserRole + 1,
        MaxValueRole,
        ValueRole,
        NameRole,
    };

    struct Task: QObject, IViewTask, IProgressBar
    {
        Task(const QString& name, QStandardItem* item):
            m_name(name),
            m_item(item)
        {
            setRole(NameRole, name);
        }

        // IViewTask overrides:
        const QString& getName() override
        {
            return m_name;
        }

        IProgressBar* getProgressBar() override
        {
            return this;
        }

        void finished() override
        {
            const int row = m_item->row();
            QStandardItemModel* model = m_item->model();
            model->removeRow(row);
            m_item = nullptr;

            deleteLater();
        }

        // IProgressBar overrides:
        void setMaximum(int v) override
        {
            setRole(Roles::MaxValueRole, v);
        }

        void setMinimum(int v) override
        {
            setRole(Roles::MinValueRole, v);
        }

        void setValue(int v) override
        {
            setRole(Roles::ValueRole, v);
        }

        template<typename T>
        void setRole(int role, const T& value)
        {
            QMetaObject::invokeMethod(this, [this, role, value]
            {
                if (m_item)
                    m_item->setData(value, role);
            });
        }

        QString m_name;
        QStandardItem* m_item;
    };
}


TasksViewWidget::TasksViewWidget()
{

}


TasksViewWidget::~TasksViewWidget()
{

}


QHash<int, QByteArray> TasksViewWidget::roleNames() const
{
    QHash<int, QByteArray> names = QAbstractItemModel::roleNames();

    names.insert(parseRoles<Roles>());

    return names;
}


IViewTask* TasksViewWidget::add(const QString& name)
{
    QStandardItem* item = new QStandardItem;
    Task* task = new Task(name, item);

    appendRow(item);

    return task;
}
