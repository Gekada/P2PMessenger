//
// Created by L on 28.09.2025.
//

#ifndef P2PMESSENGER_TEMP_H
#define P2PMESSENGER_TEMP_H
class Counter : public QObject {
    Q_OBJECT

// Note. The Q_OBJECT macro starts a private section.
// To declare public members, use the 'public:' access modifier.
public:
    Counter() { m_value = 0; }

    int value() const { return m_value; }

public slots:

            void setValue(int value) {
        qDebug("It works !!!");
        if (value != m_value) {
            m_value = value;
            emit valueChanged(value);
        }
    };


    signals:

            void valueChanged(int newValue);

private:
    int m_value;
};
#endif //P2PMESSENGER_TEMP_H
