#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QMutex>
#include <logic_analyser.h>
#include <arduinoworker.h>

#include <sstream>
#include <memory>

class QAbstractItemModel;
class QModelIndex;
class QLineEdit;
class QTime;
class LiquidCrystal;

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

    enum eFunction
    {
        ePinMode, eSetPinValue, eTone
    };

    typedef std::vector< std::vector< int > > tPinAccess;
public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

    static void pinMode(int aPin, int aType);

    static int  digitalRead(int aPin);
    static void digitalWrite(int aPin, int aValue);

    static int  analogRead(int aPin);
    static void analogWrite(int aPin, int aValue);

    static void tone(int aPin, int aFrequency, int aLength);
    static void noTone(int aPin);

    static int  pulseIn(int aPin, int aLevel, int aTimeout);

    static int  getFirstAnalogPin();
    static int  elapsed();


    void setLiquidCrystal(LiquidCrystal* aLCD);
    int  getPinValue(int aPin, int aRange=1);
    void setPinValue(int aPin, int aValue, int aRange=1);
    bool isPinSelected(int aPin);
    void setPin(const std::vector< int >& aSet );

private:
    void digitalWrite(const std::vector<int>& aPins, int aValue);
    std::vector<int>  getButtonPin(const QLineEdit& aLineEdit, int& aValue) const;
    void stopArduino();


public Q_SLOTS:
    void sendSerialText(const QString& aText);
    void printToSeriaDisplay(const QString& aText);

    void on_Setup();
    void on_Loop();

    void on_OneSecond();
    void on_Record(bool );
    void on_ShowLogicAnalyser(bool );
    void updateLiquidCrystal();
    void setPins();

private Q_SLOTS:
    void on_btnDoTest_clicked();
    void on_btnClearText_clicked();
    void on_btnSendText_clicked();
    void on_bntSendFile_clicked();

    void on_btnReleased();
    void on_btnPressed();
    void on_textChanged(const QString&str);
    void on_btnLogicAnalyzer_clicked(bool checked);
    void on_comboArduinoCurrentIndexChanged(int index);

private:
    Ui::MainWindow *ui;
    QAbstractItemModel* mListModel;
    int  mFirstAnalogPin;
    QTime* mArduinoTime;
    LiquidCrystal* mLiquidCrystal;
    LogicAnalyser  mLogicAnalyser;
    std::unique_ptr<ArduinoWorker>  mWorker;
    QList<QPushButton*> m_buttons;
    QList<QLineEdit*>   m_edits;
    bool mUseWorkerThread;
    QList<QStringList> mArduinoList;
    QTimer*            mLoopTimer = nullptr;
    QTimer*            mOneSecondTimer = nullptr;
    QTimer*            mSetPinsTimer = nullptr;

    static MainWindow* gmThis;
    QMutex mPinMutex;
    static tPinAccess gmStaticPinAccess;
    bool              mPinsSet = false;
};

#endif // MAINWINDOW_H
