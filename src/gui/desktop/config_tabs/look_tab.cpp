
#include "look_tab.hpp"

#include <configuration/iconfiguration.hpp>

#include "config_keys.hpp"
#include "ui_look_tab.h"


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


QPushButton* LookTab::color1Button()
{
    return ui->color1Button;
}


QPushButton* LookTab::color2Button()
{
    return ui->color2Button;
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
    const QColor even_qcolor = intToColor(even_color);

    QPixmap color1pxmap(16, 16);
    color1pxmap.fill(even_qcolor);

    m_tabWidget->color1Button()->setIcon(color1pxmap);

    return m_tabWidget;
}


void LookTabControler::applyConfiguration()
{
    
}


void LookTabControler::rejectConfiguration()
{
}


QColor LookTabControler::intToColor(u_int32_t c) const
{
    const int r = (c >> 24) & 0xff;
    const int g = (c >> 16) & 0xff;
    const int b = (c >> 8)  & 0xff;
    const int a = c & 0xff;

    return QColor(r, g, b, a);
}


u_int32_t LookTabControler::colorToInt(const QColor& c) const
{
    u_int32_t result = 0;

    result |= (c.red() & 0xffu) << 24;
    result |= (c.green() & 0xff) << 16;
    result |= (c.blue() & 0xff) << 8;
    result |= (c.alpha() & 0xff);

    return result;
}
