
#include "look_tab.hpp"

#include <core/iconfiguration.hpp>

#include "config_keys.hpp"
#include "ui_look_tab.h"
#include "utils/config_tools.hpp"


LookTab::LookTab(QWidget* parent) :
    QWidget(parent),
    ui(new Ui::LookTab)
{
    ui->setupUi(this);
}


LookTab::~LookTab()
{
    delete ui;
}


ColorPickButton* LookTab::colorEvenButton()
{
    return ui->colorEvenButton;
}


ColorPickButton* LookTab::colorOddButton()
{
    return ui->colorOddButton;
}


QSpinBox* LookTab::spacingSpinBox()
{
    return ui->spacingSpinBox;
}



LookTabController::LookTabController(): m_configuration(nullptr), m_tabWidget(nullptr)
{
}


LookTabController::~LookTabController()
{
}


void LookTabController::set(IConfiguration* configuration)
{
    m_configuration = configuration;
}


int LookTabController::tabId() const
{
    return 10;
}


QString LookTabController::tabName() const
{
    return tr("Look");
}


QWidget* LookTabController::constructTab()
{
    m_tabWidget = new LookTab;

    const uint32_t even_color = m_configuration->getEntry(ViewConfigKeys::bkg_color_even).toUInt();
    const QColor even_qcolor = ConfigTools::intToColor(even_color);

    m_tabWidget->colorEvenButton()->setColor(even_qcolor);

    const uint32_t odd_color = m_configuration->getEntry(ViewConfigKeys::bkg_color_odd).toUInt();
    const QColor odd_qcolor = ConfigTools::intToColor(odd_color);

    m_tabWidget->colorOddButton()->setColor(odd_qcolor);

    const int items_spacing = m_configuration->getEntry(ViewConfigKeys::itemsSpacing).toInt();

    m_tabWidget->spacingSpinBox()->setValue(items_spacing);

    return m_tabWidget;
}


void LookTabController::applyConfiguration()
{
    const QColor& even_qcolor = m_tabWidget->colorEvenButton()->getColor();
    const uint32_t even_color = ConfigTools::colorToInt(even_qcolor);
    m_configuration->setEntry(ViewConfigKeys::bkg_color_even, even_color);

    const QColor& odd_qcolor = m_tabWidget->colorOddButton()->getColor();
    const uint32_t odd_color = ConfigTools::colorToInt(odd_qcolor);
    m_configuration->setEntry(ViewConfigKeys::bkg_color_odd, odd_color);

    const int items_spacing = m_tabWidget->spacingSpinBox()->value();
    m_configuration->setEntry(ViewConfigKeys::itemsSpacing, items_spacing);
}


void LookTabController::rejectConfiguration()
{
}
