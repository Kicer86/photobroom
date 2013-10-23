
#ifndef TAG_DEFINITION_HPP 
#define TAG_DEFINITION_HPP 

#include <QWidget>

class QPushButton;
class QComboBox;
class QString;
class QAbstractItemModel;

class TagDefinition: public QWidget
{
        Q_OBJECT        
    
    public:
        explicit TagDefinition(QWidget* parent = 0, Qt::WindowFlags f = 0);
        TagDefinition(const TagDefinition &) = delete;
        virtual ~TagDefinition();
        
        void setModel(QAbstractItemModel *) const;
        
        TagDefinition& operator=(const TagDefinition &) = delete;
        
    private:
        QComboBox* m_comboBox;
        QPushButton* m_button;
        
    private slots:
        void buttonPressed() const;
        void comboChanged(const QString &) const;
        
    signals:
        void tagChoosen(const QString &) const;
};

#endif
