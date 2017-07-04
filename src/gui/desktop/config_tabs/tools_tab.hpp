
#ifndef TOOLS_TAB_HPP
#define TOOLS_TAB_HPP

#include <QWidget>

#include "a_config_tab.hpp"

class QtExtChooseFile;

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

        QtExtChooseFile* aisPath() const;
        QtExtChooseFile* convertPath() const;

    private:
        Ui::ToolsTab *ui;
};


class ToolsTabControler: public QObject, public AConfigTab<ToolsTab>
{
        Q_OBJECT

    public:
        ToolsTabControler();
        ToolsTabControler(const ToolsTabControler &) = delete;
        virtual ~ToolsTabControler();

        ToolsTabControler& operator=(const ToolsTabControler &) = delete;

        int tabId() const override;
        QString tabName() const override;

        QWidget* constructTab() override;
        void applyConfiguration() override;
        void rejectConfiguration() override;
};

#endif // TOOLS_TAB_HPP
