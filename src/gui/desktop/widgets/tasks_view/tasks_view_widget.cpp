/*
 * Widget for list of tasks.
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

#include "itask.hpp"


struct TasksViewWidget::Task: QWidget, ITask
{
    Task(const QString& name, TasksViewWidget* parent): QWidget(parent), ITask(), m_name(name), m_progressBar(nullptr), m_parent(parent)
    {
        m_progressBar = new QProgressBar(this);
        const QString format = QString("%1%p%").arg(name);
        m_progressBar->setFormat(format);
    }

    Task(const Task &) = delete;
    Task& operator=(const Task &) = delete;

    const QString& getName() override
    {
        return m_name;
    }

    QProgressBar* getProgressBar() override
    {
        return m_progressBar;
    }

    void finished() override
    {
        m_parent->finished(this);
    }

    QString m_name;
    QProgressBar* m_progressBar;
    TasksViewWidget* m_parent;
};


TasksViewWidget::TasksViewWidget(QWidget* p): QWidget(p), m_view(nullptr)
{
    m_view = new QScrollArea(this);
    m_view->setWidgetResizable(true);

    QWidget* mainWidget = new QWidget(this);
    new QVBoxLayout(mainWidget);

    m_view->setWidget(mainWidget);

    QVBoxLayout* mainLayout = new QVBoxLayout(this);
    mainLayout->addWidget(m_view);
}


TasksViewWidget::~TasksViewWidget()
{

}


ITask* TasksViewWidget::add(const QString& name)
{
    Task* task = new Task(name, this);

    getLayout()->addWidget(task);

    return task;
}


void TasksViewWidget::finished(ITask* task)
{
    Task* t = static_cast<Task *>(task);

    t->deleteLater();
}


QBoxLayout* TasksViewWidget::getLayout()
{
    QWidget* w = m_view->widget();
    QLayout* l = w->layout();

    assert(dynamic_cast<QBoxLayout*>(l));
    QBoxLayout *bl = static_cast<QBoxLayout *>(l);

    return bl;
}
