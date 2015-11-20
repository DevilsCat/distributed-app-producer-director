#pragma once
#include "GenericFiniteStateMachine.h"
#include <iostream>

enum StateCode { kEntry, kFoo, kBar, kEnd };
enum InputCode { kTimeout, kStart, kStop, kQuit };
class ConcreteFiniteStateMachine : public GenericFiniteStateMachine<StateCode, InputCode> {
public:
    ConcreteFiniteStateMachine(const StateCode& cur_state)
        : GenericFiniteStateMachine<StateCode, InputCode>(cur_state) 
    {
        add_state_function(kEntry, bind(&ConcreteFiniteStateMachine::entry_state, this, std::placeholders::_1));
        add_state_function(kFoo, bind(&ConcreteFiniteStateMachine::foo_state, this, std::placeholders::_1));
        add_state_function(kBar, bind(&ConcreteFiniteStateMachine::bar_state, this, std::placeholders::_1));
        add_state_function(kEnd, bind(&ConcreteFiniteStateMachine::exit_state, this, std::placeholders::_1));

        add_transition(kEntry, kOk, kFoo);
        add_transition(kEntry, kFail, kEnd);
        add_transition(kFoo, kOk, kBar);
        add_transition(kFoo, kRepeat, kFoo);
        add_transition(kBar, kOk, kEnd);
        add_transition(kBar, kFail, kEnd);
        add_transition(kBar, kRepeat, kFoo);
    }

    RetCode entry_state(InputCode input) {
        std::cout << "entry state:"<< input << " -> ok" << std::endl;
        return kOk;
    }

    RetCode foo_state(InputCode input) {
        std::cout << "foo:" << input << " -> ok" << std::endl;
        return kOk;
    }

    RetCode bar_state(InputCode input) {
        std::cout << "bar:"<< input << " -> repeat" << std::endl;
        return kRepeat;
    }

    RetCode exit_state(InputCode input) {
        std::cout << "end" << std::endl;
        return kOk;
    }
};
