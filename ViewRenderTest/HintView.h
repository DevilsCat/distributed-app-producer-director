#pragma once
#include "View.h"

class HintView : public View {
public:
    static const char* kHintDefault;

    virtual void Draw(const short& width) const override {
        DrawTitle_(width);
        DrawHint_(width);
    }

    static HintView* MakeView(const std::string& title) {
        return new HintView(title);
    }

    std::string hint() const {
        std::lock_guard<std::mutex> lk(m_);
        return hint_;
    }

    void set_hint(const std::string& hint) {
        std::lock_guard<std::mutex> lk(m_);
        hint_ = hint;
    }

private:
    explicit HintView(const std::string& title) :
        View(title) 
    {}

    void DrawHint_(const short& width) const {
        std::cout << utils::windows::center(hint_, width, ' ');
    }

    std::string hint_;
    mutable std::mutex m_;
};

const char* HintView::kHintDefault = "";