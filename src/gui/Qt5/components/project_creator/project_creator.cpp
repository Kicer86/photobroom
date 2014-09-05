/*
 * Dialog for new projects creation
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

#include "project_creator.hpp"

#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QPushButton>
#include <QLineEdit>
#include <QFileDialog>
#include <QDialogButtonBox>
#include <QLabel>
#include <QComboBox>
#include <QGroupBox>
#include <QStackedLayout>

#include <QtExt/qtext_choosefile.hpp>

#include <core/iplugin_loader.hpp>
#include <database/idatabase_plugin.hpp>

struct PrjLocationDialog: QtExtChooseFileDialog
{
    PrjLocationDialog(): m_result() {}

    virtual int exec()
    {
        m_result = QFileDialog::getSaveFileName(nullptr, tr("Project File Name"),
                                                "", tr("Broom project (*.bpj)")
        );

        return m_result.isEmpty()? QDialog::Rejected: QDialog::Accepted;
    }

    virtual QString result() const
    {
        return m_result;
    }

    QString m_result;
};


ProjectCreator::ProjectCreator(): QDialog(),
                                  m_chooseDialog(nullptr),
                                  m_prjLocation(nullptr),
                                  m_engines(nullptr),
                                  m_dbOptions(nullptr),
                                  m_pluginLoader(nullptr),
                                  m_plugins()
{
    setWindowTitle(tr("Project creator"));
    resize(500, 250);

    //project location line
    QLabel* prjLocationLabel = new QLabel(tr("Project location:"), this);
    QPushButton* prjLocationBrowseButton = new QPushButton(tr("Browse"), this);
    m_prjLocation = new QLineEdit(this);
    m_chooseDialog = new QtExtChooseFile(prjLocationBrowseButton, m_prjLocation, new PrjLocationDialog);

    //project location line layout
    QHBoxLayout* prjLocationLayout = new QHBoxLayout;
    prjLocationLayout->addWidget(prjLocationLabel);
    prjLocationLayout->addWidget(m_prjLocation);
    prjLocationLayout->addWidget(prjLocationBrowseButton);

    //storage engine
    QLabel* dbEngine = new QLabel(tr("Database engine:"), this);
    m_engines = new QComboBox(this);

    //storage engine layout
    QHBoxLayout* dbEngineLayout = new QHBoxLayout;
    dbEngineLayout->addWidget(dbEngine);
    dbEngineLayout->addWidget(m_engines);

    //storage options
    m_dbOptions = new QGroupBox(tr("Engine options"));

    //default buttons
    QDialogButtonBox* defaultButtons = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel);
    connect(defaultButtons, SIGNAL(accepted()), this, SLOT(accept()));
    connect(defaultButtons, SIGNAL(rejected()), this, SLOT(reject()));

    //main layout
    QVBoxLayout* mainLayout = new QVBoxLayout(this);
    mainLayout->addLayout(prjLocationLayout);
    mainLayout->addLayout(dbEngineLayout);
    mainLayout->addWidget(m_dbOptions);
    mainLayout->addStretch();
    mainLayout->addWidget(defaultButtons);
}


ProjectCreator::~ProjectCreator()
{

}


void ProjectCreator::set(IPluginLoader* pluginLoader)
{
    m_pluginLoader = pluginLoader;
    initEngines();
}


void ProjectCreator::initEngines()
{
    const std::deque<Database::IPlugin *>& plugins = m_pluginLoader->getDBPlugins();

    for(Database::IPlugin* plugin: plugins)
        m_plugins[plugin->backendName()] = plugin;

    QStackedLayout* engineOptionsLayout = new QStackedLayout(m_dbOptions);

    for(auto plugin: m_plugins)
    {
        m_engines->addItem(plugin.first);

        QWidget* optionsWidget = new QWidget(this);
        QLayout* optionsWidgetLayout = plugin.second->buildDBOptions();

        if (optionsWidgetLayout != nullptr)
            optionsWidget->setLayout(optionsWidgetLayout);

        engineOptionsLayout->addWidget(optionsWidget);
    }

    connect(m_engines, SIGNAL(activated(int)), engineOptionsLayout, SLOT(setCurrentIndex(int)));
}
