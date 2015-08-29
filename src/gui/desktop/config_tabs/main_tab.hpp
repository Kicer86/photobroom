#ifndef MAIN_TAB_HPP
#define MAIN_TAB_HPP

#include <QWidget>

namespace Ui {
class MainTab;
}

class MainTab : public QWidget
{
    Q_OBJECT

public:
    explicit MainTab(QWidget *parent = 0);
    ~MainTab();

private:
    Ui::MainTab *ui;
};

#endif // MAIN_TAB_HPP
