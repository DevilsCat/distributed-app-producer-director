#pragma once
#include <mutex>
#include <iostream>
#include <vector>
#include "TableView.h"
#include <memory>
#include <map>

#define PROMPT_OFFSET    1

#define PROMPT_MARK      ">>"

class ViewRenderer {
    struct Position {
        short x;
        short y;
        Position(const short& x_, const short& y_) : x(x_), y(y_) {}
    };
    typedef short HeightType;
    typedef short WidthType;

public:
    static ViewRenderer* instance();

    // Stores the user inputs for echo them back.
    void ReceiveUserInput(const char& ch);
    void ClearUserInput();

    // Stores table views
    void AddView(const std::string& name, View* view);
    View* GetView(const std::string& name);

    void Render();
    
    // Update prompt.
    void RenderPrompt();

    void NextView();
    void PrevView();

private:
    ViewRenderer();
    ViewRenderer(const ViewRenderer&);
    ViewRenderer& operator=(const ViewRenderer&);

    // Core render methods
    void RenderAll_();
    void RenderView_();
    void RenderPrompt_();

    // Update window_height_ and window_width_.
    int UpdateWindowSize();

    // "Goto" function, for specific position on the window.
    static void GoToXY(WidthType x, HeightType y);
    void GoToPromptPos(WidthType x = 0);

    static ViewRenderer* renderer_;
    static std::once_flag once_flag_;

    std::ostream& std_out_;

    // Stores latest window width and height.
    HeightType window_height_;
    WidthType window_width_;

    // mutual excludes the rendering from input and output thread
    std::mutex render_mut_;

    // user prompt update
    Position cursor_pos_;
    HeightType prompt_height_; 
    std::string user_buf_;
    std::mutex user_mut_;
    
    // view update
    std::map<std::string, std::unique_ptr<View>> view_map_;
    std::vector<std::string> view_names_;
    size_t curr_view_idx_;
};
