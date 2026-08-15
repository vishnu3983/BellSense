#include "doorbell.h"

namespace esphome {
namespace doorbell {

void DoorbellBinarySensor::setup()
{
    ESP_LOGI("doorbell", "Doorbell component started");
    pressed_ = false;
    publish_state(false);

    set_interval("sample", sample_interval_, [this]() {
        sample();
    });

    set_interval("evaluate", window_, [this]() {
        evaluate();
    });
    
}

void DoorbellBinarySensor::dump_config()
{
    ESP_LOGCONFIG("doorbell", "Doorbell Detector");
    ESP_LOGCONFIG("doorbell", "  Name              : %s", get_name().c_str());
    ESP_LOGCONFIG("doorbell", "  Sample interval   : %u ms", sample_interval_);
    ESP_LOGCONFIG("doorbell", "  Window            : %u ms", window_);
    ESP_LOGCONFIG("doorbell", "  Press threshold   : %.3f", press_threshold_);
    ESP_LOGCONFIG("doorbell", "  Release threshold : %.3f", release_threshold_);
}

void DoorbellBinarySensor::sample()
{
    // ESP8266 ADC returns 0-1023
    // float value = analogRead(A0) / 1023.0f;
    sample_history_[sample_index_] = analogRead(A0) / 1023.0f;

    sample_index_++;

    if (sample_index_ >= 3)
    {
        sample_index_ = 0;
        history_valid_ = true;
    }

    if (!history_valid_)
        return;

    float value = median3(
    sample_history_[0],
    sample_history_[1],
    sample_history_[2]);

    min_ = std::min(min_, value);
    max_ = std::max(max_, value);
}

void DoorbellBinarySensor::evaluate()
{
    float swing = max_ - min_;
    
    // Ignore first second after boot
    if (startup_windows_ > 0)
    {
        startup_windows_--;

        min_ = 1.0f;
        max_ = 0.0f;

        return;
    }

    if (swing > press_threshold_)
    {
        press_counter_++;
        release_counter_ = 0;
    }
    else if (swing < release_threshold_)
    {
        release_counter_++;
        press_counter_ = 0;
    }
    else
    {
        // In hysteresis band
        press_counter_ = 0;
        release_counter_ = 0;
    }

    if (!pressed_ && press_counter_ >= REQUIRED_WINDOWS)
    {
        pressed_ = true;

        publish_state(true);

        ESP_LOGI("doorbell",
                 "Doorbell PRESSED");
    }

    if (pressed_ && release_counter_ >= REQUIRED_WINDOWS)
    {
        pressed_ = false;

        publish_state(false);

        ESP_LOGI("doorbell", "Doorbell RELEASED");
    }

    min_ = 1.0f;
    max_ = 0.0f;
}

}  // namespace doorbell
}  // namespace esphome