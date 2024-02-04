#ifndef ARDUINOWORKER_H
#define ARDUINOWORKER_H

#include <QThread>
#include <boost/function.hpp>
#include <boost/bind.hpp>
#include <boost/atomic.hpp>


using LoopFunction = boost::function<void ()>;

class ArduinoWorker : public QThread
{
    Q_OBJECT

public:
    ArduinoWorker(QObject *parent = Q_NULLPTR);
    virtual ~ArduinoWorker();

    void setLoopFunction(LoopFunction aLoop);
    void stop();
    void setChanged();
    bool runningInCurrentThread();

public Q_SLOTS:

Q_SIGNALS:
    void updateLiquidCrystal();
    void repaint();

private:
    void run() override;

    LoopFunction mLoop;
    boost::atomic_bool mRun;
    boost::atomic_bool mChanged;
    Qt::HANDLE mID;

};

#endif // ARDUINOWORKER_H
