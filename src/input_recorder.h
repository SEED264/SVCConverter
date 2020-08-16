#ifndef _SVCCONVERTER_SRC_INPUT_RECORDER_H_
#define _SVCCONVERTER_SRC_INPUT_RECORDER_H_

#include <wx/wxprec.h>
#ifndef WX_PRECOMP
#include <wx/wx.h>
#endif // WX_PRECOMP
#include "input_manager.h"

class ButtonRecorder {
public:
    void SetButtonStates(USHORT usage_page, std::unordered_map<USHORT, bool> states);
    void SetButtonStates(std::unordered_map<USHORT, std::unordered_map<USHORT, bool>> states);

    auto GetTriggeredStates() { return triggered_buttons_; }
    auto GetPressingStates() { return pressing_buttons_; }
    auto GetReleasedStates() { return released_buttons_; }

private:
    std::unordered_map<USHORT, std::unordered_map<USHORT, bool>> triggered_buttons_;
    std::unordered_map<USHORT, std::unordered_map<USHORT, bool>> pressing_buttons_;
    std::unordered_map<USHORT, std::unordered_map<USHORT, bool>> released_buttons_;
};

class KnobRecorder {
public:
    void SetKnobState(const HIDUsagePair &usage, const RawInputValue &state);
    void SetKnobStates(const std::map<HIDUsagePair, RawInputValue> &states);

    auto GetCurrentStates() { return current_states_; }
    auto GetDifference() { return difference_; }

private:
    std::map<HIDUsagePair, RawInputValue> current_states_;
    std::map<HIDUsagePair, unsigned long> difference_;
};

#endif // _SVCCONVERTER_SRC_INPUT_RECORDER_H_