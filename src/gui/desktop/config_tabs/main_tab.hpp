#ifndef MAIN_TAB_HPP
#define MAIN_TAB_HPP

#include <QWidget>

#include <ui_utils/iconfig_dialog_manager.hpp>

class QCheckBox;
class QComboBox;

struct IConfiguration;

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

        QCheckBox* updateCheckBox();

    private:
        Ui::MainTab *ui;
};


class MainTabController: public QObject, public IConfigTab
{
        Q_OBJECT

    public:
        MainTabController();
        MainTabController(const MainTabController &) = delete;
        virtual ~MainTabController();

        void set(IConfiguration *);

        MainTabController& operator=(const MainTabController &) = delete;

        int tabId() const override;
        QString tabName() const override;

        QWidget* constructTab() override;
        void applyConfiguration() override;
        void rejectConfiguration() override;

    private:
        IConfiguration* m_configuration;
        MainTab* m_tabWidget;
};


#endif // MAIN_TAB_HPP
