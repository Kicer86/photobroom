
#include "mainwindow.hpp"

#include <QCloseEvent>
#include <QMenuBar>

#include "database/database_builder.hpp"
#include "database/idatabase.hpp"

#include "centralwidget.hpp"


MainWindow::MainWindow(QWidget *p): QMainWindow(p)
{
    setCentralWidget(new CentralWidget(this));

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


void MainWindow::closeEvent(QCloseEvent *e)
{
    Database::Builder::instance()->closeAll();

    e->accept();
}


void MainWindow::newProject()
{

}


void MainWindow::openProject()
{

}

