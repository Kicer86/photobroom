
#include "preparing_photos_widget_private.hpp"

#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLineEdit>
#include <QPushButton>
#include <QFileDialog>
#include <QLabel>

#include "../photos_editor_widget.hpp"

BrowseLine::BrowseLine(QWidget *parent):
    QWidget(parent),
    m_dialog(nullptr),
    m_button(nullptr),
    m_line(nullptr),
    m_addButton(nullptr)
{
    m_button = new QPushButton(tr("Browse"), this);
    m_line = new QLineEdit(this);
    m_addButton = new QPushButton(tr("add"));

    QHBoxLayout *layout = new QHBoxLayout(this);

    layout->addWidget(new QLabel(tr("Path to photos:"), this));
    layout->addWidget(m_line);
    layout->addWidget(m_button);
    layout->addWidget(m_addButton);

    connect(m_button, SIGNAL(clicked()), this, SLOT(browseButtonClicked()));
    connect(m_addButton, SIGNAL(clicked()), this, SLOT(addButtonClicked()));
}


BrowseLine::~BrowseLine()
{

}


void BrowseLine::browseButtonClicked()
{

}


void BrowseLine::addButtonClicked()
{
    const QString path = m_line->text();

    emit addPath(path);
}


BrowseList::BrowseList(QWidget *parent): QWidget(parent)
{
    QVBoxLayout *layout = new QVBoxLayout(this);

    BrowseLine *line = new BrowseLine(this);
    layout->addWidget(line);
    connect(line, SIGNAL(addPath(QString)), this, SLOT(addPathToAnalyze(QString)));
}


BrowseList::~BrowseList()
{
}


void BrowseList::addPathToAnalyze(QString path)
{
    emit addPath(path);
}
