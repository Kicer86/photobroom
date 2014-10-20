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

#include "project_creator_dialog.hpp"

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
#include <QString>

#include <QtExt/qtext_choosefile.hpp>

#include <core/iplugin_loader.hpp>
#include <database/idatabase_plugin.hpp>


Q_DECLARE_METATYPE(Database::IPlugin *)


struct PrjLocationDialog: QtExtChooseFileDialog
{
    PrjLocationDialog(): m_result() {}

    virtual int exec()
    {
        m_result = QFileDialog::getSaveFileName(nullptr, tr("File name"),
                                                "", tr("Photo Broom albums (*.bpj)")
        );

        return m_result.isEmpty()? QDialog::Rejected: QDialog::Accepted;
    }

    virtual QString result() const
    {
        return m_result;
    }

    QString m_result;
};


ProjectCreatorDialog::ProjectCreatorDialog(): QDialog(),
                                              m_chooseDialog(nullptr),
                                              m_prjLocation(nullptr),
                                              m_engines(nullptr),
                                              m_engineOptions(nullptr),
                                              m_pluginLoader(nullptr),
                                              m_plugins()
{
    setWindowTitle(tr("Album creator"));
    resize(500, 250);

    //project location line
    QLabel* prjLocationLabel = new QLabel(tr("Album location:"), this);
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
    m_engineOptions = new QGroupBox(tr("Engine options"));

    //default buttons
    QDialogButtonBox* defaultButtons = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel);
    connect(defaultButtons, SIGNAL(accepted()), this, SLOT(accept()));
    connect(defaultButtons, SIGNAL(rejected()), this, SLOT(reject()));

    //main layout
    QVBoxLayout* mainLayout = new QVBoxLayout(this);
    mainLayout->addLayout(prjLocationLayout);
    mainLayout->addLayout(dbEngineLayout);
    mainLayout->addWidget(m_engineOptions);
    mainLayout->addStretch();
    mainLayout->addWidget(defaultButtons);
}


ProjectCreatorDialog::~ProjectCreatorDialog()
{

}


void ProjectCreatorDialog::set(IPluginLoader* pluginLoader)
{
    m_pluginLoader = pluginLoader;
    initEngines();
}


QString ProjectCreatorDialog::getPrjPath() const
{
    QString prjPath = m_prjLocation->text();
    const QFileInfo resultInfo(prjPath);

    if (resultInfo.suffix() != "bpj")
        prjPath += ".bpj";

    return prjPath;
}


Database::IPlugin* ProjectCreatorDialog::getEnginePlugin() const
{
    Database::IPlugin* plugin = getSelectedPlugin();

    return plugin;
}


void ProjectCreatorDialog::initEngines()
{
    const std::deque<Database::IPlugin *>& plugins = m_pluginLoader->getDBPlugins();

    for(Database::IPlugin* plugin: plugins)
        m_plugins[plugin->backendName()] = plugin;

    QStackedLayout* engineOptionsLayout = new QStackedLayout(m_engineOptions);

    for(auto plugin: m_plugins)
    {
        const QVariant itemData = QVariant::fromValue<Database::IPlugin *>(plugin.second);
        m_engines->addItem(plugin.first, itemData);

        QWidget* optionsWidget = new QWidget(this);
        QLayout* optionsWidgetLayout = plugin.second->buildDBOptions();

        if (optionsWidgetLayout != nullptr)
            optionsWidget->setLayout(optionsWidgetLayout);

        engineOptionsLayout->addWidget(optionsWidget);
    }

    connect(m_engines, SIGNAL(activated(int)), engineOptionsLayout, SLOT(setCurrentIndex(int)));
}


Database::IPlugin* ProjectCreatorDialog::getSelectedPlugin() const
{
    const QVariant pluginRaw = m_engines->currentData();
    Database::IPlugin* plugin = pluginRaw.value<Database::IPlugin *>();

    return plugin;
}
