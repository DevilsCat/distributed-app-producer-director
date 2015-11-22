// ViewRenderTest.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "ViewRenderer.h"
#include <conio.h>
#include <csignal>
#include "TableView.h"
#include <thread>
#include "PromptView.h"
#include "HintView.h"

#define KEY_UP      72
#define KEY_DOWN    80
#define KEY_LEFT    75
#define KEY_RIGHT   77
#define KEY_ARROW_PREFIX  224

void PromptTest() {
    std::string line;
    int ch;
    PromptView* prompt_view = ViewRenderer::instance()->GetPrompt();
    HintView* hint_view = dynamic_cast<HintView*>(ViewRenderer::instance()->GetView("Hint"));
    do {
        ch = _getch();
        if (ch == 0x03) raise(SIGINT);  // recover the ctrl-c function.
        else if (ch == KEY_ARROW_PREFIX) {}  // ignore arrow prefix. 
        else if (ch == KEY_UP) { hint_view->set_hint("TODO: paging up"); }   // so far no-ops
        else if (ch == KEY_DOWN) { hint_view->set_hint("TODO: paging down"); } // so far no-ops
        else if (ch == KEY_LEFT) { hint_view->set_hint("TODO: switching tag prev"); } // ViewRenderer::instance()->PrevView();
        else if (ch == KEY_RIGHT) { hint_view->set_hint("TODO: switching tag next"); } // ViewRenderer::instance()->NextView();
        else if (ch == '\r') {
            prompt_view->ClearUserInput();
            hint_view->set_hint(HintView::kHintDefault);
        }
        else prompt_view->ReceiveUserInput(ch);
        ViewRenderer::instance()->Render(hint_view);
        ViewRenderer::instance()->RenderPrompt();
        
    } while (true);
}

void TableViewTest() {
    TableView<PlayTableViewCell>* view = TableView<PlayTableViewCell>::MakeView("Play List");
    view->AddCell(std::make_shared<PlayTableViewCell>("director 1", "hamlet", false));
    view->AddCell(std::make_shared<PlayTableViewCell>("director 2", "Thunderstorm", false));
    view->AddCell(std::make_shared<PlayTableViewCell>("director 2", "macbeth", true));
    view->Draw(60);

    view->DeleteCellAt(1);
    view->Draw(60);

    delete view;
}

void RenderTableViewTest() {
    ViewRenderer::instance()->AddView("Play List", TableView<PlayTableViewCell>::MakeView("Play List"), 0.6);
    ViewRenderer::instance()->AddView("Person Info", TableView<PersonTableViewCell>::MakeView("Person Information Table"), 0.3);
    ViewRenderer::instance()->AddView("Hint", HintView::MakeView("Hint"), 0.1);

    std::vector<std::shared_ptr<PlayTableViewCell>> play_cells;
    play_cells.push_back(std::make_shared<PlayTableViewCell>("director 1", "hamlet", false));
    play_cells.push_back(std::make_shared<PlayTableViewCell>("director 2", "Thunderstorm", false));
    play_cells.push_back(std::make_shared<PlayTableViewCell>("director 2", "macbeth", true));

    for (std::shared_ptr<PlayTableViewCell> cell : play_cells) {
        dynamic_cast<TableView<PlayTableViewCell>*>(ViewRenderer::instance()->GetView("Play List"))
            ->AddCell(cell);
    }
    
    std::vector<std::shared_ptr<PersonTableViewCell>> person_cells;
    person_cells.push_back(std::make_shared<PersonTableViewCell>("Yu Xiao", 24, "735 Interdrive", "(314)309-7890"));
    person_cells.push_back(std::make_shared<PersonTableViewCell>("Anqi Zhang", 22, "6591 Rosberry", "(314)537-0018"));

    for (std::shared_ptr<PersonTableViewCell> cell : person_cells) {
        dynamic_cast<TableView<PersonTableViewCell>*>(ViewRenderer::instance()->GetView("Person Info"))
            ->AddCell(cell);
    }

    // Test delete entry from table view.
    ViewRenderer::instance()->Render();
    // delet "Thunderstorm"
    dynamic_cast<TableView<PlayTableViewCell>*>(ViewRenderer::instance()->GetView("Play List"))->DeleteCell(play_cells[1]);
    play_cells.erase(play_cells.begin() + 1);  // also erase from vector.
    
    // Test modify cell in table view.
    play_cells[0]->set_status(true);

    std::thread input_thread([]{
        PromptTest();
    });

    std::thread output_thread([]{
        while (true) {
            std::this_thread::sleep_for(std::chrono::seconds(1));
            ViewRenderer::instance()->Render();
        }
    });

    input_thread.join();
    output_thread.join();
}

int main(int argc, char* argv[])
{
    RenderTableViewTest();
    //PromptTest();
    //TableViewTest();
	return 0;
}

