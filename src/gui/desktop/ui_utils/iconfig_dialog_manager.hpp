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

#ifndef ICONFIGDIALOGMANAGER_HPP
#define ICONFIGDIALOGMANAGER_HPP


#include <QObject>


struct IConfigTab
{
    virtual ~IConfigTab();

    virtual int tabId() const = 0;
    virtual QString tabName() const = 0;

    virtual QWidget* constructTab() = 0;
    virtual void applyConfiguration() = 0;
    virtual void rejectConfiguration() = 0;
};


struct IConfigDialogManager
{
    virtual ~IConfigDialogManager();

    virtual void registerTab(IConfigTab *) = 0;
    virtual void unregisterTab(IConfigTab *) = 0;
};

#endif // ICONFIGDIALOGMANAGER_HPP
