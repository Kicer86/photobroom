
#ifndef LOOK_TAB_HPP
#define LOOK_TAB_HPP

#include <QWidget>

namespace Ui
{
    class LookTab;
}

class LookTab : public QWidget
{
        Q_OBJECT

    public:
        explicit LookTab(QWidget* parent = 0);
        LookTab(const LookTab &) = delete;
        ~LookTab();

        LookTab& operator=(const LookTab &) = delete;

    private:
        Ui::LookTab* ui;
};

#endif // LOOK_TAB_HPP
