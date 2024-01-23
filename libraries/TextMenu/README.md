## TextMenu Library for Arduino

The TextMenu library simplifies realization of a text based menu.

### Code example, see examples
<pre>

MenuItem gMainMenu[] =
{
    MenuItem("Item:"  , &gSelectListItem, eListItem, length_of(listitems), MenuItem::no_cursor|MenuItem::limit_turn),
    MenuItem("Long:"  , &gEditLong      , eLongValue, 3),
    MenuItem("Double:", &gEditDouble    , eDoubleValue, 3, MenuItem::float_value|MenuItem::limit_turn),
    MenuItem("Command", (ItemEditBase*)0, eCommand)
};

MenuItem gMenu("SimpleMenu1...", gMainMenu, length_of(gMainMenu));
</pre>

### License 

see File: [LICENSE](../../LICENSE.md)
