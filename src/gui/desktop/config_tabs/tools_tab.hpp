
#ifndef TOOLS_TAB_HPP
#define TOOLS_TAB_HPP

#include <QWidget>

#include <ui_utils/iconfig_dialog_manager.hpp>


namespace Ui
{
    class ToolsTab;
}

struct IConfiguration;

class ToolsTab : public QWidget
{
        Q_OBJECT

    public:
        explicit ToolsTab(QWidget *parent = 0);
        ~ToolsTab();

        ToolsTab(const ToolsTab &) = delete;
        ToolsTab& operator=(const ToolsTab &) = delete;

    private:
        Ui::ToolsTab *ui;
};


class ToolsTabControler: public QObject, public IConfigTab
{
        Q_OBJECT

    public:
        ToolsTabControler();
        ToolsTabControler(const ToolsTabControler &) = delete;
        virtual ~ToolsTabControler();

        void set(IConfiguration *);

        ToolsTabControler& operator=(const ToolsTabControler &) = delete;

        int tabId() const override;
        QString tabName() const override;

        QWidget* constructTab() override;
        void applyConfiguration() override;
        void rejectConfiguration() override;

    private:
        IConfiguration* m_configuration;
        ToolsTab* m_tabWidget;
};

#endif // TOOLS_TAB_HPP
