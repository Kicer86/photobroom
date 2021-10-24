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

#include "project_creator_dialog.hpp"

#include <set>

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

#include <database/idatabase_plugin.hpp>
#include <plugins/iplugin_loader.hpp>
#include <project_utils/iproject_manager.hpp>
#include "widgets/qtext_choosefile.hpp"


namespace
{
    struct PluginOrder
    {
        bool operator() (const Database::IPlugin* p1, const Database::IPlugin* p2) const
        {
            return p1->simplicity() > p2->simplicity();
        }
    };
}


ProjectCreatorDialog::ProjectCreatorDialog(): QDialog(),
                                              m_prjName(nullptr),
                                              m_engines(nullptr),
                                              m_engineOptions(nullptr),
                                              m_location(nullptr),
                                              m_defaultButtons(nullptr),
                                              m_pluginLoader(nullptr)
{
    setWindowTitle(tr("Photo collection creator"));
    resize(500, 250);

    //project location line
    QLabel* prjNameLabel = new QLabel(tr("Collection name:"), this);
    m_prjName = new QLineEdit(this);

    //project location line layout
    QHBoxLayout* prjNameLayout = new QHBoxLayout;
    prjNameLayout->addWidget(prjNameLabel);
    prjNameLayout->addWidget(m_prjName);

    //storage engine
    QLabel* dbEngine = new QLabel(tr("Database engine:"), this);
    m_engines = new QComboBox(this);

    //storage engine layout
    QHBoxLayout* dbEngineLayout = new QHBoxLayout;
    dbEngineLayout->addWidget(dbEngine);
    dbEngineLayout->addWidget(m_engines);

    //storage options
    m_engineOptions = new QGroupBox(tr("Engine options"));

    // photos location
    QGroupBox* locationGroup = new QGroupBox(tr("Photos storage location"));
    QVBoxLayout* locationLayout = new QVBoxLayout(locationGroup);
    QLabel* locationInfo = new QLabel(tr("<small>Photos storage location is a place where all your photos and videos will be kept.<br>"
                                         "Also Photo Broom's database will be kept there.<br>"
                                         "Location may already contain photos and videos which will be automatically loaded.<br>"
                                         "<b>Photo Broom does not modify or move your files. "
                                         "All applied informations are stored in database not in files itself.</b></small>")
    );

    locationInfo->setTextFormat(Qt::RichText);

    m_location = new QtExtChooseFile(tr("Location"), tr("Browse"), [this]
    {
        return QFileDialog::getExistingDirectory(this, tr("Photos location"));
    });

    locationLayout->addWidget(locationInfo);
    locationLayout->addWidget(m_location);

    //default buttons
    m_defaultButtons = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel);
    connect(m_defaultButtons, SIGNAL(accepted()), this, SLOT(accept()));
    connect(m_defaultButtons, SIGNAL(rejected()), this, SLOT(reject()));

    //main layout
    QVBoxLayout* mainLayout = new QVBoxLayout(this);
    mainLayout->addLayout(prjNameLayout);
    mainLayout->addLayout(dbEngineLayout);
    mainLayout->addWidget(m_engineOptions);
    mainLayout->addWidget(locationGroup);
    mainLayout->addStretch();
    mainLayout->addWidget(m_defaultButtons);

    connect(m_location, &QtExtChooseFile::valueChanged, this, &ProjectCreatorDialog::updateButtons);
    connect(m_prjName, &QLineEdit::textChanged, this, &ProjectCreatorDialog::updateButtons);

    updateButtons();

    // TODO: temporary (?) removal of some complex optios
    dbEngine->hide();
    m_engines->hide();
    m_engineOptions->hide();
}


ProjectCreatorDialog::~ProjectCreatorDialog()
{

}


void ProjectCreatorDialog::set(IPluginLoader* pluginLoader)
{
    m_pluginLoader = pluginLoader;
    initEngines();
}


QString ProjectCreatorDialog::getPrjName() const
{
    const QString prjName = m_prjName->text();

    return prjName;
}


QString ProjectCreatorDialog::getLocation() const
{
    const QString location = m_location->text();

    return location;
}


Database::IPlugin* ProjectCreatorDialog::getEnginePlugin() const
{
    Database::IPlugin* plugin = getSelectedPlugin();

    return plugin;
}


void ProjectCreatorDialog::initEngines()
{
    const std::vector<Database::IPlugin *>& plugins = m_pluginLoader->getDBPlugins();

    std::set<Database::IPlugin *, PluginOrder> plugins_ordered;
    plugins_ordered.insert(plugins.cbegin(), plugins.cend());

    QStackedLayout* engineOptionsLayout = new QStackedLayout(m_engineOptions);

    for(auto plugin: plugins_ordered)
    {
        const QVariant itemData = QVariant::fromValue<Database::IPlugin *>(plugin);
        m_engines->addItem(plugin->backendName(), itemData);

        QWidget* optionsWidget = new QWidget(this);
        QLayout* optionsWidgetLayout = plugin->buildDBOptions();

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


void ProjectCreatorDialog::updateButtons()
{
    const bool allData = m_prjName->text().isEmpty() == false && m_location->text().isEmpty() == false;

    m_defaultButtons->button(QDialogButtonBox::Ok)->setEnabled(allData);
}



///////////////////////////////////////////////////////////////////////////////


ProjectCreator::ProjectCreator(): m_prj()
{

}


bool ProjectCreator::create(IProjectManager* prjManager, IPluginLoader* pluginLoader)
{
    ProjectCreatorDialog prjCreatorDialog;
    prjCreatorDialog.set(pluginLoader);
    const int status = prjCreatorDialog.exec();

    if (status == QDialog::Accepted)
    {
        const QString prjName   = prjCreatorDialog.getPrjName();
        const auto*   prjPlugin = prjCreatorDialog.getEnginePlugin();
        const QString location  = prjCreatorDialog.getLocation();

        m_prj = prjManager->new_prj(prjName, prjPlugin, location);
    }

    const bool result = m_prj.isValid();

    return result;
}


const ProjectInfo& ProjectCreator::project() const
{
    return m_prj;
}
