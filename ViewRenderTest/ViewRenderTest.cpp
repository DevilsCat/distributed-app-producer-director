// ViewRenderTest.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "ViewRenderer.h"
#include <conio.h>
#include <csignal>
#include "TableView.h"

void PromptTest() {
    std::string line;
    int ch;
    do {
        ch = _getch();
        if (ch == 0x03) raise(SIGINT);  // recover the ctrl-c function.
        if (ch == '\r') ViewRenderer::instance()->ClearUserInput(); 
        else            ViewRenderer::instance()->ReceiveUserInput(ch);
        ViewRenderer::instance()->RenderPrompt();
    } while (true);
}

void TableViewTest() {
    TableView<PlayTableViewCell> view("Play List");
    view.AddCell(new PlayTableViewCell("director 1", "hamlet", false));
    view.AddCell(new PlayTableViewCell("director 2", "Thunderstorm", false));
    view.AddCell(new PlayTableViewCell("director 2", "macbeth", true));
    view.Draw(60);

    view.DeleteCellAt(1);
    view.Draw(60);
}

int main(int argc, char* argv[])
{
    TableViewTest();
	return 0;
}

