
#include "preparing_photos_widget.hpp"

#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLineEdit>
#include <QPushButton>
#include <QFileDialog>
#include <QLabel>

#include "photos_editor_widget.hpp"

namespace
{

    struct BrowseLine: public QWidget
    {

        BrowseLine(QWidget *parent):
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

        ~BrowseLine() {}

        signals:
            void addPath(QString);

        private slots:
            QFileDialog *m_dialog;
            QPushButton *m_button;
            QLineEdit   *m_line;
            QPushButton *m_addButton;

            void browseButtonClicked()
            {

            }

            void addButtonClicked()
            {
                const QString path = m_line->text();

                emit addPath(path);
            }
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

        BrowseLine *browse = new BrowseLine(preparer);
        PhotosEditorWidget *editor = new PhotosEditorWidget(preparer);

        browse->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Minimum);

        layout->addWidget(browse);
        layout->addWidget(editor);
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
