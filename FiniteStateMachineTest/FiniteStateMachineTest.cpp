// FiniteStateMachineTest.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include <iostream>
#include "ConcreteFiniteStateMachine.h"

int main(int argc, char* argv[])
{
    ConcreteFiniteStateMachine fsm(kEntry);

    for (;;) {
        fsm.run(kTimeout);
        std::cin.get();
    }

    /*setup();
    enum state_codes cur_state = ENTRY_STATE;
    enum ret_codes rc;
    std::function<int(void)> state_fun;

    for (;;) {
        std::cin.get();
        state_fun = state_map[cur_state];
        rc = ret_codes(state_fun());
        if (EXIT_STATE == cur_state) break;
        cur_state = state_transitions.lookup_transition(cur_state, rc);
    }*/

	return 0;
}

