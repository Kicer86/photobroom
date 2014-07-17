
#include "main_view_widget.hpp"

#include <QVBoxLayout>
#include <QTreeView>
#include <QPushButton>

#include <core/base_tags.hpp>
#include <database/databasebuilder.hpp>

#include "model_view/images_tree_view.hpp"
#include "main_view_data_model.hpp"
#include "components/filters_editor/filters_widget.hpp"

MainViewWidget::MainViewWidget(QWidget *p): QWidget(p), m_imagesModel(nullptr), m_imagesView(nullptr)
{
    DBDataModel* dbModel = new MainViewDataModel(this);
    dbModel->setDatabase(Database::Builder::instance()->get());

    m_imagesModel = dbModel;

    m_imagesView = new ImagesTreeView(this);
    m_imagesView->setModel(m_imagesModel);

    FiltersWidget* filters = new FiltersWidget;
    filters->setBasicFilters( {"Date and time", "People"} );
    connect( filters, SIGNAL(basicFilterChoosen(int)), this, SLOT(basicFiltersUpdated(int)) );

    QVBoxLayout* main_layout = new QVBoxLayout(this);
    main_layout->addWidget(filters);
    main_layout->addWidget(m_imagesView);

    QPushButton* button = new QPushButton("Refresh", this);
    connect(button, SIGNAL(pressed()), this, SLOT(refresh()));
    main_layout->addWidget(button);
}


MainViewWidget::~MainViewWidget()
{

}


void MainViewWidget::refresh()
{
    m_imagesView->setModel(nullptr);
    m_imagesView->setModel(m_imagesModel);
}


void MainViewWidget::basicFiltersUpdated(int index)
{
    if (index == 0)
    {
        Hierarchy hierarchy;
        hierarchy.levels = { { BaseTags::get(BaseTagsList::Date), Hierarchy::Level::Order::ascending }  };

        m_imagesModel->setHierarchy(hierarchy);
    }
    else
    {
        Hierarchy hierarchy;
        hierarchy.levels = { { BaseTags::get(BaseTagsList::People), Hierarchy::Level::Order::ascending }  };

        m_imagesModel->setHierarchy(hierarchy);
    }

}
