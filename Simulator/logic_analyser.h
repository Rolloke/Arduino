#ifndef LOGIC_ANALYSER_H
#define LOGIC_ANALYSER_H

#include <logicview.h>

#include <QDialog>
#include <memory>


namespace Ui {
class LogicAnalyser;
}

class LogicAnalyser : public QDialog
{
    Q_OBJECT

public:

    explicit LogicAnalyser(QWidget *parent = 0);
    ~LogicAnalyser();

    bool isRecording();
    void setValue(const std::string& aPin, int aTime, float aValue);

    int (*millis)();

protected:
    void setVisible(bool visible) override;
    void timerEvent(QTimerEvent *) override;
    void resizeEvent(QResizeEvent *) override;

Q_SIGNALS:
    void record(bool );
    void isshown(bool );

public Q_SLOTS:
    void on_btnUpdate_clicked();

private Q_SLOTS:
    void on_btnClear_clicked();
    void on_btnRecord_clicked(bool checked);
    void on_ckAutoUpdate_clicked(bool checked);

private:
    void updateVerticalLayoutItems();


    std::unique_ptr<PinMap> mPinMap;
    int  mUpdateTimerID;
    Ui::LogicAnalyser *ui;
};

#endif // LOGIC_ANALYSER_H
