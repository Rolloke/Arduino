#ifndef SERIALINTERFACE_H
#define SERIALINTERFACE_H

#include <QObject>
#include <QString>

class SerialInterface : public QObject
{
    Q_OBJECT
public:
    explicit SerialInterface(QObject *parent = 0);

    void pushText(const QString& aText);

    void begin(int );

    void println(int aValue);
    void print(int aValue);

    void println(const char* aText);
    void print(const char* aText);

    int  available();
    char read();

Q_SIGNALS:
    void sendText(const QString& aText);

public Q_SLOTS:

private:
    QString mBuffer;
};

#endif // SERIALINTERFACE_H
