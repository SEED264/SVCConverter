#ifndef _SVCCONVERTER_SRC_INPUT_RECORDER_H_
#define _SVCCONVERTER_SRC_INPUT_RECORDER_H_

#include "input_manager.h"

// Record button states and triggered, released flags
class ButtonRecorder {
public:
    void SetButtonStates(USHORT usage_page, std::unordered_map<USHORT, bool> states);
    void SetButtonStates(std::unordered_map<USHORT, std::unordered_map<USHORT, bool>> states);

    auto GetTriggeredStates() const { return triggered_buttons_; }
    auto GetPressingStates() const { return pressing_buttons_; }
    auto GetReleasedStates() const { return released_buttons_; }

private:
    std::unordered_map<USHORT, std::unordered_map<USHORT, bool>> triggered_buttons_;
    std::unordered_map<USHORT, std::unordered_map<USHORT, bool>> pressing_buttons_;
    std::unordered_map<USHORT, std::unordered_map<USHORT, bool>> released_buttons_;
};

// Recording knob states and difference
class KnobRecorder {
public:
    void SetKnobState(const HIDUsagePair &usage, const RawInputValue &state);
    void SetKnobStates(const std::map<HIDUsagePair, RawInputValue> &states);

    auto GetCurrentStates() const { return current_states_; }
    auto GetDifference() const { return difference_; }

private:
    std::map<HIDUsagePair, RawInputValue> current_states_;
    std::map<HIDUsagePair, long> difference_;
};

// Record knob states and total difference for register knob binding
class KnobRecorderForRegister {
public:
    void SetKnobState(const HIDUsagePair &usage, const RawInputValue &state);
    void SetKnobStates(const std::map<HIDUsagePair, RawInputValue> &states);

    auto GetCurrentStates() const { return current_states_; }

    auto GetTotalDifference() const { return total_difference_; }
    auto GetTotalDifferenceAsRatio() const { return total_difference_ratio_; }

private:
    std::map<HIDUsagePair, long> total_difference_;
    std::map<HIDUsagePair, double> total_difference_ratio_;
    std::map<HIDUsagePair, RawInputValue> current_states_;
};

class KeyRecorder {
public:
    void SetKeyStates(const std::vector<unsigned char> &states);

    auto GetTriggeredStates() const { return triggered_; }

private:
    std::vector<bool> triggered_;
    std::vector<bool> previous_pressed_;
};

#endif // _SVCCONVERTER_SRC_INPUT_RECORDER_H_