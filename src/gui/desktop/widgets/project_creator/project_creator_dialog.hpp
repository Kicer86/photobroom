/*
 * Photo Broom - photos management tool.
 * Copyright (C) 2014  Michał Walenciak <MichalWalenciak@gmail.com>
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

#ifndef PROJECTCREATOR_H
#define PROJECTCREATOR_H

#include <QDialog>

#include <project_utils/project.hpp>

class QComboBox;
class QtExtChooseFile;
class QLineEdit;
class QGroupBox;
class QDialogButtonBox;

class QtExtChooseFile;

struct IPluginLoader;
class IProjectManager;

namespace Database
{
    struct IPlugin;
}

class ProjectCreatorDialog: public QDialog
{
        Q_OBJECT

    public:
        ProjectCreatorDialog();
        ProjectCreatorDialog(const ProjectCreatorDialog& other) = delete;
        ~ProjectCreatorDialog();

        ProjectCreatorDialog& operator=(const ProjectCreatorDialog& other) = delete;

        void set(IPluginLoader *);

        // getters:
        QString getPrjName() const;
        QString getLocation() const;
        Database::IPlugin* getEnginePlugin() const;

    private:
        QLineEdit* m_prjName;
        QComboBox* m_engines;
        QGroupBox* m_engineOptions;
        QtExtChooseFile* m_location;
        QDialogButtonBox* m_defaultButtons;
        IPluginLoader* m_pluginLoader;

        void initEngines();
        Database::IPlugin* getSelectedPlugin() const;

        // slots:
        void updateButtons();
};


//runs ProjectCreator and adds project to IProjectManager
class ProjectCreator
{
    public:
        ProjectCreator();

        bool create(IProjectManager *, IPluginLoader *);
        const ProjectInfo& project() const;

    private:
        ProjectInfo m_prj;
};

#endif // PROJECTCREATOR_H
