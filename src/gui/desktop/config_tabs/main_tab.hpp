#ifndef MAIN_TAB_HPP
#define MAIN_TAB_HPP

#include <QWidget>
#include <utils/iconfig_dialog_manager.hpp>

namespace Ui
{
    class MainTab;
}

class MainTab: public QWidget
{
        Q_OBJECT

    public:
        explicit MainTab(QWidget *parent = 0);
        MainTab(const MainTab &) = delete;
        ~MainTab();

        MainTab& operator=(const MainTab &) = delete;

    private:
        Ui::MainTab *ui;
};


class MainTabControler: public QObject, public IConfigTab
{
    public:
        MainTabControler();
        MainTabControler(const MainTabControler &) = delete;
        virtual ~MainTabControler();

        MainTabControler& operator=(const MainTabControler &) = delete;

        QString tabId() const override;
        QString tabName() const override;

        QWidget* constructTab() override;
        void applyConfiguration() override;
        void rejectConfiguration() override;

    private:
        MainTab* m_tabWidget;
};


#endif // MAIN_TAB_HPP
