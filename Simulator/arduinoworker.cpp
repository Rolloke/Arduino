#include "arduinoworker.h"

ArduinoWorker::ArduinoWorker(QObject *parent) : QThread(parent), mRun(true), mChanged(false), mID(0)
{
}

ArduinoWorker::~ArduinoWorker()
{
    stop();
    wait();
}

void ArduinoWorker::stop()
{
    mRun= false;
}

void ArduinoWorker::run()
{
    mID = currentThreadId();
    while (mRun.load())
    {
        mLoop();
        QThread::msleep(1);
        Q_EMIT updateLiquidCrystal();
    }
    exit();
}

void ArduinoWorker::setLoopFunction(LoopFunction aLoop)
{
    mLoop = aLoop;
}

void ArduinoWorker::setChanged()
{
    mChanged.store(true);
}

bool ArduinoWorker::runningInCurrentThread()
{
    return currentThreadId() == mID;
}
