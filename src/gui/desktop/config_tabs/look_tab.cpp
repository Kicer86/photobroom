
#include "look_tab.hpp"

#include <configuration/iconfiguration.hpp>

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



LookTabControler::LookTabControler(): m_configuration(nullptr), m_tabWidget(nullptr)
{
}


LookTabControler::~LookTabControler()
{
}


void LookTabControler::set(IConfiguration* configuration)
{
    m_configuration = configuration;
}


int LookTabControler::tabId() const
{
    return 10;
}


QString LookTabControler::tabName() const
{
    return tr("Look");
}


QWidget* LookTabControler::constructTab()
{
    m_tabWidget = new LookTab;

    const u_int32_t even_color = m_configuration->getEntry(ViewConfigKeys::bkg_color_even).toUInt();
    const QColor even_qcolor = ConfigTools::intToColor(even_color);

    m_tabWidget->colorEvenButton()->setColor(even_qcolor);

    const u_int32_t odd_color = m_configuration->getEntry(ViewConfigKeys::bkg_color_odd).toUInt();
    const QColor odd_qcolor = ConfigTools::intToColor(odd_color);

    m_tabWidget->colorOddButton()->setColor(odd_qcolor);

    return m_tabWidget;
}


void LookTabControler::applyConfiguration()
{
    const QColor& even_qcolor = m_tabWidget->colorEvenButton()->getColor();
    const u_int32_t even_color = ConfigTools::colorToInt(even_qcolor);
    m_configuration->setEntry(ViewConfigKeys::bkg_color_even, even_color);

    const QColor& odd_qcolor = m_tabWidget->colorOddButton()->getColor();
    const u_int32_t odd_color = ConfigTools::colorToInt(odd_qcolor);
    m_configuration->setEntry(ViewConfigKeys::bkg_color_odd, odd_color);
}


void LookTabControler::rejectConfiguration()
{
}
