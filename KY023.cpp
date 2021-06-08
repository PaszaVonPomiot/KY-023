// KY023.cpp
// #include "Joy.h"

/*
Joy class for KY-023 joystick connected to Arduino.
- finds stable reading of joy's neutral position (fixes issue with reading wobbling when joy is not being touched)
- projects unequal axes to ideal axes (fixes issue with with reading not showing center when joy is in neutral position)
- outputs 0-255 value to fit in byte (for easier transmission via eg. nRF24L01+) instead of joy's actual 0-1023
- allows reversing axis output (0..255 -> 255..0)

Usage:
1. Create Joy instance - Joy(byte x_pin, byte y_pin, byte button_pin, byte deadzone_radius) - eg. Joy joy(A0, A1, 7, 3);
2. Run joy.setup() within Arduino main setup() section
3. Run joy.readJoyAxisAsByte(X_AXIS) within Arduino main loop() section to read selected axis
4. When Arduino starts leave joy in neutral position for 1s so it can find stable neutral read
5. If needed reverse axis using eg. joy.reverse(joy.readJoyAxisAsByte(X_AXIS)) 
*/
class Joy {
  public:
    // constexpr indicates that the value, or return value, is constant and, where possible, is computed at compile time
    // static indicates that variable is stored once across all class instances
    const byte X_PIN;
    const byte Y_PIN;
    const byte BUTTON_PIN;
    const byte DEADZONE_RADIUS;
    static constexpr byte X_AXIS = 0;  // alias for x axis
    static constexpr byte Y_AXIS = 1;  // alias for y axis
    static constexpr word XY_MAX_IN = 1023;  // max joystick output
    static constexpr word XY_MAX_OUT = 255;  // max casted output eg. for byte
    word x_center;  // x read when joy in neutral position
    word y_center;  // y read when joy in neutral position

    

    // constructor - initialize data members - Classname(int parameter) : membername(parameter) { }
    Joy(byte x_pin, byte y_pin, byte button_pin, byte deadzone_radius) : X_PIN(x_pin), Y_PIN(y_pin), BUTTON_PIN(button_pin), DEADZONE_RADIUS(deadzone_radius) {
    }
    
    // Setup pins and assign some key positions from joystick
    void setup() {
      pinMode(X_PIN, INPUT);
      pinMode(Y_PIN, INPUT);
      pinMode(BUTTON_PIN, INPUT_PULLUP);
      x_center = getAxisCenter(X_PIN);
      y_center = getAxisCenter(Y_PIN);
    }

    // Find stable neutral/center read on given axis pin
    word getAxisCenter(byte axis_pin) {
      int lastRead = 0;
      byte readCount = 0;
      const byte sureCount = 10;  // how many similar reads give assurance of stable neutral position
      while (readCount<sureCount) 
      {
        lastRead = analogRead(axis_pin);
        if (analogRead(axis_pin) == lastRead)
          readCount++;
        else
          readCount = 0;
      }
      return lastRead;
    }

    // Make a read based on which axis was selected
    byte readJoyAxisAsByte(byte axis) {
      if(axis == X_AXIS) {
        return castToByte(readJoyAxis(X_PIN, x_lower_edge(), x_upper_edge(), XY_MAX_IN));
      } 
      if(axis == Y_AXIS) {
        return castToByte(readJoyAxis(Y_PIN, y_lower_edge(), y_upper_edge(), XY_MAX_IN));
      }
      return 0;  // dummy
    }
    
    // Return reversed axis read (0..255 -> 255..0)
    byte reverse(byte value_to_reverse) {
      // return (XY_MAX_OUT-value_to_reverse);
      return map(value_to_reverse, 0, XY_MAX_OUT, XY_MAX_OUT, 0);
    }


  private:
    
    word x_lower_edge() {
      return x_center-DEADZONE_RADIUS;
    }
    word x_upper_edge() {
      return x_center+DEADZONE_RADIUS;
    }
    word y_lower_edge() {
      return y_center-DEADZONE_RADIUS;
    }
    word y_upper_edge() {
      return y_center+DEADZONE_RADIUS;
    }
    
    // cast 1023->255 to fit in byte
    static byte castToByte(int axis_read)
    {
      // return axis_read/4;
      return map(axis_read, 0, XY_MAX_IN, 0, XY_MAX_OUT);
    }
	
    // return joy axis position accounting for real center and deadzone
    static word readJoyAxis(byte axis_pin, word lower_edge, word upper_edge, word axis_max) {
      word pos;  // position read from joystick
      float pos_norm;  // normalized position
      
      pos = analogRead(axis_pin);
      if (pos < lower_edge) 
      {
        // Projection of real axis to ideal axis - lower part
        pos_norm = static_cast<float>(pos)/(lower_edge);
        pos = pos_norm * (axis_max/2);
      }
      else if (pos > upper_edge) 
      {
        // Projection of real axis to ideal axis - upper part
        pos_norm = static_cast<float>(pos-upper_edge)/(axis_max-upper_edge);
        pos = axis_max/2 + pos_norm*(axis_max/2);
      }
      else
      {
        pos = axis_max/2;
      }
      return pos;
    }
};
