
#ifndef LOOK_TAB_HPP
#define LOOK_TAB_HPP

#include <QWidget>

#include <utils/iconfig_dialog_manager.hpp>

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



class LookTabControler: public QObject, public IConfigTab
{
        Q_OBJECT

    public:
        LookTabControler();
        LookTabControler(const LookTabControler &) = delete;
        virtual ~LookTabControler();

        void set(IConfiguration *);

        LookTabControler& operator=(const LookTabControler &) = delete;

        int tabId() const override;
        QString tabName() const override;

        QWidget* constructTab() override;
        void applyConfiguration() override;
        void rejectConfiguration() override;

    private:
        IConfiguration* m_configuration;
        LookTab* m_tabWidget;

        QColor intToColor(u_int32_t) const;
        u_int32_t colorToInt(const QColor &) const;
};

#endif // LOOK_TAB_HPP
