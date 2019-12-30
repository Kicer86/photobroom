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

#ifndef CONFIGDIALOGMANAGER_HPP
#define CONFIGDIALOGMANAGER_HPP

#include "iconfig_dialog_manager.hpp"

class ConfigDialogManager: public QObject, public IConfigDialogManager
{
    public:
        ConfigDialogManager();
        ConfigDialogManager(const ConfigDialogManager &) = delete;
        ~ConfigDialogManager();

        ConfigDialogManager& operator=(const ConfigDialogManager &) = delete;

        void run();

        // IConfigDialogManager:
        virtual void registerTab(IConfigTab *) override;
        virtual void unregisterTab(IConfigTab *) override;

    private:
        std::map<int, IConfigTab *> m_tabs;

        void applyConfiguration();
        void initDialog();
};

#endif // CONFIGDIALOGMANAGER_HPP
