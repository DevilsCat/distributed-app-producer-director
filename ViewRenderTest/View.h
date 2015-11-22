#pragma once
#include <string>
#include "Utils.h"
#include <iostream>

class View {  // interface of any View class
public:
    View(const std::string& title) : title_(title) {}
    virtual ~View() {}

    virtual void Draw(const short& width) const = 0;

protected:
    virtual void DrawTitle_(const short& width) const {
        std::cout << utils::windows::left(title_, width, '=') << std::endl;
    }

private:
    std::string title_;
};
