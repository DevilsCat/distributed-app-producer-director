#pragma once
#include <mutex>
#include <iostream>

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

    // Update prompt.
    void RenderPrompt();

private:
    ViewRenderer();
    ViewRenderer(const ViewRenderer&);
    ViewRenderer& operator=(const ViewRenderer&);

    // Update window_height_ and window_width_.
    int UpdateWindowSize();

    // "Goto" function, for specific position on the window.
    static void GoToXY(WidthType x, HeightType y);
    void GoToPromptPos(WidthType x = 0);

    // Printing helper function
    static std::string left(const std::string s, const int w, const char delimiter);
    static std::string center(const std::string& s, const int& w, const char delimiter);
    static std::string truncate(const std::string& s, size_t w, bool show_ellipsis = true);

    static ViewRenderer* renderer_;
    static std::once_flag once_flag_;

    std::ostream& std_out_;

    // Stores latest window  width and height.
    HeightType window_height_;
    WidthType window_width_;

    // user prompt update
    Position cursor_pos_;
    HeightType prompt_height_; 
    std::string user_buf_;
    std::mutex user_mut_;
};
