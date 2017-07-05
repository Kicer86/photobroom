
#ifndef LOOK_TAB_HPP
#define LOOK_TAB_HPP

#include <QWidget>

#include <ui_utils/iconfig_dialog_manager.hpp>

class QSpinBox;

struct IConfiguration;
class ColorPickButton;

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

        ColorPickButton* colorEvenButton();
        ColorPickButton* colorOddButton();
        QSpinBox*    spacingSpinBox();

        LookTab& operator=(const LookTab &) = delete;

    private:
        Ui::LookTab* ui;
};



class LookTabController: public QObject, public IConfigTab
{
        Q_OBJECT

    public:
        LookTabController();
        LookTabController(const LookTabController &) = delete;
        virtual ~LookTabController();

        void set(IConfiguration *);

        LookTabController& operator=(const LookTabController &) = delete;

        int tabId() const override;
        QString tabName() const override;

        QWidget* constructTab() override;
        void applyConfiguration() override;
        void rejectConfiguration() override;

    private:
        IConfiguration* m_configuration;
        LookTab* m_tabWidget;
};

#endif // LOOK_TAB_HPP
