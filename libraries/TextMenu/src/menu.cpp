#include <menu.h>

#include <Arduino.h>

pFunction MenuItem::mNotify = 0;

const char* MenuItem::dir::nameof(MenuItem::dir::to aDir)
{
    switch (aDir)
    {
    case dir::enter:      return "enter";
    case dir::begin_edit: return "begin_edit";
    case dir::down:       return "down";
    case dir::escape:     return "escape";
    case dir::left:       return "left";
    case dir::right:      return "right";
    case dir::text:       return "text";
    case dir::unknown:    return "unknown";
    case dir::up:         return "up";
    default:              return "invalid";
    }
}

MenuItem::MenuItem(const char*aText, MenuItem* aSub, int8_t aLength)
: mText(aText)
, mLength(aLength)
, mIndex(0)
, mFlag(0)
{
    mSubFunction.mSubMenu = aSub;
    if (aText == 0) mFlag = active_submenu;
}

MenuItem::MenuItem(const char*aText, ItemEditBase* aEdit, uint8_t aID, int8_t aLength, uint8_t aFlag)
: mText(aText)
, mLength(aLength)
, mIndex(0)
, mFlag(aFlag)
{
    mSubFunction.mItem = aEdit;
    setFlag(item_edit, aEdit != 0);
    if (mSubFunction.mItem)
    {
        setFlag(editable, true);
        mSubFunction.mItem->init(this, aID);
    }
    else
    {
        setFlag(command, true);
        mIndex = aID;
    }
}

String MenuItem::getPath() const
{
    String fString;
    if (isFlagSet(active_submenu))
    {
        if (mText) fString += String(mText) + "/";
        fString += mSubFunction.mSubMenu[mIndex].getPath();
    }
    return fString;
}

String MenuItem::getText(int* aCursorPos) const
{
    if (isFlagSet(active_submenu))
    {
        return mSubFunction.mSubMenu[mIndex].getText(aCursorPos);
    }
    else
    {
        String fText;
        if (isFlagSet(item_edit) && mSubFunction.mItem)
        {
            bool fEdit = isFlagSet(edit);
            fText  = mText;
            if (fEdit) fText += "|";
            fText += mSubFunction.mItem->editItem(MenuItem::dir::text);
            if (fEdit)
            {
                fText += "|";
                if (aCursorPos && !isFlagSet(no_cursor))
                {
                    *aCursorPos = fText.lastIndexOf('.');
                    *aCursorPos = (*aCursorPos == -1) ? fText.length()-2 : *aCursorPos - 1;
                    if (mIndex > 0)
                    {
                        *aCursorPos -= mIndex;
                    }
                    else if (mIndex < 0)
                    {
                        *aCursorPos -= (mIndex-1);
                    }
                }
            }
            return fText;
        }
        return mText;
    }
    return "";
}

void MenuItem::pressBtn(MenuItem::dir::to aDir, MenuItem* aParent)
{
    if (isFlagSet(active_submenu))
    {
        mSubFunction.mSubMenu[mIndex].pressBtn(aDir, this);
    }
    else if (aDir == MenuItem::dir::enter)
    {
        if (isSubmenu() && !isFlagSet(active_submenu))
        {
            setFlag(active_submenu, true);
        }
        else if (isFlagSet(item_edit))
        {
            if (isFlagSet(edit))
            {
                mSubFunction.mItem->editItem(MenuItem::dir::enter);
                setFlag(edit, false);
            }
            else if (isFlagSet(editable) && !isFlagSet(edit))
            {
                mSubFunction.mItem->editItem(MenuItem::dir::begin_edit);
                setFlag(edit, true);
            }
        }
        else if (mNotify)
        {
            mNotify(aDir, mIndex);
        }
    }
    else if (isFlagSet(edit))
    {
        mSubFunction.mItem->editItem(aDir);
        if (aDir == MenuItem::dir::escape)
        {
            setFlag(edit, false);
        }
    }
    else if (aParent)
    {
        switch (aDir)
        {
        case MenuItem::dir::left:
            ++aParent->mIndex;
            if (aParent->mIndex == aParent->mLength) aParent->mIndex = 0;
            break;
        case MenuItem::dir::right:
            if (aParent->mIndex == 0) aParent->mIndex = aParent->mLength - 1;
            else --aParent->mIndex;
            break;
        case MenuItem::dir::up:
        case MenuItem::dir::escape:
            if (aParent->isFlagSet(active_submenu) && aParent->mText)
            {
                aParent->setFlag(active_submenu, false);
            }
            break;
        case MenuItem::dir::down:
            if (isSubmenu() && !isFlagSet(active_submenu))
            {
                setFlag(active_submenu, true);
            }
            break;
        default:
            break;
        }
    }
}

bool MenuItem::isFlagSet(eFlags aFlag) const
{
    return (mFlag & aFlag) != 0;
}

bool MenuItem::isSubmenu()
{
    return !isFlagSet(item_edit) && mSubFunction.mSubMenu;
}

void MenuItem::setFlag(eFlags aFlag, bool aSet)
{
    if (aSet)
    {
        mFlag |= aFlag;
    }
    else
    {
        mFlag &= ~aFlag;
    }
}

void MenuItem::setNotificationFunction(pFunction aNotificationFunction)
{
    mNotify = aNotificationFunction;
}


ItemEditBase::ItemEditBase() : mMenuItem(0)
{
}

void ItemEditBase::init(MenuItem *aMenuItem, uint8_t aID)
{
    mMenuItem = aMenuItem;
    mID = aID;
}

int8_t ItemEditBase::getIndex() const
{
    if (mMenuItem)
    {
        return mMenuItem->mIndex;
    }
    return 0;
}

int8_t ItemEditBase::getLength() const
{
    if (mMenuItem)
    {
        return mMenuItem->mLength;
    }
    return 0;
}

uint8_t ItemEditBase::getID() const
{
    return mID;
}

bool ItemEditBase::isFlagSet(MenuItem::eFlags aFlag) const
{
    if (mMenuItem)
    {
        return mMenuItem->isFlagSet(aFlag);
    }
    return false;
}

void ItemEditBase::notify(uint8_t aDir)
{
    if (MenuItem::mNotify)
    {
        MenuItem::mNotify(aDir, mID);
    }
}

ItemSelect::ItemSelect(const char **aItems)
    : mItems(aItems)
{

}

void ItemSelect::select(int aIndex)
{
    if (mMenuItem)
    {
        mMenuItem->mIndex = aIndex;
    }
}

void ItemSelect::select(const char* aName)
{
    if (mItems && mMenuItem)
    {
        for (int i=0; i<getLength(); ++i)
        {
            if (strcmp(mItems[i], aName) == 0)
            {
                mMenuItem->mIndex = i;
                break;
            }
        }
    }
}

String ItemSelect::editItem(uint8_t aDir)
{
    static uint8_t fOldIndex=0;
    switch (aDir)
    {
      case MenuItem::dir::begin_edit: fOldIndex = mMenuItem->mIndex; break;
      case MenuItem::dir::escape:     mMenuItem->mIndex = fOldIndex; break;
      case MenuItem::dir::enter:      notify(getIndex()); break;
      case MenuItem::dir::up:         ++mMenuItem->mIndex; break;
      case MenuItem::dir::down:       --mMenuItem->mIndex; break;
    }
    mMenuItem->mIndex = limitValue(getIndex(), static_cast<int8_t>(0), static_cast<int8_t>(getLength()-1), mMenuItem->isFlagSet(MenuItem::limit_turn));

    return mItems[mMenuItem->mIndex];
}

void ItemSelect::init(MenuItem *aMenuItem, uint8_t aID)
{
    ItemEditBase::init(aMenuItem, aID);
    if (mItems && getLength() == 0)
    {
        for (mMenuItem->mLength=0; mItems[mMenuItem->mLength] != 0; ++mMenuItem->mLength);
    }
}

void fillStringWithSpaces(String& aStr, unsigned int aLen, bool behind)
{
    while (aStr.length() < aLen)
    {
        if (behind) aStr += " ";
        else        aStr = " " + aStr;
    }
}
