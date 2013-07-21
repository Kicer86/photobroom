
#include "preparing_photos_widget.hpp"

#include <QVBoxLayout>


struct BrowseList: public QWidget
{
    BrowseList(QWidget *parent): QWidget(parent)
    {
    }

    ~BrowseList()
    {
    }
};


struct PreparingPhotosWidget::GuiData
{
    GuiData(QWidget *preparer): m_preparer(preparer)
    {
        QVBoxLayout *layout = new QVBoxLayout(preparer);

        layout->addWidget(new BrowseList(preparer));
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
