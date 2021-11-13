
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
        QtExtChooseFile* magickPath() const;
        QtExtChooseFile* ffmpegPath() const;
        QtExtChooseFile* ffprobePath() const;
        QtExtChooseFile* exiftoolPath() const;

    private:
        Ui::ToolsTab *ui;
};


class ToolsTabController: public QObject, public AConfigTab<ToolsTab>
{
        Q_OBJECT

    public:
        ToolsTabController();
        ToolsTabController(const ToolsTabController &) = delete;
        virtual ~ToolsTabController();

        ToolsTabController& operator=(const ToolsTabController &) = delete;

        int tabId() const override;
        QString tabName() const override;

        QWidget* constructTab() override;
        void applyConfiguration() override;
        void rejectConfiguration() override;
};

#endif // TOOLS_TAB_HPP
