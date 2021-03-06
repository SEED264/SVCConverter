#include "input_recorder.h"

void ButtonRecorder::SetButtonStates(USHORT usage_page, std::unordered_map<USHORT, bool> states) {
    for (auto state = states.begin(); state != states.end(); ++state) {
        auto usage_id = state->first;
        auto is_pressing = state->second;
        // Set triggered flag true if the button is pressing now and it wasn't pressed at the last call
        auto was_pressing = pressing_buttons_[usage_page][usage_id];
        triggered_buttons_[usage_page][usage_id] = is_pressing && !was_pressing;
        // Set released flag true if the button isn't pressing now and it was pressed at the last call
        released_buttons_[usage_page][usage_id] = !is_pressing && was_pressing;

        pressing_buttons_[usage_page][usage_id] = is_pressing;
    }
}

void ButtonRecorder::SetButtonStates(std::unordered_map<USHORT, std::unordered_map<USHORT, bool>> states) {
    for (auto istate = states.begin(); istate != states.end(); ++istate) {
        auto usage_page = istate->first;
        auto inner_states = istate->second;
        SetButtonStates(usage_page, inner_states);
    }
}

void KnobRecorder::SetKnobState(const HIDUsagePair &usage, const RawInputValue &state) {
    bool is_first_record = !current_states_.count(usage);
    if (!is_first_record) {
        auto previous_value = current_states_[usage].value;
        long difference = state.value - previous_value;
        auto range = state.max - state.min;
        bool is_increase = state.value > previous_value;
        bool is_reverse = std::abs(difference) > (range / 2);
        if (is_reverse) {
            if (is_increase) {
                difference = (state.min - previous_value) + (state.value - state.max);
            } else {
                difference = (state.max - previous_value) + (state.value - state.min);
            }
        }
        difference_[usage] = difference;
    }
    current_states_[usage] = state;

}

void KnobRecorder::SetKnobStates(const std::map<HIDUsagePair, RawInputValue> &states) {
    for (auto st = states.begin(); st != states.end(); ++st) {
        auto usage = st->first;
        auto state = st->second;
        SetKnobState(usage, state);
    }
}

void KnobRecorderForRegister::SetKnobState(const HIDUsagePair &usage, const RawInputValue &state) {
    bool is_first_record = !current_states_.count(usage);
    if (!is_first_record) {
        auto previous_value = current_states_[usage].value;
        long difference = state.value - previous_value;
        auto range = state.max - state.min;
        bool is_increase = state.value > previous_value;
        bool is_reverse = std::abs(difference) > (range / 2);
        if (is_reverse) {
            if (is_increase) {
                difference = (state.min - previous_value) + (state.value - state.max);
            } else {
                difference = (state.max - previous_value) + (state.value - state.min);
            }
        }
        total_difference_[usage] += difference;
        if (range)
            total_difference_ratio_[usage] += static_cast<double>(difference) / range;
    }
    current_states_[usage] = state;
}

void KnobRecorderForRegister::SetKnobStates(const std::map<HIDUsagePair, RawInputValue> &states) {
    for (auto st = states.begin(); st != states.end(); ++st) {
        auto usage = st->first;
        auto state = st->second;
        SetKnobState(usage, state);
    }
}

void KeyRecorder::SetKeyStates(const std::vector<unsigned char> &states) {
    auto states_size = states.size();
    if (states_size > triggered_.size()) {
        triggered_.resize(states_size);
        previous_pressed_.resize(states_size);
    }
    for (auto i = 0; i < states_size; ++i) {
        bool is_pressed = states[i] & 0x80;
        triggered_[i] = is_pressed && !previous_pressed_[i];
        previous_pressed_[i] = is_pressed;
    }
}