#include "serialinterface.h"

SerialInterface::SerialInterface(QObject *parent) : QObject(parent)
{

}

void SerialInterface::pushText(const QString& aText)
{
    mBuffer += aText;
}

void SerialInterface::begin(int)
{

}

void SerialInterface::print(int aValue)
{
    Q_EMIT sendText(QString::number(aValue));
}

void SerialInterface::println(int aValue)
{
    Q_EMIT sendText(QString::number(aValue) + "\n");
}

void SerialInterface::print(const char *aText)
{
    Q_EMIT sendText(QString(aText));
}

void SerialInterface::println(const char *aText)
{
    Q_EMIT sendText(QString(aText) + "\n");
}

int SerialInterface::available()
{
    return mBuffer.size();
}

char SerialInterface::read()
{
    char fChar = 0xff;
    if (available())
    {
        fChar = mBuffer.at(0).toLatin1();
        mBuffer.remove(0, 1);
    }
    return fChar;
}
