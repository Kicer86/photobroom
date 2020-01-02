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

#include "config_dialog_manager.hpp"

#include "ui/configuration_dialog.hpp"


ConfigDialogManager::ConfigDialogManager(): m_tabs()
{

}


ConfigDialogManager::~ConfigDialogManager()
{

}


void ConfigDialogManager::run()
{
    ConfigurationDialog config;

    for(auto& tab: m_tabs)
    {
        IConfigTab* confTab = tab.second;

        QWidget* w = confTab->constructTab();
        const QString name = confTab->tabName();

        config.addTab(name, w);
    }

    connect(&config, &ConfigurationDialog::saveData, this, &ConfigDialogManager::applyConfiguration);

    config.exec();
}


void ConfigDialogManager::registerTab(IConfigTab* tab)
{
    const int id = tab->tabId();

    m_tabs[id] = tab;
}


void ConfigDialogManager::unregisterTab(IConfigTab* tab)
{
    const int id = tab->tabId();

    m_tabs.erase(id);
}


void ConfigDialogManager::applyConfiguration()
{
    for(const auto& tab: m_tabs)
        tab.second->applyConfiguration();
}


void ConfigDialogManager::initDialog()
{

}
