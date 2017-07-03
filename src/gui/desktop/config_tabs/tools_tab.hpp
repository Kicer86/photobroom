#ifndef TOOLS_TAB_HPP
#define TOOLS_TAB_HPP

#include <QWidget>

namespace Ui {
class ToolsTab;
}

class ToolsTab : public QWidget
{
    Q_OBJECT

public:
    explicit ToolsTab(QWidget *parent = 0);
    ~ToolsTab();

private:
    Ui::ToolsTab *ui;
};

#endif // TOOLS_TAB_HPP
