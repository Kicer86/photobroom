
#ifndef PROPERTY_AWAITER_TEST_OBJECT_HPP_INCLUDED
#define PROPERTY_AWAITER_TEST_OBJECT_HPP_INCLUDED

#include <QObject>

class TestObject : public QObject
{
    Q_OBJECT
    Q_PROPERTY(int first READ first WRITE setFirst NOTIFY firstChanged)
    Q_PROPERTY(int second READ second WRITE setSecond NOTIFY secondChanged)
    Q_PROPERTY(int third READ third WRITE setThird NOTIFY thirdChanged)

public:
    int first() const { return m_first; }
    int second() const { return m_second; }
    int third() const { return m_third; }

    void setFirst(int value)
    {
        if (m_first == value)
            return;

        m_first = value;
        emit firstChanged();
    }

    void setSecond(int value)
    {
        if (m_second == value)
            return;

        m_second = value;
        emit secondChanged();
    }

    void setThird(int value)
    {
        if (m_third == value)
            return;

        m_third = value;
        emit thirdChanged();
    }

signals:
    void firstChanged();
    void secondChanged();
    void thirdChanged();

private:
    int m_first = 0;
    int m_second = 0;
    int m_third = 0;
};

#endif
