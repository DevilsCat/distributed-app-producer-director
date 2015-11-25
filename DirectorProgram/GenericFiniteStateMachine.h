#pragma once
#include <vector>
#include <unordered_map>
#include <functional>

template<typename StateCode, typename InputCode>
class GenericFiniteStateMachine {
public:
    virtual ~GenericFiniteStateMachine() {
    }

    enum RetCode { kOk, kFail, kRepeat };

    struct Transition {
        StateCode src_state;
        RetCode   ret_code;
        StateCode dst_state;
        Transition(StateCode src_state, RetCode ret_code, StateCode dst_state) : 
            src_state(src_state), ret_code(ret_code), dst_state(dst_state) 
        {}
    };

    GenericFiniteStateMachine(const StateCode& cur_state) : 
        cur_state(cur_state), rc() 
    {}

    void add_transition(StateCode src, RetCode rc, StateCode dst) {
        transitions.push_back(Transition(src, rc, dst));
    }

    void add_state_function(StateCode code, std::function<RetCode(InputCode)> func) {
        state_map[code] = func;
    }

    StateCode lookup_transition(StateCode src, RetCode rc) {
        for (size_t i = 0; i < transitions.size(); ++i) {
            if (src == transitions[i].src_state && rc == transitions[i].ret_code)
                return transitions[i].dst_state;
        }
        return StateCode();
    }

    void run(InputCode input) {
        state_fun = state_map[cur_state];
        rc = state_fun(input);
        cur_state = lookup_transition(cur_state, rc);
    }

protected:
    // Override this to add finite state machine rules here
    virtual void on_machine_setup() = 0;

    void setup() {
        on_machine_setup();
    }
private:
    

    std::vector<Transition> transitions;
    std::unordered_map<StateCode, std::function<RetCode(InputCode)>> state_map;

    StateCode cur_state;
    RetCode rc;
    std::function<RetCode(InputCode)> state_fun;
};
