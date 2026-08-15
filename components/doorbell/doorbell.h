#pragma once

#include "esphome.h"

namespace esphome {
namespace doorbell {

class DoorbellBinarySensor
    : public Component,
      public binary_sensor::BinarySensor
{
public:
    void setup() override;
    void dump_config() override;
    void set_sample_interval(uint32_t ms)
    {
        sample_interval_ = ms;
    }

    void set_window(uint32_t ms)
    {
        window_ = ms;
    }

    void set_press_threshold(float t)
    {
        press_threshold_ = t;
    }

    void set_release_threshold(float t)
    {
        release_threshold_ = t;
    }
private:
    float median3(float a, float b, float c)
    {
        if (a > b) std::swap(a, b);
        if (b > c) std::swap(b, c);
        if (a > b) std::swap(a, b);

        return b;
    }
    void sample();
    void evaluate();

    uint32_t sample_interval_ = 5;
    uint32_t window_ = 100;

    float press_threshold_ = 0.10f;
    float release_threshold_ = 0.05f;

    float min_ = 1.0f;  
    float max_ = 0.0f;

    bool pressed_ = false;

    // Ignore first second after boot
    uint8_t startup_windows_ = 10;

    // Require 2 consecutive windows
    uint8_t press_counter_ = 0;
    uint8_t release_counter_ = 0;

    static constexpr uint8_t REQUIRED_WINDOWS = 2;

    float sample_history_[3] = {0};

    uint8_t sample_index_ = 0;

    bool history_valid_ = false;
};

}  // namespace doorbell
}  // namespace esphome