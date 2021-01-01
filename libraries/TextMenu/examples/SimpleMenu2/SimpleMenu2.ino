/// Sample for usage of library menu
/// Available classes in this library:
/// - MenuItem a menu that may contain submenues or editable values, selectable lists or simple commands
/// - ItemSelect a list item selector
/// - ItemValue a template class for editing floating point or integral values


#include <menu.h>

/// additional used libraries:
#include <button.h>         // implementation of a simple debounced button
#include <LiquidCrystal.h>  // implementation of an LCD control class

// LCD display
LiquidCrystal LCD(3, 4, 5, 6, 7, 8);

#define LCD_XDIM 16
#define LCD_YDIM 2

#include <inttypes.h>


void displayMenuItem();
void displayAll();

long   gLongValue   = 123;
double gDoubleValue = 123;

void notifyMenu(uint8_t aDirOrIndex, uint8_t aID);

// Menu implementation
const char * listitems[] = { "item0", "item1", "item2", "item3", "item4", "item5" };
ItemSelect         gSelectListItem(listitems);
ItemValue<long>    gEditLong(gLongValue, 100, 200);
ItemValue<double>  gEditDouble(gDoubleValue, -10, 1200);

enum eIDs
{
    eListItem,
    eLongValue,
    eDoubleValue,
    eCommand
};

const char* nameof(eIDs id)
{
    switch(id)
    {
    case eListItem:    return "ListItem";
    case eLongValue:   return "LongValue";
    case eDoubleValue: return "DoubleValue";
    case eCommand:     return "Command";
    default: return "unknown";
    }

}

MenuItem gMainMenu[] =
{
    MenuItem("Item:"  , &gSelectListItem, eListItem, length_of(listitems), MenuItem::no_cursor|MenuItem::limit_turn),
    MenuItem("Long:"  , &gEditLong      , eLongValue, 3),
    MenuItem("Double:", &gEditDouble    , eDoubleValue, 3, MenuItem::float_value|MenuItem::limit_turn),
    MenuItem("Command", (ItemEditBase*)0, eCommand)
};

MenuItem gMenu("SimpleMenu2...", gMainMenu, length_of(gMainMenu));

// Button implementation for input pins with internal pullup
// definition of button numbers realized using a button matrix connected to three pins
#define  BTN_F1      1      // 001
#define  BTN_ENTER   2      // 010
#define  BTN_DOWN    3      // 011
#define  BTN_LEFT    4      // 100
#define  BTN_ESCAPE  5      // 101
#define  BTN_RIGHT   6      // 110
#define  BTN_UP      7      // 111

void    triggerButton(uint8_t aState, uint8_t aID);
uint8_t gButtonPins[3] = { 12, 11, 10 };
Button  gButtons(gButtonPins, sizeof(gButtonPins), triggerButton);

void setup()
{
  MenuItem::setNotificationFunction(notifyMenu);
  LCD.begin(LCD_XDIM, LCD_YDIM);
  LCD.clear();

  gButtons.setDeBounce(50);
  gButtons.setDelay(1000);
  gButtons.setRepeat(250);

  Serial.begin(115200);

  displayMenuItem();
}

void loop()
{
  unsigned long fNow = millis();
  gButtons.tick(fNow);
}


void notifyMenu(uint8_t aDirOrIndex, uint8_t aID)
{
    Serial.print("notifyMenu(");
    if (aID != eListItem)
    {
        Serial.print(MenuItem::dir::nameof(static_cast<MenuItem::dir::to>(aDirOrIndex)));
    }
    else
    {
        Serial.print((int)aDirOrIndex);
    }
    Serial.print(", ");
    Serial.print(nameof(static_cast<eIDs>(aID)));
    Serial.println(")");
}

MenuItem::dir::to toMenuBtn(uint8_t aPin)
{
  switch(aPin)
  {
    case BTN_LEFT:   return MenuItem::dir::left;
    case BTN_RIGHT:  return MenuItem::dir::right;
    case BTN_UP:     return MenuItem::dir::up;
    case BTN_DOWN:   return MenuItem::dir::down;
    case BTN_ESCAPE: return MenuItem::dir::escape;
    case BTN_ENTER:  return MenuItem::dir::enter;
    default:         return MenuItem::dir::unknown;
  }
}

void triggerButton(uint8_t aState, uint8_t aPin)
{
  switch (aState)
  {
    case Button::pressed:
    case Button::delayed:
    case Button::repeated:
    gMenu.pressBtn(toMenuBtn(aPin));
    displayMenuItem();
    break;
  }
}

String fLine0;
String fLine1;

void displayMenuItem()
{
    int fCursor = 0;
    String fString = gMenu.getText(&fCursor);
    fillStringWithSpaces(fString, LCD_XDIM);
    if (fLine1 != fString)
    {
        LCD.setCursor(0, 1);
        LCD.print(fString);
        fLine1 = fString;
    }

    fString = gMenu.getPath();
    fString.replace("...", "");
    fillStringWithSpaces(fString, LCD_XDIM);
    if (fLine0 != fString)
    {
        LCD.setCursor(0, 0);
        LCD.print(fString);
        fLine0 = fString;
    }

    if (fCursor)
    {
        LCD.setCursor(fCursor, 1);
        LCD.cursor();
    }
    else
    {
        LCD.noCursor();
    }
}
