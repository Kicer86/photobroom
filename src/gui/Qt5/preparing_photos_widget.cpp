
#include "preparing_photos_widget.hpp"

#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLineEdit>
#include <QPushButton>
#include <QFileDialog>
#include <QLabel>

namespace
{

    struct BrowseLine: public QWidget
    {

        BrowseLine(QWidget *parent): QWidget(parent), m_button(nullptr), m_line(nullptr)
        {
            m_button = new QPushButton(tr("Browse"), this);
            m_line = new QLineEdit(this);

            QHBoxLayout *layout = new QHBoxLayout(this);

            layout->addWidget(new QLabel(tr("Path to photos:"), this));
            layout->addWidget(m_line);
            layout->addWidget(m_button);
        }

        ~BrowseLine() {}

        QFileDialog *m_dialog;
        QPushButton *m_button;
        QLineEdit *m_line;

    };


    struct BrowseList: public QWidget
    {
        BrowseList(QWidget *parent): QWidget(parent)
        {
            QVBoxLayout *layout = new QVBoxLayout(this);

            layout->addWidget(new BrowseLine(this));
        }

        ~BrowseList()
        {
        }
    };

}


struct PreparingPhotosWidget::GuiData
{
    GuiData(QWidget *preparer): m_preparer(preparer)
    {
        QVBoxLayout *layout = new QVBoxLayout(preparer);

        layout->addWidget(new BrowseList(preparer));
        layout->addWidget(new QWidget(preparer));
    }

    ~GuiData() {}

    QWidget *m_preparer;
};


PreparingPhotosWidget::PreparingPhotosWidget(QWidget *parent): QWidget(parent), m_gui(new GuiData(this))
{

}


PreparingPhotosWidget::~PreparingPhotosWidget()
{
}
