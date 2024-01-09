#include <menu.h>

// NOTE: may be undefined to reduce the size of the sketch
//#define USE_ACCURACY 1
//#define USE_SSV 1

enum eProtocol
{
    Off,
    Pashr,
    Tss1,
#if USE_SSV == 1
    SSV,
#endif
};

enum eOperationMode : uint16_t
{
    Simulation,
    MeasureYawPitchRoll,
    MeasureEuler
};

struct SimulatorItem
{
    uint16_t mInterval;
    float    mMin;
    float    mMax;
    float    mValue;
#if USE_ACCURACY == 1
    float    mAccuracy;
#endif
};

struct PersistentData
{
    long mBaudrate;
    uint16_t mInterval;     // ms;
    uint16_t mProtocoll;    // Off, PASHR, TSS1, SSV
    uint16_t mOperationMode;// Simulation, YawPitchRoll, Euler
    SimulatorItem mHeave;
    SimulatorItem mRoll;
    SimulatorItem mPitch;
    SimulatorItem mHeading;
#if USE_SSV == 1
    SimulatorItem mSSV;
#endif
};

enum eKeys
{
    Baudrate = 0,
    Protocol,
    Settings,
    Value,
    Simulate,
    Interval
};

enum eSettings
{
    Store,
    Clear
};


extern PersistentData gData;

void initDefaultData();
void   printPASHR();
void   printTSS1();
void   printSSV();

void   modifyValues(unsigned long aNow);
void   calculateValue(SimulatorItem* aItem, unsigned long aNow);


MenuItem& getMenu();
ItemSelect& getProtocolItem();
ItemSelect& getBaudrateItem();
const char * getOperationMode(int);
const char * getProtocol(int);
const char * getBaudrate(int);
