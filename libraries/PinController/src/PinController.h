#ifndef PinController_h
#define PinController_h

/*!
 * PinController.h - library for switching pins on and off or controlling
 * analog pins using a predefined function
 * The library switches pins on and off asynchronously controlled by
 * repeatedly calling the tick(..) function within the function loop().
 */
#include <inttypes.h>

class TimerController
{
public:
    enum e { one_shot=1, for_ever = 255 };
    //! @brief starts playing the melody
    virtual void start();

    //! @brief stops playing the melody
    void stop();

    //! @brief determines, if melody playing is active
    bool isPlaying();

protected:
    TimerController(uint8_t repeats);
    bool do_repeat();

    uint8_t  mRepeats;
    uint8_t  mCurrentRepeat;
    int16_t  mCurrentCommand;
    unsigned long mNextTime_ms;
};

#define PINS02(                                                               p2, p1)                                                                                                  p2<<1|p1
#define PINS03(                                                           p3, p2, p1)                                                                                            p3<<2|p2<<1|p1
#define PINS04(                                                       p4, p3, p2, p1)                                                                                      p4<<3|p3<<2|p2<<1|p1
#define PINS05(                                                   p5, p4, p3, p2, p1)                                                                                p5<<4|p4<<3|p3<<2|p2<<1|p1
#define PINS06(                                               p6, p5, p4, p3, p2, p1)                                                                          p6<<5|p5<<4|p4<<3|p3<<2|p2<<1|p1
#define PINS07(                                           p7, p6, p5, p4, p3, p2, p1)                                                                    p7<<6|p6<<5|p5<<4|p4<<3|p3<<2|p2<<1|p1
#define PINS08(                                       p8, p7, p6, p5, p4, p3, p2, p1)                                                              p8<<7|p7<<6|p6<<5|p5<<4|p4<<3|p3<<2|p2<<1|p1
#define PINS09(                                   p9, p8, p7, p6, p5, p4, p3, p2, p1)                                                        p9<<8|p8<<7|p7<<6|p6<<5|p5<<4|p4<<3|p3<<2|p2<<1|p1
#define PINS10(                              p10, p9, p8, p7, p6, p5, p4, p3, p2, p1)                                                 p10<<9|p9<<8|p8<<7|p7<<6|p6<<5|p5<<4|p4<<3|p3<<2|p2<<1|p1
#define PINS11(                         p11, p10, p9, p8, p7, p6, p5, p4, p3, p2, p1)                                         p11<<10|p10<<9|p9<<8|p8<<7|p7<<6|p6<<5|p5<<4|p4<<3|p3<<2|p2<<1|p1
#define PINS12(                    p12, p11, p10, p9, p8, p7, p6, p5, p4, p3, p2, p1)                                 p12<<11|p11<<10|p10<<9|p9<<8|p8<<7|p7<<6|p6<<5|p5<<4|p4<<3|p3<<2|p2<<1|p1
#define PINS13(               p13, p12, p11, p10, p9, p8, p7, p6, p5, p4, p3, p2, p1)                         p13<<12|p12<<11|p11<<10|p10<<9|p9<<8|p8<<7|p7<<6|p6<<5|p5<<4|p4<<3|p3<<2|p2<<1|p1
#define PINS14(          p14, p13, p12, p11, p10, p9, p8, p7, p6, p5, p4, p3, p2, p1)                 p14<<13|p13<<12|p12<<11|p11<<10|p10<<9|p9<<8|p8<<7|p7<<6|p6<<5|p5<<4|p4<<3|p3<<2|p2<<1|p1
#define PINS15(     p15, p14, p13, p12, p11, p10, p9, p8, p7, p6, p5, p4, p3, p2, p1)         p15<<14|p14<<13|p13<<12|p12<<11|p11<<10|p10<<9|p9<<8|p8<<7|p7<<6|p6<<5|p5<<4|p4<<3|p3<<2|p2<<1|p1
#define PINS16(p16, p15, p14, p13, p12, p11, p10, p9, p8, p7, p6, p5, p4, p3, p2, p1) p16<<15|p15<<14|p14<<13|p13<<12|p12<<11|p11<<10|p10<<9|p9<<8|p8<<7|p7<<6|p6<<5|p5<<4|p4<<3|p3<<2|p2<<1|p1

#ifndef lengthof
  #define lengthof(array) sizeof(array) / sizeof(array[0])
#endif

typedef struct
{
    uint16_t mBits;     // 16 bits
    uint16_t mDelay;    // delay for each command
                        // if common delay is used this value may be used for further pin bits
} Command;

class DigitalPinController : public TimerController
{
public:
    enum flags
    {
        none = 0,
        reverse      = 1,
        common_delay = 2
    };
    //! @brief constructor with parameters
    //! @param outputPin No
    //! @param pCommands array with command bits zero terminated, see structure tones
    //! @param commands length of array with commands
    //! @param repeats of control commands or for_ever
    DigitalPinController( uint8_t* pOutputPin, Command* pCommands, uint16_t commands, uint8_t repeats=for_ever);

    //! @brief controlles events to switch pins on and off
    //! @param current_ms current time determined by millis()
    //! @note must be called repeatedly within loop()
    void tick(unsigned long current_ms);

    //! @brief sets a new array with commands
    //! @param pCommands array with commands, see structure Commands
    //! @param commands length of array with commands
    void setCommands(Command* aCommands, uint16_t commands, uint8_t repeats=for_ever);

    //! @brief sets or clears control flags
    //! @param flags, see enum flags
    //! @param set (true, false) -> (set, clear) flags
    void setFlags(uint8_t flags, bool set);

    //! @brief sets common delay
    //! @param common delay
    //! @note sets flag common_delay automatically
    void setCommonDelay(uint16_t delay);

    //! @brief sets value of all pins
    //! @param value (LOW, HIGH)
    void set_all_pins(uint8_t value);

private:
    DigitalPinController();
    bool is_set(uint8_t);

    uint8_t* mpOutputPin;
    Command* mpCommands;
    uint16_t mCommands;
    uint8_t  mFlags;
    uint16_t mDelay;
};


struct Function
{
    enum eFunction
    {
        none,
        set_start_value,
        linear_ramp,
        constant,
        sine,
        sine_half_1,
        sine_half_2,
        sine_quater_1,
        sine_quater_2,
        sine_quater_3,
        sine_quater_4,
    };
    eFunction mFunction;
    uint16_t  mValue;
    uint16_t  mPeriod;
};

class AnalogPinController: public TimerController
{
public:
    //! @brief constructor with parameters
    //! @param outputPin No
    //! @param pCommands array with functions zero terminated, see structure Functions
    //! @note the last entry must contain zeros
    //! @param repeats of control commands or (one_shot, for_ever)
    AnalogPinController( uint8_t* OutputPins, Function* pFunctions, uint8_t repeats=for_ever);

    //! @brief controles events to control analog pin
    //! @param current_ms current time determined by millis()
    //! @note must be called repeatedly within loop()
    void tick(unsigned long current_ms);

    //! @brief sets a new functions for controlling analog pin
    //! @param functions array with mathematical functions zero terminated, see structure Function
    //! @note the last entry must contain zeros
    void setFunctions(Function* functions, uint8_t repeats=for_ever);

    //! @brief sets value of all pins
    //! @param value (0, ..., 256 [4096])
    void set_value_of_all_pins(uint16_t value);

private:
    AnalogPinController();
    int32_t calculate_linear_ramp(uint16_t current_time, uint16_t length_ms);

    uint8_t*  mOutputPins;
    Function* mpFunctions;
    uint16_t  mPreviousValue;
    uint16_t  mPeriodStep;
    unsigned long mPreviousTime_ms;
};

typedef bool (*TriggerFunc)();

class TimerFunction: public TimerController
{
public:
    //! @brief constructor with parameters
    //! @param function callback function to be called after timeout
    //! @param repeats of function calls or for_ever
    TimerFunction(TriggerFunc function, uint16_t timeout, uint8_t repeats=for_ever);

    //! @brief controles events to call the callback function
    //! @param current_ms current time determined by millis()
    //! @note must be called repeatedly within loop()
    void tick(unsigned long current_ms);

    //! @brief sets a new callback function
    //! @param function callback function to be called after timeout
    //! @param repeats of function calls or (one_shot, for_ever)
    void setTimerFunction(TriggerFunc function, uint16_t timeout, uint8_t repeats=for_ever);

private:
    TriggerFunc mTriggerFunction;
    uint16_t mTimeout;
};

#endif
