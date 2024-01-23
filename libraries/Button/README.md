## Button Library for Arduino 

The Button library simplifies button implementation.
- variable debounce time
- optional variable delay for repeat function
- optional variable repeatition time
- single or multiple buttons
- asynchroneous request via method gButton.tick(millis()); called within loop() function
- callback function for button notifications
  - pressed Signal when button was pressed (signaled after debounce time)
  - delayed  Signal when button is hold longer than delay time
  - repeated Signal when button is hold longer than delay time + n * repetition time
  - released Signal when button is released

### License 

see File: [LICENSE](../../LICENSE.md)
