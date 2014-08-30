
#include "mainwindow.hpp"

#include <QCloseEvent>

#include "database/database_builder.hpp"
#include "database/idatabase.hpp"

#include "centralwidget.hpp"


MainWindow::MainWindow(QWidget *p): QMainWindow(p)
{
    setCentralWidget(new CentralWidget(this));
}


MainWindow::~MainWindow()
{
}


void MainWindow::closeEvent(QCloseEvent *e)
{
    Database::Builder::instance()->closeAll();

    e->accept();
}
