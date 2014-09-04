
#include "mainwindow.hpp"

#include <QCloseEvent>
#include <QMenuBar>
#include <QFileDialog>

#include <database/database_builder.hpp>
#include <database/idatabase.hpp>
#include <project_utils/iproject_manager.hpp>
#include <project_utils/iproject.hpp>

#include "centralwidget.hpp"
#include "components/project_creator/project_creator.hpp"


MainWindow::MainWindow(QWidget *p): QMainWindow(p), m_prjManager(nullptr), m_currentPrj(nullptr), m_centralWidget(nullptr)
{
    m_centralWidget = new CentralWidget(this);
    setCentralWidget(m_centralWidget);

    QMenu* projectMenu = new QMenu( tr("Project"), this);
    QAction* newPrjAction  = projectMenu->addAction( tr("New project") );
    QAction* openPrjAction = projectMenu->addAction( tr("Open project") );

    QMenuBar* mainMenuBar = new QMenuBar(this);
    mainMenuBar->addMenu(projectMenu);

    setMenuBar(mainMenuBar);

    connect( newPrjAction,  SIGNAL(triggered(bool)), this, SLOT(newProject()) );
    connect( openPrjAction, SIGNAL(triggered(bool)), this, SLOT(openProject()) );
}


MainWindow::~MainWindow()
{
}


void MainWindow::set(IProjectManager* prjManager)
{
    m_prjManager = prjManager;
}


void MainWindow::closeEvent(QCloseEvent *e)
{
    //m_currentPrj->close();

    e->accept();
}


void MainWindow::newProject()
{
    ProjectCreator prjCreator;
    prjCreator.exec();
}


void MainWindow::openProject()
{
    const QString prjFile = QFileDialog::getOpenFileName(this, tr("Open File"),
                                                         "",
                                                         tr("broom projects (*.bpj)"));

    std::shared_ptr<IProject> prj = m_prjManager->open(prjFile);
    m_currentPrj = prj;
    Database::IDatabase* db = m_currentPrj->getDatabase();

    m_centralWidget->setDatabase(db);
}

