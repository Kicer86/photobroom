
#include "qtext_choosefile.hpp"

#include <QCompleter>
#include <QFileSystemModel>
#include <QHBoxLayout>
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>


QtExtChooseFile::QtExtChooseFile(QWidget* p): QtExtChooseFile("", "...", std::function<QString ()>(), p)
{

}


QtExtChooseFile::QtExtChooseFile(const QString& button,
                                 const std::function<QString ()>& dialogCallback,
                                 QWidget* p):
    QtExtChooseFile("", button, dialogCallback, p)
{

}


QtExtChooseFile::QtExtChooseFile(const QString& title,
                                 const QString& button,
                                 const std::function<QString()>& dialogCallback,
                                 QWidget* p):
    QWidget(p),
    m_label(nullptr),
    m_button(nullptr),
    m_lineEdit(nullptr),
    m_dialogCallback()
{
    setup(title, button, dialogCallback);
}


QtExtChooseFile::~QtExtChooseFile()
{

}


void QtExtChooseFile::setLabel(const QString& label)
{
    m_label->setText(label);

    if (label.isEmpty())
        m_label->setVisible(false);
}


void QtExtChooseFile::setBrowseButtonText(const QString& caption)
{
    m_button->setText(caption);
}


void QtExtChooseFile::setBrowseCallback(const std::function<QString()>& callback)
{
    m_dialogCallback = callback;
}


void QtExtChooseFile::setValue(const QString& value)
{
    m_lineEdit->setText(value);
}


QString QtExtChooseFile::text() const
{
    return value();
}


QString QtExtChooseFile::value() const
{
    return m_lineEdit->text();
}


void QtExtChooseFile::buttonClicked() const
{
    const QString value = m_dialogCallback();
    if (value.isEmpty() == false)
        m_lineEdit->setText(value);
}


void QtExtChooseFile::setup(const QString& title, const QString& button, const std::function<QString ()>& dialogCallback)
{
    QHBoxLayout* l = new QHBoxLayout(this);
    m_label = new QLabel(this);
    m_lineEdit = new QLineEdit(this);
    m_button = new QPushButton(this);

    l->addWidget(m_label);
    l->addWidget(m_lineEdit);
    l->addWidget(m_button);

    setLabel(title);
    setBrowseButtonText(button);
    setBrowseCallback(dialogCallback);

    QCompleter *completer = new QCompleter(this);
    QFileSystemModel* model = new QFileSystemModel(completer);
    model->setRootPath(QDir::homePath());
    completer->setModel(model);
    m_lineEdit->setCompleter(completer);

    connect(m_button, &QPushButton::clicked, this, &QtExtChooseFile::buttonClicked);
    connect(m_lineEdit, &QLineEdit::textChanged, this, &QtExtChooseFile::valueChanged);
}
