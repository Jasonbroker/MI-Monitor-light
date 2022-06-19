
////////////////////////////////////
//   DEVICE-SPECIFIC LED SERVICES //
////////////////////////////////////

#include "extras/PwmPin.h"                          // NEW! Include this HomeSpan "extra" to create LED-compatible PWM signals on one or more pins

// Here's the new code defining DEV_DimmableLED - changes from above are noted in the comments

struct DEV_DimmableLED : Service::LightBulb {       // Dimmable LED

  SpanCharacteristic *power;                        // reference to the On Characteristic
  SpanCharacteristic *level;                        // NEW! Create a reference to the Brightness Characteristic instantiated below
  SpanCharacteristic *temp;                         // temperature
  LedPin *coldPIN;                                   // NEW! Create reference to LED Pin instantiated below
  LedPin *warmPIN;                                   // NEW! Create reference to LED Pin instantiated below
  int pwrPIN;                                   // NEW! Create reference to LED Pin instantiated below
  
  DEV_DimmableLED(int pwrpin, int coldpin, int warmpin) : Service::LightBulb(){       // constructor() method

    power=new Characteristic::On();     
                
    level=new Characteristic::Brightness(10);       // NEW! Instantiate the Brightness Characteristic with an initial value of 50% (same as we did in Example 4)
    level->setRange(5,100,1);                       // NEW! This sets the range of the Brightness to be from a min of 5%, to a max of 100%, in steps of 1% (different from Example 4 values)

    temp = new Characteristic::ColorTemperature(140);
    temp->setRange(140, 500, 5);
    this->coldPIN=new LedPin(coldpin, 10000);                   // NEW! Configures a PWM LED for output to the specified pin.  Note pinMode() does NOT need to be called in advance
    this->warmPIN=new LedPin(warmpin, 10000);                   // NEW! Configures a PWM LED for output to the specified pin.  Note pinMode() does NOT need to be called in advance

    pinMode(pwrpin, OUTPUT);
    this->pwrPIN = pwrpin;
    
  } // end constructor

  boolean update(){                              // update() method

    // Here we set the brightness of the LED by calling ledPin->set(brightness), where brightness=0-100.
    // Note HomeKit sets the on/off status of a LightBulb separately from its brightness, which means HomeKit
    // can request a LightBulb be turned off, but still retains the brightness level so that it does not need
    // to be resent once the LightBulb is turned back on.
    
    // Multiplying the newValue of the On Characteristic ("power", which is a boolean) with the newValue of the
    // Brightness Characteristic ("level", which is an integer) is a short-hand way of creating the logic to
    // set the LED level to zero when the LightBulb is off, or to the current brightness level when it is on.
    if (power->getNewVal())
    {
        uint16_t temperature_val = temp->getNewVal(); // 140 - 500
        uint16_t brightness_val = level->getNewVal(); // 5 - 100
        digitalWrite(pwrPIN, HIGH);
        // uint8_t cold_val = (temperature_val - 140) / 360.f * 100 * brightness_val / 100;
        // uint8_t warm_val = (500 - temperature_val) / 360.f * 100 * brightness_val / 100;
        uint8_t cold_val = (500 - temperature_val) / 360.f * brightness_val;
        uint8_t warm_val = (temperature_val - 140) / 360.f * brightness_val;
        coldPIN->set(cold_val);
        warmPIN->set(warm_val);
    } else {
        digitalWrite(pwrPIN, LOW);
    }
    Serial.printf("value changed on %d, level %d, temp %d\n",power->getNewVal(), level->getNewVal(), temp->getNewVal());

   
    return(true);                               // return true
  
  } // update
};
      
//////////////////////////////////
