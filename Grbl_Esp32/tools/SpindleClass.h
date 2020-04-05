/*
    SpindleClass.h

    Header file for a Spindle Class

    Part of Grbl_ESP32

    2020 -	Bart Dring This file was modified for use on the ESP32
                    CPU. Do not use this with Grbl for atMega328P

    Grbl is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.
    Grbl is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.
    You should have received a copy of the GNU General Public License
    along with Grbl.  If not, see <http://www.gnu.org/licenses/>.

*/
#include "grbl.h"
#include <driver/dac.h>


#ifndef SPINDLE_CLASS_H
#define SPINDLE_CLASS_H

// This is the base class. Do not use this as your spindle
class Spindle {
  public:
    virtual void init(); // not in constructor because this also gets called when $$ settings change
    virtual float set_rpm(float rpm);
    virtual void set_pwm(uint32_t duty);
    virtual void set_state(uint8_t state, float rpm);
    virtual uint8_t get_state();
    virtual void stop();
    virtual void config_message();
    virtual bool isRateAdjusted();

};

// This is a dummy spindle that has no I/O.
// It is used to ignore spindle commands when no spinde is desired
class NullSpindle : public Spindle {
  public:
    void init();
    float set_rpm(float rpm);
    void set_pwm(uint32_t duty);
    void set_state(uint8_t state, float rpm);
    uint8_t get_state();
    void stop();
    void config_message();
};

// This adds support for PWM
class PWMSpindle : public Spindle {
  public:
    void init();
    float set_rpm(float rpm);
    void set_state(uint8_t state, float rpm);
    uint8_t get_state();
    void stop();
    void config_message();
    virtual void set_pwm(uint32_t duty);

  private:
    int8_t _spindle_pwm_chan_num;
    

    int32_t _current_pwm_duty;

    float _pwm_gradient; // Precalulated value to speed up rpm to PWM conversions.

    uint32_t _pwm_off_value;
    uint32_t _pwm_min_value;
    uint32_t _pwm_max_value;
    float _min_rpm;
    float _max_rpm;

    void set_enable_pin(bool enable_pin);
    void set_spindle_dir_pin(bool Clockwise);

  protected:
    float _pwm_freq;
    uint32_t _pwm_period; // how many counts in 1 period
};

// This is for an on/off spindle all RPMs above 0 are on
class RelaySpindle : public PWMSpindle {
  public:
    void init();
    void config_message();
    void set_pwm(uint32_t duty);
  private:

};

// this is the same as a PWM spindle, but the M4 compensation is supported.
class Laser : public PWMSpindle {
  public:
    bool isRateAdjusted();
    void config_message();
};

// This uses one of the (2) DAC pins on ESP32 to output a voltage
class DacSpindle : public PWMSpindle {
  public:
    void init();
    void config_message();
    void set_pwm(uint32_t duty); // sets DAC instead of PWM
  private:
    dac_channel_t _dac_channel_num;
    bool _gpio_ok;
};


#endif