// FiniteStateMachineTest.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include <iostream>
#include "ConcreteFiniteStateMachine.h"

int main(int argc, char* argv[])
{
    ConcreteFiniteStateMachine fsm(kEntry);

    for (;;) {
        fsm.run(kStart);
        std::cin.get();
    }

	return 0;
}

