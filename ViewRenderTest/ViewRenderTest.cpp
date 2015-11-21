// ViewRenderTest.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "ViewRenderer.h"
#include <conio.h>
#include <csignal>

void PromptTest() {
    std::string line;
    int ch;
    do {
        ch = _getch();
        if (ch == 0x03) raise(SIGINT);  // recover the ctrl-c function.
        if (ch == '\r')  ViewRenderer::instance()->ClearUserInput(); 
        else             ViewRenderer::instance()->ReceiveUserInput(ch);
        ViewRenderer::instance()->RenderPrompt();
    } while (true);

    
}

int main(int argc, char* argv[])
{
    PromptTest();
	return 0;
}

