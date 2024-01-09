#include "OctansSimulatorMenu.h"

#include <texttools.h>
#include <Time.h>

PersistentData gData;

// Menu implementation
const char * gOperationMode[] =
{
    "Simulation"
  , "YPR Angles"
  , "Euler Angles"
};
ItemValue<uint16_t>  gOperationModeItem(gData.mOperationMode, Simulation, MeasureEuler);

ItemValue<uint16_t>  gInterval(gData.mInterval, 5, 10000);

const char * gBaudrate[] =
{
    "2400"
  , "4800"
  , "9600"
  , "14400"
  , "19200"
  , "38400"
  , "57600"
  , "115200"
};
ItemSelect gBaudrateItem(gBaudrate);

const char * gProtocol[] =
{
    "Off"
  , "PASHR"
  , "TSS1"
 #if USE_SSV == 1
  , "SSV"
 #endif
};
ItemSelect gProtocolItem(gProtocol);

const char * gSettings[] =
{
    "Store"
  , "Clear"
};
ItemSelect gSettingsItem(gSettings);


const char * sValue    = "Value:";
const char * sMin      = "Min:";
const char * sMax      = "Max:";
const char * sAccuracy = "Accuracy:";
const char * sInterval = "Interval:";

namespace ItemMenuHeading
{
ItemValue<float>    gValue(   gData.mHeading.mValue   ,   0, 360);
ItemValue<float>    gMin(     gData.mHeading.mMin     ,   0, 360);
ItemValue<float>    gMax(     gData.mHeading.mMax     ,   0, 360);
#if USE_ACCURACY == 1
ItemValue<float>    gAccuracy(gData.mHeading.mAccuracy,   0, 10);
#endif
ItemValue<uint16_t> gInterval(gData.mHeading.mInterval,   0, 65535);

MenuItem gSimulator[] =
{
    MenuItem(sValue,    &gValue   , Value, 2, MenuItem::float_value|MenuItem::limit_turn),
    MenuItem(sMin,      &gMin     , Value, 2, MenuItem::float_value|MenuItem::limit_turn),
    MenuItem(sMax,      &gMax     , Value, 2, MenuItem::float_value|MenuItem::limit_turn),
    #if USE_ACCURACY == 1
    MenuItem(sAccuracy, &gAccuracy, Value, 2, MenuItem::float_value|MenuItem::limit_turn),
    #endif
    MenuItem(sInterval, &gInterval, Value, 5)
};
}

namespace ItemMenuHeave
{
ItemValue<float>    gValue(   gData.mHeave.mValue   , -30, 30);
ItemValue<float>    gMin(     gData.mHeave.mMin     , -10, 10);
ItemValue<float>    gMax(     gData.mHeave.mMax     , -10, 10);
#if USE_ACCURACY == 1
ItemValue<float>    gAccuracy(gData.mHeave.mAccuracy,   0, 10);
#endif
ItemValue<uint16_t> gInterval(gData.mHeave.mInterval,   0, 65535);

MenuItem gSimulator[] =
{
    MenuItem(sValue,    &gValue   , Value, 2, MenuItem::float_value|MenuItem::limit_turn),
    MenuItem(sMin,      &gMin     , Value, 2, MenuItem::float_value|MenuItem::limit_turn),
    MenuItem(sMax,      &gMax     , Value, 2, MenuItem::float_value|MenuItem::limit_turn),
    #if USE_ACCURACY == 1
    MenuItem(sAccuracy, &gAccuracy, Value, 2, MenuItem::float_value|MenuItem::limit_turn),
    #endif
    MenuItem(sInterval, &gInterval, Value, 5)
};
}

namespace ItemMenuRoll
{
ItemValue<float>    gValue(   gData.mRoll.mValue   , -90, 90);
ItemValue<float>    gMin(     gData.mRoll.mMin     , -45, 45);
ItemValue<float>    gMax(     gData.mRoll.mMax     , -45, 45);
#if USE_ACCURACY == 1
ItemValue<float>    gAccuracy(gData.mRoll.mAccuracy,   0, 10);
#endif
ItemValue<uint16_t> gInterval(gData.mRoll.mInterval,   0, 65535);

MenuItem gSimulator[] =
{
    MenuItem(sValue,    &gValue   , Value, 2, MenuItem::float_value|MenuItem::limit_turn),
    MenuItem(sMin,      &gMin     , Value, 2, MenuItem::float_value|MenuItem::limit_turn),
    MenuItem(sMax,      &gMax     , Value, 2, MenuItem::float_value|MenuItem::limit_turn),
    #if USE_ACCURACY == 1
    MenuItem(sAccuracy, &gAccuracy, Value, 2, MenuItem::float_value|MenuItem::limit_turn),
    #endif
    MenuItem(sInterval, &gInterval, Value, 5)
};
}

namespace ItemMenuPitch
{
ItemValue<float>    gValue(   gData.mPitch.mValue   , -90, 90);
ItemValue<float>    gMin(     gData.mPitch.mMin     , -45, 45);
ItemValue<float>    gMax(     gData.mPitch.mMax     , -45, 45);
#if USE_ACCURACY == 1
ItemValue<float>    gAccuracy(gData.mPitch.mAccuracy,   0, 10);
#endif
ItemValue<uint16_t> gInterval(gData.mPitch.mInterval,   0, 65535);

MenuItem gSimulator[] =
{
    MenuItem(sValue,    &gValue   , Value, 2, MenuItem::float_value|MenuItem::limit_turn),
    MenuItem(sMin,      &gMin     , Value, 2, MenuItem::float_value|MenuItem::limit_turn),
    MenuItem(sMax,      &gMax     , Value, 2, MenuItem::float_value|MenuItem::limit_turn),
    #if USE_ACCURACY == 1
    MenuItem(sAccuracy, &gAccuracy, Value, 2, MenuItem::float_value|MenuItem::limit_turn),
    #endif
    MenuItem(sInterval, &gInterval, Value, 5)
};
}

#if USE_SSV == 1
namespace ItemMenuSSV
{
ItemValue<float>    gValue(   gData.mSSV.mValue   , 1200, 1800);
ItemValue<float>    gMin(     gData.mSSV.mMin     , 1200, 1800);
ItemValue<float>    gMax(     gData.mSSV.mMax     , 1200, 1800);
#if USE_ACCURACY == 1
ItemValue<float>    gAccuracy(gData.mSSV.mAccuracy, 0, 10);
#endif
ItemValue<uint16_t> gInterval(gData.mSSV.mInterval, 0, 65535);

MenuItem gSimulator[] =
{
    MenuItem(sValue,    &gValue   , Value, 3, MenuItem::float_value|MenuItem::limit_turn),
    MenuItem(sMin,      &gMin     , Value, 3, MenuItem::float_value|MenuItem::limit_turn),
    MenuItem(sMax,      &gMax     , Value, 3, MenuItem::float_value|MenuItem::limit_turn),
    #if USE_ACCURACY == 1
    MenuItem(sAccuracy, &gAccuracy, Value, 2, MenuItem::float_value|MenuItem::limit_turn),
    #endif
    MenuItem(sInterval, &gInterval, Value, 5)
};
}
#endif

MenuItem gMainMenu[] =
{
    MenuItem("Protocol:" , &gProtocolItem, Protocol   , length_of(gProtocol), MenuItem::no_cursor|MenuItem::limit_turn),
    MenuItem("Heave..."  , ItemMenuHeave::gSimulator  , length_of(ItemMenuHeading::gSimulator)),
    MenuItem("Roll..."   , ItemMenuRoll::gSimulator   , length_of(ItemMenuHeading::gSimulator)),
    MenuItem("Pitch..."  , ItemMenuPitch::gSimulator  , length_of(ItemMenuHeading::gSimulator)),
    MenuItem("Heading...", ItemMenuHeading::gSimulator, length_of(ItemMenuHeading::gSimulator)),
    #if USE_SSV == 1
    MenuItem("SSV..."    , ItemMenuSSV::gSimulator    , length_of(ItemMenuHeading::gSimulator)),
    #endif
    MenuItem("Baudrate:" , &gBaudrateItem, Baudrate   , length_of(gBaudrate), MenuItem::no_cursor|MenuItem::limit_turn),
    MenuItem(sInterval   , &gInterval, Interval, 5),
    MenuItem("Operation:", &gOperationModeItem, Simulate, 1, MenuItem::no_cursor|MenuItem::limit_turn),
    MenuItem("Settings:" , &gSettingsItem, Settings   , length_of(gSettings), MenuItem::no_cursor|MenuItem::limit_turn)
};

MenuItem gMenu(0, gMainMenu, length_of(gMainMenu));


void initDefaultData()
{
    gData.mBaudrate       = 38400; // Bd
    gData.mInterval       = 1000;   // ms
    gData.mProtocoll      = Off;
    gData.mOperationMode  = Simulation;
#if USE_ACCURACY == 1
    gData.mHeave   = { 10000, -5.0, 5.0,   0.0, 0.1 };
    gData.mRoll    = { 10000, -5.0, 5.0,   0.0, 0.1 };
    gData.mPitch   = { 10000, -5.0, 5.0,   0.0, 0.1 };
    gData.mHeading = { 10000, -5.0, 5.0,   0.0, 0.1 };
#if USE_SSV == 1
    gData.mSSV     = { 10000, 1450, 1550, 1500, 0.1 };
#endif
#else
    gData.mHeave   = { 10000, -5.0, 5.0,   0.0 };
    gData.mRoll    = { 10000, -5.0, 5.0,   0.0 };
    gData.mPitch   = { 10000, -5.0, 5.0,   0.0 };
    gData.mHeading = { 10000, -5.0, 5.0,   0.0 };
#if USE_SSV == 1
    gData.mSSV     = { 10000, 1450, 1550, 1500 };
#endif
#endif
}

void calculateValue(SimulatorItem* aItem, unsigned long aNow)
{
    if (aItem->mInterval && (aItem->mMax - aItem->mMin) != 0)
    {
        uint16_t fArg =  aNow % aItem->mInterval;
        aItem->mValue = (aItem->mMax + aItem->mMin) * 0.5 +  (aItem->mMax - aItem->mMin) * 0.5 * sin(6.28*(float)fArg/(float)aItem->mInterval);
    }
}

void modifyValues(unsigned long aNow)
{
    switch (gData.mProtocoll)
    {
    case Pashr:
        calculateValue(&gData.mHeave  , aNow);
        calculateValue(&gData.mRoll   , aNow);
        calculateValue(&gData.mPitch  , aNow);
        calculateValue(&gData.mHeading, aNow);
        break;
    case Tss1:
        calculateValue(&gData.mHeave, aNow);
        calculateValue(&gData.mRoll , aNow);
        calculateValue(&gData.mPitch, aNow);
        break;
#if USE_SSV == 1
    case SSV:
        calculateValue(&gData.mSSV, aNow);
        break;
#endif
    }
}

void  printPASHR()
{
    String sPASHR;
    // sPASHR = "$PASHR,200345.00,78.00,T,-3.00,+2.00,+0.00,1.000,1.000,1.000,1,1*32";
    time_t fNow = now();
    tmElements_t fTime;
    breakTime(fNow, fTime);

    sPASHR = "$PASHR,";
    if (fTime.Hour < 10) sPASHR.concat('0');
    sPASHR.concat(static_cast<int>(fTime.Hour));
    if (fTime.Minute < 10) sPASHR.concat('0');
    sPASHR.concat(static_cast<int>(fTime.Minute));
    if (fTime.Second < 10) sPASHR.concat('0');
    sPASHR.concat(static_cast<int>(fTime.Second));
    sPASHR.concat(','); // Heading HHH.HH
    sPASHR.concat(printFloat(gData.mHeading.mValue, 0, 2, false));
    sPASHR.concat(",T,"); // True Heading, Roll RRR.RR
    sPASHR.concat(printFloat(gData.mRoll.mValue, 0, 2));
    sPASHR.concat(','); // Pitch PPP.PP
    sPASHR.concat(printFloat(gData.mPitch.mValue, 0, 2));
    sPASHR.concat(','); // Pitch PPP.PP
    sPASHR.concat(printFloat(gData.mHeave.mValue, 0, 2));
#if USE_ACCURACY ==1
    sPASHR.concat(',');  // Roll Accuracy
    sPASHR.concat(printFloat(gData.mRoll.mAccuracy, 0, 3));
    sPASHR.concat(',');  // Pitch Accuracy
    sPASHR.concat(printFloat(gData.mPitch.mAccuracy, 0, 3));
    sPASHR.concat(',');  // Heading Accuracy
    sPASHR.concat(printFloat(gData.mHeading.mAccuracy, 0, 3));
#else
    sPASHR.concat(",0.1,0.1,0.1");
#endif
    sPASHR.concat(",1"); // No Position
    sPASHR.concat(",1,*"); // INS Status Flag
    uint8_t fChkSum = 0;
    for (uint8_t i=0; i<sPASHR.length(); ++i)
    {
        fChkSum = fChkSum ^ sPASHR[i];
    }
    sPASHR.concat(String(fChkSum, HEX)); // INS Status Flag
    Serial.println(sPASHR);
}

void  printTSS1()
{
    //String sTSS1 = ":013D11 0000U-00510013"
    String sTSS1 = ":000000";
    sTSS1.concat(gData.mHeave.mValue < 0 ? "-" : " ");
    sTSS1.concat(printFloat(gData.mHeave.mValue*100, 4, 0, false));
    sTSS1.concat("U");
    sTSS1.concat(gData.mRoll.mValue < 0 ? "-" : " ");
    sTSS1.concat(printFloat(gData.mRoll.mValue*100, 4, 0, false));
    sTSS1.concat(gData.mPitch.mValue < 0 ? "-" : " ");
    sTSS1.concat(printFloat(gData.mPitch.mValue*100, 4, 0, false));
    Serial.println(sTSS1);
}

#if USE_SSV == 1
void  printSSV()
{
    Serial.println(printFloat(gData.mSSV.mValue, 1, 2, false));
}
#endif


MenuItem& getMenu()
{
    return gMenu;
}

ItemSelect& getProtocolItem()
{
    return gProtocolItem;
}

ItemSelect& getBaudrateItem()
{
    return gBaudrateItem;
}

const char * getOperationMode(int i)
{
    return gOperationMode[i];
}

const char * getProtocol(int i)
{
    return gProtocol[i];
}

const char * getBaudrate(int i)
{
    return gBaudrate[i];
}
