#ifndef CONFIGURATION_DIALOG_HPP
#define CONFIGURATION_DIALOG_HPP

#include <QDialog>

class QAbstractButton;

namespace Ui
{
    class ConfigurationDialog;
}

class ConfigurationDialog : public QDialog
{
        Q_OBJECT

    public:
        explicit ConfigurationDialog(QWidget *parent = 0);
        ConfigurationDialog(const ConfigurationDialog &) = delete;

        virtual ~ConfigurationDialog();

        ConfigurationDialog& operator=(const ConfigurationDialog &) = delete;

        void addTab(const QString &, QWidget *);

    private:
        Ui::ConfigurationDialog *ui;

    private slots:
        void on_buttonBox_accepted();
        void on_buttonBox_rejected();
        void on_buttonBox_clicked(QAbstractButton *button);

    signals:
        void saveData();
};

#endif // CONFIGURATION_DIALOG_HPP
