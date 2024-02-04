#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <serialinterface.h>

#include <Arduino.h>
#include <LiquidCrystal.h>

#include <iostream>

#include <QVector>
#include <QString>
#include <QSettings>
#include <QRegExp>
#include <QTimer>
#include <QTime>
#include <QThread>
#include <QStandardItemModel>
#include <QAbstractItemModel>
#include <QFileDialog>
#include <QFileInfo>
#include <QMutexLocker>

namespace config
{
constexpr char sListArduinos[] = "Arduinos";
constexpr char sArduinoName[] = "ArduinoName";
constexpr char sArduinoPins[] = "ArduinoPins";

}

#define STORE_PTR(SETTING, ITEM, FUNC) SETTING.setValue(getSettingsName(#ITEM), ITEM->FUNC())
#define STORE_NP(SETTING, ITEM, FUNC)  SETTING.setValue(getSettingsName(#ITEM), ITEM.FUNC())
#define STORE_STR(SETTING, ITEM)       SETTING.setValue(getSettingsName(#ITEM), ITEM)

#define LOAD_PTR(SETTING, ITEM, SET, GET, CONVERT) ITEM->SET(SETTING.value(getSettingsName(#ITEM), ITEM->GET()).CONVERT())
#define LOAD_NP(SETTING, ITEM, SET, GET, CONVERT)  ITEM.SET(SETTING.value(getSettingsName(#ITEM), ITEM.GET()).CONVERT())
#define LOAD_STR(SETTING, ITEM, CONVERT)           ITEM=SETTING.value(getSettingsName(#ITEM), ITEM).CONVERT()


QString getSettingsName(const QString& aItemName);

enum eTable
{
    ePinIndex, ePinNo, ePinType, eValue, eLast, eRange=eLast
};

SerialInterface Serial;

MainWindow* MainWindow::gmThis = NULL;
MainWindow::tPinAccess MainWindow::gmStaticPinAccess;

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
    , mFirstAnalogPin(0)
    , mLiquidCrystal(0)
    , mLogicAnalyser(this)
    , mUseWorkerThread(true)
{
    ui->setupUi(this);

    connect(&mLogicAnalyser, SIGNAL(record(bool)), this, SLOT(on_Record(bool)));
    connect(&mLogicAnalyser, SIGNAL(isshown(bool)), this, SLOT(on_ShowLogicAnalyser(bool)));
    mLogicAnalyser.millis = &MainWindow::elapsed;

    QSettings fSettings("config.ini", QSettings::NativeFormat);

    mArduinoTime = new QTime;
    m_edits.append(ui->EditEsc);
    m_edits.append(ui->EditUp);
    m_edits.append(ui->EditF1);
    m_edits.append(ui->EditRight);
    m_edits.append(ui->EditEnter);
    m_edits.append(ui->EditLeft);
    m_edits.append(ui->EditClear);
    m_edits.append(ui->EditDown);
    m_edits.append(ui->EditF2);

    m_buttons.append(ui->BtnEscape);
    m_buttons.append(ui->BtnUp);
    m_buttons.append(ui->BtnF1);
    m_buttons.append(ui->BtnRight);
    m_buttons.append(ui->BtnEnter);
    m_buttons.append(ui->BtnLeft);
    m_buttons.append(ui->BtnClear);
    m_buttons.append(ui->BtnDown);
    m_buttons.append(ui->BtnF2);

    for (auto& button : m_buttons)
    {
        connect(button, SIGNAL(pressed()), this, SLOT(on_btnPressed()));
        connect(button, SIGNAL(released()), this, SLOT(on_btnReleased()));
    }

    ui->comboArduino->addItem("AtMega328");
    ui->comboArduino->addItem("AtMega2560");
    mArduinoList.push_back({ "D0 Rx", "D1 Tx", "D2 Int0", "D3~ Int1", "D4", "D5~", "D6~", "D7", "D8", "D9", "D10~ SS", "D11~ MOSI", "D12 MISO", "D13 SCK", "A0", "A1", "A2", "A3", "A4 SDA", "A5 SCL"});
    mArduinoList.push_back({ "D0 Rx0", "D1 Tx0", "D2~ Int0", "D3~ Int1", "D4~", "D5~", "D6~", "D7~", "D8~", "D9~",
                                    "D10~", "D11~", "D12~", "D13~", "D14 Tx3", "D15 Rx3", "D16 Tx2", "D17 Rx2", "D18 Tx1 Int2", "D19 Rx1 Int3",
                                    "D20 SDA Int4", "D21 SCL Int5", "D22", "D23", "D24", "D25", "D26", "D27", "D28", "D29",
                                    "D30", "D31", "D32", "D33", "D34", "D35", "D36", "D37", "D38", "D39",
                                    "D40", "D41", "D42", "D43", "D44", "D45", "D46", "D47", "D48", "D49",
                                    "D50", "D51", "D52", "D53",
                                    "A0", "A1", "A2", "A3", "A4", "A5", "A6", "A7", "A8", "A9", "A10", "A11", "A12", "A13", "A14", "A15"});

    {
        int fItemCount = fSettings.beginReadArray(config::sListArduinos);
        for (int fItem = 0; fItem < fItemCount; ++fItem)
        {
            fSettings.setArrayIndex(fItem);
            QString name = fSettings.value(config::sArduinoName).toString();
            if (ui->comboArduino->findText(name) == -1)
            {
                ui->comboArduino->addItem(name);
                QString pinlist =  fSettings.value(config::sArduinoPins).toString();
                mArduinoList.push_back(pinlist.split(","));
            }
        }
        fSettings.endArray();
    }
    ui->comboArduino->setCurrentIndex(0);
    QString fArduinoName = ui->comboArduino->currentText();
    ui->comboArduino->setCurrentIndex(-1);

    LOAD_PTR(fSettings, ui->BtnEscape, setText, text, toString);
    LOAD_PTR(fSettings, ui->BtnUp, setText, text, toString);
    LOAD_PTR(fSettings, ui->BtnRight, setText, text, toString);
    LOAD_PTR(fSettings, ui->BtnEnter, setText, text, toString);
    LOAD_PTR(fSettings, ui->BtnLeft, setText, text, toString);
    LOAD_PTR(fSettings, ui->BtnClear, setText, text, toString);
    LOAD_PTR(fSettings, ui->BtnDown, setText, text, toString);
    LOAD_PTR(fSettings, ui->BtnF1, setText, text, toString);
    LOAD_PTR(fSettings, ui->BtnF2, setText, text, toString);

    LOAD_STR(fSettings, fArduinoName, toString);
    LOAD_STR(fSettings, mUseWorkerThread, toBool);


    QString tool_tip = ui->EditEsc->toolTip();
    for (int i=0; i<m_edits.size(); ++i)
    {
        connect(m_edits[i], SIGNAL(textChanged(QString)), this, SLOT(on_textChanged(QString)));
        m_edits[i]->setToolTip(tr(tool_tip.toStdString().c_str()).arg(m_buttons[i]->text()));
    }

    mListModel = new QStandardItemModel(0, eLast, this);
    mListModel->setHeaderData(ePinNo   , Qt::Horizontal, tr("Pin"));
    mListModel->setHeaderData(ePinIndex, Qt::Horizontal, tr("No."));
    mListModel->setHeaderData(ePinType , Qt::Horizontal, tr("Type"));
    mListModel->setHeaderData(eValue   , Qt::Horizontal, tr("Value"));
    ui->tableView->setModel(mListModel);

    connect(ui->comboArduino, SIGNAL(currentIndexChanged(int)), this, SLOT(on_comboArduinoCurrentIndexChanged(int)));
    int index = ui->comboArduino->findText(fArduinoName);
    ui->comboArduino->setCurrentIndex(index);

    LOAD_PTR(fSettings, ui->EditEsc, setText, text, toString);
    LOAD_PTR(fSettings, ui->EditUp, setText, text, toString);
    LOAD_PTR(fSettings, ui->EditF1, setText, text, toString);
    LOAD_PTR(fSettings, ui->EditRight, setText, text, toString);
    LOAD_PTR(fSettings, ui->EditEnter, setText, text, toString);
    LOAD_PTR(fSettings, ui->EditLeft, setText, text, toString);
    LOAD_PTR(fSettings, ui->EditClear, setText, text, toString);
    LOAD_PTR(fSettings, ui->EditDown, setText, text, toString);
    LOAD_PTR(fSettings, ui->EditF2, setText, text, toString);

    ui->tableView->setColumnWidth(ePinIndex,  30);
    ui->tableView->setColumnWidth(ePinNo   ,  85);
    ui->tableView->setColumnWidth(ePinType , 100);
    ui->tableView->setColumnWidth(eValue   ,  60);

    QObject::connect(&Serial, SIGNAL(sendText(QString)), this, SLOT(printToSeriaDisplay(QString)));

    initialize(ui);
    gmThis = this;
}

MainWindow::~MainWindow()
{
    stopArduino();

    QSettings fSettings("config.ini", QSettings::NativeFormat);
    STORE_PTR(fSettings, ui->EditEsc, text);
    STORE_PTR(fSettings, ui->EditUp, text);
    STORE_PTR(fSettings, ui->EditF1, text);
    STORE_PTR(fSettings, ui->EditRight, text);
    STORE_PTR(fSettings, ui->EditEnter, text);
    STORE_PTR(fSettings, ui->EditLeft, text);
    STORE_PTR(fSettings, ui->EditClear, text);
    STORE_PTR(fSettings, ui->EditDown, text);
    STORE_PTR(fSettings, ui->EditF2, text);

    STORE_PTR(fSettings, ui->BtnEscape, text);
    STORE_PTR(fSettings, ui->BtnUp, text);
    STORE_PTR(fSettings, ui->BtnRight, text);
    STORE_PTR(fSettings, ui->BtnEnter, text);
    STORE_PTR(fSettings, ui->BtnLeft, text);
    STORE_PTR(fSettings, ui->BtnClear, text);
    STORE_PTR(fSettings, ui->BtnDown, text);
    STORE_PTR(fSettings, ui->BtnF1, text);
    STORE_PTR(fSettings, ui->BtnF2, text);
    QString fArduinoName = ui->comboArduino->currentText();
    STORE_STR(fSettings, fArduinoName);
    STORE_STR(fSettings, mUseWorkerThread);

    fSettings.beginWriteArray(config::sListArduinos);
    {
        int fIndex = 0;
        for (int i=0; i < mArduinoList.size(); ++i)
        {
            fSettings.setArrayIndex(fIndex++);
            fSettings.setValue(config::sArduinoName, ui->comboArduino->itemText(i));
            fSettings.setValue(config::sArduinoPins, mArduinoList[i].join(","));
        }
        fSettings.endArray();
    }

    delete mArduinoTime;
    delete ui;
}

void MainWindow::stopArduino()
{
    if (mSetPinsTimer)
    {
        mSetPinsTimer->stop();
        disconnect(mSetPinsTimer, SIGNAL(timeout()), this, SLOT(setPins()));
        delete mSetPinsTimer;
        mSetPinsTimer = nullptr;
    }
    if (mLoopTimer)
    {
        mLoopTimer->stop();
        connect(mLoopTimer, SIGNAL(timeout()), this, SLOT(on_Loop()));
        delete mLoopTimer;
        mLoopTimer = nullptr;
    }
    if (mOneSecondTimer)
    {
        mOneSecondTimer->stop();
        connect(mOneSecondTimer, SIGNAL(timeout()), this, SLOT(on_OneSecond()));
        delete mOneSecondTimer;
        mOneSecondTimer = nullptr;
    }
    if (mWorker)
    {
        mWorker->stop();
        mWorker->wait();
    }
    mPinsSet = false;
}

void MainWindow::on_comboArduinoCurrentIndexChanged(int index)
{
    if (index >= 0 && index < mArduinoList.size())
    {
        auto& fPins = mArduinoList[index];
        mFirstAnalogPin = fPins.indexOf("A0");
        mListModel->removeRows(0, mListModel->rowCount());
        int fRow = 0;
        for (const auto &fPin : fPins)
        {
            mListModel->insertRows(fRow, 1, QModelIndex());
            mListModel->setData(mListModel->index(fRow, ePinIndex, QModelIndex()), QString::number(fRow));
            mListModel->setData(mListModel->index(fRow, ePinNo, QModelIndex()), fPin);
            mListModel->setData(mListModel->index(fRow, ePinType, QModelIndex()), "");
            mListModel->setData(mListModel->index(fRow, eValue, QModelIndex()), "0");
            mListModel->setData(mListModel->index(fRow, eRange, QModelIndex()), "1");
            ++fRow;
        }
        QTimer::singleShot(5, this, SLOT(on_Setup()));
    }
}

void MainWindow::sendSerialText(const QString& aText)
{
    Serial.pushText(aText + "\n");
    loop();
}

void MainWindow::printToSeriaDisplay(const QString &aText)
{
    ui->textReceivedFromSerialDevice->insertPlainText(aText);
}

int  MainWindow::elapsed()
{
    if (gmThis)
    {
        return gmThis->mArduinoTime->elapsed();
    }
    return 0;
}

void MainWindow::setPins()
{
    if (!mPinsSet)
    {
        QMutexLocker fLock(&mPinMutex);
        for (const auto& fPA : gmStaticPinAccess)
        {
            setPin(fPA);
        }
    }
    mPinsSet = true;
}


void MainWindow::on_Setup()
{
    stopArduino();
    setPins();
    mArduinoTime->restart();
    setup();

    if (mUseWorkerThread)
    {
        mWorker.reset(new ArduinoWorker(this));
    }

    if (mWorker)
    {
        connect(mWorker.get(), SIGNAL(updateLiquidCrystal()), this, SLOT(updateLiquidCrystal()));
        mWorker->setLoopFunction(boost::bind(&MainWindow::on_Loop, this));
        mWorker->start();
        mSetPinsTimer = new QTimer(this);
        connect(mSetPinsTimer, SIGNAL(timeout()), this, SLOT(setPins()));
        mSetPinsTimer->start(1);
    }
    else
    {
        mLoopTimer = new QTimer(this);
        connect(mLoopTimer, SIGNAL(timeout()), this, SLOT(on_Loop()));
        mLoopTimer->start(10);
    }

    mOneSecondTimer = new QTimer(this);
    connect(mOneSecondTimer, SIGNAL(timeout()), this, SLOT(on_OneSecond()));
    mOneSecondTimer->start(1000);
}

void MainWindow::on_Loop()
{
    loop();
    if (!mWorker)
    {
        updateLiquidCrystal();
    }
}

void MainWindow::on_OneSecond()
{
    if (mLiquidCrystal)
    {
        mLiquidCrystal->updateBlinkState();
    }
}

void MainWindow::updateLiquidCrystal()
{
    if (mLiquidCrystal)
    {
        if (mLiquidCrystal->isChanged())
        {
            QString fText;
            for (int fLine = 0; fLine < mLiquidCrystal->getLines(); ++fLine)
            {
                fText += QString("[") + mLiquidCrystal->getDisplayTextLine(fLine).c_str() + QString("]\n");
            }
            ui->textLCD_Sim->setText(fText);
            mLiquidCrystal->setUnchanged();
        }
        uint8_t fRow, fCol;
        if (mLiquidCrystal->getCursor(fRow, fCol))
        {
            QTextCursor cursor;
            if (mLiquidCrystal->getCursorState())
            {
                int pos = fRow * (mLiquidCrystal->getDisplayTextLine(0).size() + 2) + fCol + 1;
                cursor.setPosition(pos);
            }
            else
            {
                cursor.clearSelection();
            }
            ui->textLCD_Sim->setTextCursor(cursor);
        }
    }
}

int  MainWindow::getPinValue(int aPin, int aRange)
{
    if (aPin >= 0 && aPin < mListModel->rowCount())
    {
        if (aRange != 1)
        {
            mListModel->setData(mListModel->index(aPin, eRange, QModelIndex()), QString::number(aRange));
        }
        int fValue = mListModel->data(mListModel->index(aPin, eValue, QModelIndex())).toInt();
        if (mLogicAnalyser.isRecording() && isPinSelected(aPin))
        {
            QString fPinName = mListModel->data(mListModel->index(aPin, ePinNo, QModelIndex())).toString();
            mLogicAnalyser.setValue(fPinName.toStdString(), millis(),
                                    aRange != 1 ? static_cast<float>(fValue) / static_cast<float>(aRange) : fValue);
        }
        return fValue;
    }
    else
    {
        //printToSeriaDisplay("Wrong pin number: " + QString::number(aPin) + "\n");
    }

    return -1;
}

void MainWindow::setPinValue(int aPin, int aValue, int aRange)
{
    if (aPin >= 0 && aPin < mListModel->rowCount())
    {
        if (mWorker && mWorker->runningInCurrentThread())
        {
            QMutexLocker fLock(&mPinMutex);
            gmStaticPinAccess.push_back({ eSetPinValue, aPin, aValue, aRange });
        }
        else
        {
            if (aRange != 1)
            {
                mListModel->setData(mListModel->index(aPin, eRange, QModelIndex()), QString::number(aRange));
            }
            mListModel->setData(mListModel->index(aPin, eValue, QModelIndex()), QString::number(aValue));

            if (mLogicAnalyser.isRecording() && isPinSelected(aPin))
            {
                QString fPinName = mListModel->data(mListModel->index(aPin, ePinNo, QModelIndex())).toString();
                mLogicAnalyser.setValue(fPinName.toStdString(), millis(),
                                        aRange != 1 ? static_cast<float>(aValue) / static_cast<float>(aRange) : aValue);
            }
        }
    }
    else
    {
        printToSeriaDisplay("Wrong pin number: " + QString::number(aPin) + "\n");
    }
}

void MainWindow::setPin(const std::vector< int >& aSet )
{
    switch (aSet[0])
    {
    case ePinMode:     pinMode(aSet[1], aSet[2]); break;
    case eSetPinValue: setPinValue(aSet[1], aSet[2], aSet[3]); break;
    case eTone:        tone(aSet[1], aSet[2], aSet[3]); break;
    }

}


void MainWindow::on_Record(bool aRecord)
{
    if (aRecord)
    {
        QList<int> fPinList;
        QModelIndexList indexes = ui->tableView->selectionModel()->selection().indexes();
        if (indexes.count())
        {
            for (auto &findex : indexes)
            {
                fPinList.append(findex.row());
            }
        }
        else
        {
            for (int i=0; i<mListModel->rowCount(); ++i)
            {
                fPinList.append(i);
            }
        }

        for (auto &fPin : fPinList)
        {
            QString fPinName = mListModel->data(mListModel->index(fPin, ePinNo, QModelIndex())).toString();
            int fValue = mListModel->data(mListModel->index(fPin, eValue, QModelIndex())).toInt();
            int fRange  = mListModel->data(mListModel->index(fPin, eRange, QModelIndex())).toInt();
            mLogicAnalyser.setValue(fPinName.toStdString(), millis(),
                                    fRange != 1 ? static_cast<float>(fValue) / static_cast<float>(fRange) : fValue);
        }
    }
}

void MainWindow::on_ShowLogicAnalyser(bool aShow)
{
    ui->btnLogicAnalyzer->setChecked(aShow);
}



bool MainWindow::isPinSelected(int aPin)
{
    QModelIndexList indexes = ui->tableView->selectionModel()->selection().indexes();
    if (indexes.count())
    {
        if (std::find_if(indexes.begin(), indexes.end(), [aPin] (QModelIndexList::const_reference aR) { return aR.row() == aPin; } ) != indexes.end())
        {
            return true;
        }
        else
        {
            return false;
        }
    }
    return true;
}

void MainWindow::pinMode(int aPin, int aType)
{
    if (gmThis)
    {
        if (gmThis->mWorker && gmThis->mWorker->runningInCurrentThread())
        {
            QMutexLocker fLock(&gmThis->mPinMutex);
            gmStaticPinAccess.push_back({ ePinMode, aPin, aType });
        }
        else
        {
            if (aPin >= 0 && aPin < gmThis->mListModel->rowCount())
            {
                gmThis->mListModel->setData(gmThis->mListModel->index(aPin, ePinType, QModelIndex()), nameof_pintype(aType));
            }
            else
            {
                gmThis->printToSeriaDisplay("Wrong pin number: " + QString::number(aPin) + "\n");
            }
        }
    }
    else
    {
        gmStaticPinAccess.push_back({ ePinMode, aPin, aType });
    }
}

int  MainWindow::digitalRead(int aPin)
{
    if (gmThis)
    {
       return gmThis->getPinValue(aPin);
    }
    return 0;
}

void MainWindow::digitalWrite(int aPin, int aValue)
{
    if (gmThis)
    {
        gmThis->setPinValue(aPin, aValue);
    }
    else
    {
        gmStaticPinAccess.push_back({ eSetPinValue, aPin, aValue, 1});
    }
}

void MainWindow::digitalWrite(const std::vector<int>& aPins, int aValue)
{
    for (auto fPin : aPins)
    {
        digitalWrite(fPin, aValue);
    }
}

int  MainWindow::analogRead(int aPin)
{
    if (gmThis)
    {
        return gmThis->getPinValue(aPin, 1024);
    }
    return 0;
}

void MainWindow::analogWrite(int aPin, int aValue)
{
    if (gmThis)
    {
        gmThis->setPinValue(aPin, aValue, 256);
    }
    else
    {
        gmStaticPinAccess.push_back({ eSetPinValue, aPin, aValue, 256 });
    }
}

class Beeper : public QThread
{
public:
    typedef std::map<int, Beeper*> tThreadMap;
    static void beep(int aFrequency, int aLength_ms, int aPin)
    {
        new Beeper(aFrequency, aLength_ms, aPin);
    }

    static void nobeep(int aPin)
    {
        auto fThread = mThreadMap.find(aPin);
        if (fThread != mThreadMap.end())
        {
//            fThread->second->terminate();
        }
    }

private:
    Beeper(int aFrequency, int aLength, int aPin) : mFrequency(aFrequency), mLength(aLength), mPin(aPin)
    {
        connect(this, SIGNAL(finished()), this, SLOT(deleteLater()));
        if (aLength == 0)
        {
            mThreadMap[aPin] = this;
            aLength = 100000;
        }
        start();
    }
    ~Beeper()
    {
        auto fThread = mThreadMap.find(mPin);
        if (fThread != mThreadMap.end())
        {
            mThreadMap.erase(fThread);
        }
    }

    void run()
    {
        do_beep(mFrequency, mLength);
    }

    void do_beep(int aFrequency, int aLength_ms)
    {
        std::stringstream fCmd;
        fCmd << "( speaker-test -t sine -f" << aFrequency << " )& pid=$! ; sleep " << static_cast<double>(aLength_ms * 0.001) << "s ; kill -9 $pid";
        system(fCmd.str().c_str());
    }
    int mFrequency;
    int mLength;
    int mPin;
    static tThreadMap mThreadMap;
};

Beeper::tThreadMap Beeper::mThreadMap;

void MainWindow::tone(int aPin, int aFrequency, int aLength)
{
    if (gmThis)
    {
        Beeper::beep(aFrequency, aLength, aPin);
        gmThis->setPinValue(aPin, aFrequency);
    }
    else
    {
        gmStaticPinAccess.push_back( { eTone, aPin, aFrequency, aLength });
    }
}

void MainWindow::noTone(int aPin)
{
    if (gmThis)
    {
        if (aPin >= 0 && aPin < gmThis->mListModel->rowCount())
        {
            gmThis->mListModel->setData(gmThis->mListModel->index(aPin, eValue, QModelIndex()), "");
            Beeper::nobeep(aPin);
        }
        else
        {
            gmThis->printToSeriaDisplay("Wrong pin number: " + QString::number(aPin) + "\n");
        }
    }
}

int  MainWindow::pulseIn(int /* aPin */, int /* aLevel */, int /* aTimeout */)
{
    if (gmThis)
    {
        gmThis->printToSeriaDisplay("pulseIn not implemented\n");
    }
    return 0;
}


void MainWindow::on_btnSendText_clicked()
{
    sendSerialText(ui->textToSendToSerialDevice->text());
}

void MainWindow::on_bntSendFile_clicked()
{
    QString fileName = QFileDialog::getOpenFileName(this, tr("Open File"), tr(""), tr("Textfile (*.txt *.*)"));
    QFile fFile;
    if (fileName.length())
    {
        fFile.setFileName(fileName);
        if (fFile.open(QIODevice::ReadOnly|QIODevice::Text))
        {
            bool fOK = true;
            do
            {
                QByteArray fLine = fFile.readLine();
                fOK = fLine.length() > 0;
                if (fOK)
                {
                    sendSerialText(fLine);
                }
            }
            while (fOK);
        }
    }
}

void MainWindow::on_btnClearText_clicked()
{
    ui->textReceivedFromSerialDevice->setText("");
}

void MainWindow::on_btnDoTest_clicked()
{
    doTest();
}

int MainWindow::getFirstAnalogPin()
{
    if (gmThis)
    {
        return gmThis->mFirstAnalogPin;
    }
    return 14;
}


std::vector<int>  MainWindow::getButtonPin(const QLineEdit& aLineEdit, int& aValue) const
{
    std::vector<int> fPins;
    bool bNegative = false;
    QStringList fPinList = aLineEdit.text().split(',');
    for (auto fPinVal : fPinList)
    {
        fPins.push_back(fPinVal.toInt());
        if (*fPins.rbegin() < 0)
        {
            *fPins.rbegin() = -(*fPins.rbegin());
            bNegative = true;
        }
    }
    if (bNegative)
    {
        aValue = !aValue;
    }

    return fPins;
}

void MainWindow::on_textChanged(const QString& str)
{
    auto *edit = dynamic_cast<QLineEdit*>(sender());
    int index = m_edits.indexOf(edit);
    if (index >= 0 && index < m_buttons.size())
    {
        bool ok = false;
        QStringList pins = str.split(",");
        if (pins.size())
        {
            int pin = abs(pins[0].toInt(&ok));
            ok = ok && pin >= 0 && pin < mListModel->rowCount();
        }
        m_buttons[index]->setEnabled(ok);
    }
}


void MainWindow::on_btnPressed()
{
    auto *button = dynamic_cast<QPushButton*>(sender());
    int index = m_buttons.indexOf(button);
    if (index >= 0 && index < m_edits.size())
    {
        int fValue = 1;
        std::vector<int> fPins = getButtonPin(*m_edits[index], fValue);
        digitalWrite(fPins, fValue);
    }
}

void MainWindow::on_btnReleased()
{
    auto *button = dynamic_cast<QPushButton*>(sender());
    int index = m_buttons.indexOf(button);
    if (index >= 0 && index < m_edits.size())
    {
        int fValue = 0;
        std::vector<int> fPins = getButtonPin(*m_edits[index], fValue);
        digitalWrite(fPins, fValue);
    }
}

void MainWindow::setLiquidCrystal(LiquidCrystal *aLCD)
{
    mLiquidCrystal = aLCD;
}

QString getSettingsName(const QString& aItemName)
{
    QRegExp fRegEx("([A-Z][A-Za-z0-9]+)");
    int fPos = fRegEx.indexIn(aItemName);
    if (fPos != -1 && fRegEx.captureCount())
    {
        return fRegEx.capturedTexts()[0];
    }
    else return aItemName;
}

void MainWindow::on_btnLogicAnalyzer_clicked(bool checked)
{
    if (checked)
    {
        mLogicAnalyser.show();
    }
    else
    {
        mLogicAnalyser.hide();
    }
}
