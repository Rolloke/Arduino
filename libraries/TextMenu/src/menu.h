#ifndef MENU_H
#define MENU_H

/// implements a menue displayed in an LCD or other text displays
///
/// the menu text is retrieved by two functions:
/// - MenuItem::getText(): item text
/// - MenuItem::getPath(): path to item including all submenues
/// each menue can contain submenues or editable subitems
/// the menue is controlled by buttons (left, right, up, down, enter, escape)
/// - MenuItem::pressBtn()
/// editable subitems can be:
/// - floating point or integral values
/// - selectable list items
/// change notifications for commands or item selection is retrived by a registered function:
/// - MenuItem::setNotificationFunction()
/// Implementation see example SimpleMenu1 and SimpleMenue2
/// author: Rolf Kary-Ehlers
/// date  : 05.03.2018
/// e-mail: rolf-kary-ehlers@t-online.de

#include <inttypes.h>
#include <Arduino.h>


class ItemEditBase;
class MenuItem;

union Subfunction
{
    ItemEditBase* mItem;
    MenuItem*     mSubMenu;
};

#define length_of(X) sizeof(X) / sizeof(X[0])

/// @brief notification function pointer type definition
/// @param aDirOrIndex identifies the pressed button or a list index
///        - the pressed button for a command (MenuItem::dir::to)
///        - list index for ItemSelect()
/// @param aID an identifier for
///        - a command
///        - a list, where an entry is selected
///        - a value edited
typedef void (*pFunction)(uint8_t aDirOrIndex, uint8_t aID);

/// @brief fills the string with spaces for given length
/// @param aStr string to fill
/// @param aLen lenght to fill
/// @param behind where to fill
///        - true concat spaces at end
///        - false insert spaces at begin
void fillStringWithSpaces(String& aStr, unsigned int aLen, bool behind=true);

class MenuItem
{
    friend class ItemSelect;
    friend class ItemEditBase;
    template <class Type>
    friend class ItemValue;
public:
    struct dir
    {
      enum to {unknown, left, right, up, down, enter, escape, begin_edit, text };
      static const char* nameof(to);
    };

    enum eFlags : uint8_t
    {
        active_submenu = 0x01,
        edit           = 0x02,
        flagmask       = 0xFC,
        editable       = 0x04,
        command        = 0x08,
        limit_turn     = 0x10,
        float_value    = 0x20,
        no_cursor      = 0x40,
        item_edit      = 0x80
    };

    /// @brief Creates a menue entry containing a submenue
    /// @param aText title of the submenue item
    /// @param aSub pointer to a MenuItem defining a submenu
    /// @param aLength number of submenue items
    MenuItem(const char*aText, MenuItem* aSub, int8_t aLength);

    /// @brief Creates a menue entry containing a editable entry
    /// @param aText title of the submenue item
    /// @param aItemEdit may be a item derived ItemEditBase
    ///        - ItemValue<type> type may be an integral or floating point value
    ///        - ItemSelect a list of values of type (char **)
    /// @param [aID] an identifier passed to notification function:
    ///        - command id
    ///        - list id
    ///        - value id
    /// @param [aLength] number of list items
    /// @note  - is used for ItemSelect array length
    /// @param aFlag flags defining behaviour
    ///        - editable   makes item editable
    ///        - command    defines command item
    ///        - limit_turn turn over min or max value editing values or stepping through lists
    ///        - float_val  floating point value item specifying edit behaviour
    ///        - no_cursor  dont show cursor editing value
    MenuItem(const char*aText, ItemEditBase* aItemEdit=0, uint8_t aID=0, int8_t aLength=0, uint8_t aFlag=0);

    /// @brief retrieves the text of a selected menu item
    /// @param aCursorPos delivers the cursor position, when editig a value
    /// @return text of the menuitem or the value selected
    String getText(int* aCursorPos = 0) const;

    /// @brief retrieves the path from root menu to the sub item or leaf
    /// @return path of menu
    String getPath() const;

    /// @brief presses a button to control the menu item or subitem selection or editing
    /// @param aDir button defining actions navigating through the menu or editing a value
    ///        - left, right: selection of items and navigation
    ///        - up, down: navigation and editing values
    ///        - enter: start editing or enter submenue, enter changed value and stop editing
    ///        - escape: stop editing with no changes, go up from submenu
    /// @note aParent do not use this argument
    ///       - this argument is filled internally
    void   pressBtn(MenuItem::dir::to aDir, MenuItem* aParent=0);

    /// @brief sets a notification function for commands or list item selection
    static void setNotificationFunction(pFunction aNotificationFunction);

private:
    bool isFlagSet(eFlags aFlag) const;
    void setFlag(eFlags aFlag, bool aSet);
    bool isSubmenu();

    const char*   mText;
    Subfunction   mSubFunction;
    int8_t        mLength;
    int8_t        mIndex;
    uint8_t       mFlag;
    static pFunction mNotify;
};

template <class Type>
Type limitValue(Type aValue, Type aMin, Type aMax, bool aTurn)
{
    if (aValue < aMin)
    {
      aValue = aTurn ? aMax : aMin;
    }
    else if (aValue > aMax)
    {
      aValue = aTurn ? aMin : aMax;
    }
    return aValue;
}

class ItemEditBase
{
    friend class MenuItem;

protected:
    ItemEditBase();
    int8_t         getIndex() const;
    int8_t         getLength() const;
    uint8_t        getID() const;
    bool           isFlagSet(MenuItem::eFlags aFlag) const;
    void           notify(uint8_t aDir);

    virtual String editItem(uint8_t aDir) = 0;
    virtual void init(MenuItem* aMenuItem, uint8_t aID);
    MenuItem* mMenuItem;
    uint8_t mID;
};

class ItemSelect : public ItemEditBase
{
public:
    /// @brief Creates an object for a list of text items
    /// @param aItems an array of character strings
    /// @note the lenght of this array is determined by length_of()
    ///       - this is used by aLength argument within constructor MenuItem
    ItemSelect(const char** aItems);

    /// @brief selects list item by index
    /// @param aIndex index of the item
    void select(int aIndex);

    /// @brief selects list item by text
    /// @param aName name of the item to be selected
    void select(const char* aName);

protected:
    String editItem(uint8_t aDir);
    void init(MenuItem* aMenuItem, uint8_t aID);

    const char** mItems;
};


template <class Type>
class ItemValue : public ItemEditBase
{
public:
    /// @brief Creates an edit object for an editable variable
    /// @param a Variable of integral or floating point type
    /// @param aMin minimum value
    /// @param aMax maximum value
    ItemValue(Type& aValue, Type aMin, Type aMax) : mValue(aValue), mMin(aMin), mMax(aMax)
    {  }
protected:
    Type   getIncrement()
    {
        Type fIncrement = 1;
        int8_t fIndex = mMenuItem->mIndex;
        if (fIndex > 0)
        {
            while (fIndex--) fIncrement *= 10;
        }
        else
        {
            while (fIndex++) fIncrement /= 10;
        }

        return fIncrement;
    }

    String editItem(uint8_t aDir)
    {
        static Type fOldValue=0;
        switch (aDir)
        {
        case MenuItem::dir::begin_edit: fOldValue = mValue;       break;
        case MenuItem::dir::escape    : mValue    = fOldValue;    break;
        case MenuItem::dir::enter     : notify(aDir);             break;
        case MenuItem::dir::up        : mValue += getIncrement(); break;
        case MenuItem::dir::down      : mValue -= getIncrement(); break;
        case MenuItem::dir::left      : --mMenuItem->mIndex;      break;
        case MenuItem::dir::right     : ++mMenuItem->mIndex;      break;
        case MenuItem::dir::text      :
        {
            String fString (mValue);
            fillStringWithSpaces(fString, mMenuItem->mLength, false);
            return fString;
        }   break;
        }
        mMenuItem->mIndex = limitValue(getIndex(),
                                       static_cast<int8_t>(isFlagSet(MenuItem::float_value) ? -2 : 0),
                                       static_cast<int8_t>(getLength()-1), false);
        mValue = limitValue(mValue, mMin, mMax, mMenuItem->isFlagSet(MenuItem::limit_turn));

        return String("");
    }

    Type& mValue;
    Type  mMin;
    Type  mMax;
};


#endif // MENU_H
