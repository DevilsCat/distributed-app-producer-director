#pragma once
#include <mutex>
#include <iostream>
#include <vector>
#include <memory>
#include <map>
#include "Views.h"

// Instead of using ACE_DEBUG, use this to make sure
// output is UI clean.
#ifndef PROGRAM_DEBUG
#define PROGRAM_DEBUG(X, ...) \
    do { \
        HintView* view = ViewRenderer::instance()->hint_view(); \
        std::string msg = utils::windows::StringPrintf(X, __VA_ARGS__); \
        view->set_hint(msg); \
        TableView<DebugTableViewCell>* debug_view = dynamic_cast<TableView<DebugTableViewCell>*>(ViewRenderer::instance()->GetView("Debug")); \
        if (debug_view) { \
            debug_view->AddCell(std::make_shared<DebugTableViewCell>(DebugTableViewCell(msg))); \
            ViewRenderer::instance()->Render("Debug"); \
        }\
        ViewRenderer::instance()->RenderHintView(); \
    } while(0)
#endif

#define PROMPT_OFFSET    (1)
#define HINT_OFFSET      (PROMPT_OFFSET + 2)

class ViewRenderer {
    typedef short HeightType;
    typedef short WidthType;

    struct ViewInfo {
        std::shared_ptr<View> view;
        double weight;
        short start_height;
        short nline_max;
        ViewInfo() : ViewInfo(nullptr, 0) {}
        ViewInfo(View* view_, double weight_) : 
            view(view_), weight(weight_),
            start_height(0), nline_max(0)
        {}
    };

public:
    static ViewRenderer* instance();

    // Stores table views
    void AddView(const std::string& name, View* view, double weight);
    View* GetView(const std::string& name);
    HintView* hint_view() const;
    PromptView* prompt_view() const;
    

    void Render();
    void Render(const std::string& view_name);
    void Render(View* view);
    void RenderHintView();
    void RenderPromptView();
    
    void NextView();
    void PrevView();

private:
    ViewRenderer();
    ViewRenderer(const ViewRenderer&);
    ViewRenderer& operator=(const ViewRenderer&);

    // Core render methods
    void RenderAll_();
    void RenderCurrView_();
    void RenderView_(const ViewInfo& vi, bool cursor_back = false) const;
    void RenderHintView_() const;
    void RenderPromptView_() const;

    // Update window_height_ and window_width_.
    int UpdateWindowSize_();

    // "Goto" function, for specific position on the window.
    void GoToHintPos() const ;
    void GoToPromptPos(WidthType x = 0) const;

    // Retrieves the current view
    ViewInfo & GetCurrentViewInfo_();

    static ViewRenderer* renderer_;
    static std::once_flag once_flag_;

    std::ostream& std_out_;

    // prompt view class
    std::shared_ptr<PromptView> prompt_view_;
    // hint view class
    std::shared_ptr<HintView> hint_view_;

    // Stores latest window width and height.
    HeightType window_height_;
    WidthType window_width_;

    // mutual excludes the rendering from input and output thread
    std::mutex render_mut_;
    
    // view update
    std::map<std::string, ViewInfo> view_info_map_;
    std::vector<std::string> view_names_;
    
    // view switching.
    size_t curr_view_idx_;
};
