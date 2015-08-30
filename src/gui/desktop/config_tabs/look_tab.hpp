
#ifndef LOOK_TAB_HPP
#define LOOK_TAB_HPP

#include <QWidget>

#include <utils/iconfig_dialog_manager.hpp>

class QCheckBox;
class QComboBox;

struct IConfiguration;

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



class LookTabControler: public QObject, public IConfigTab
{
    public:
        LookTabControler();
        LookTabControler(const LookTabControler &) = delete;
        virtual ~LookTabControler();

        void set(IConfiguration *);

        LookTabControler& operator=(const LookTabControler &) = delete;

        QString tabId() const override;
        QString tabName() const override;

        QWidget* constructTab() override;
        void applyConfiguration() override;
        void rejectConfiguration() override;

    private:
        IConfiguration* m_configuration;
        LookTab* m_tabWidget;
};

#endif // LOOK_TAB_HPP
