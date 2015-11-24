#ifndef VIEW_RENDER_H
#define VIEW_RENDER_H

#include <mutex>
#include <vector>
#include <memory>
#include <map>
#include "Views.h"

// Instead of using ACE_DEBUG, use this { PROGRAM_DEBUG } to make sure
// output is UI clean.
#ifndef PROGRAM_DEBUG
#define PROGRAM_DEBUG(X, ...) \
    do { \
        HintView* view = ViewRenderer::instance()->hint_view(); \
        std::string msg = utils::windows::StringPrintf(X, __VA_ARGS__); \
        view->set_hint(msg); \
        TableView<DebugTableViewCell>* debug_view = dynamic_cast<TableView<DebugTableViewCell>*>(ViewRenderer::instance()->GetView("Debug")); \
        if (debug_view) { \
            debug_view->AddCell(std::make_shared<DebugTableViewCell>(msg)); \
            ViewRenderer::instance()->Render("Debug"); \
        }\
        ViewRenderer::instance()->Render(ViewRenderer::sNoMainView); \
    } while(0)
#endif

#define PROMPT_OFFSET               (1)
#define HINT_OFFSET                 (PROMPT_OFFSET + 2)
#define WINDOW_HEIGHT_PRESERVED     (3)

class ViewRenderer {
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
    ~ViewRenderer();
    // Default string for view rendering.
    static std::string sNoMainView;
    static std::string sAllViews;
    static ViewRenderer* instance();

    // Views' Getter/Setter.
    void AddView(const std::string& name, View* view, double weight = 0);
    View* GetView(const std::string& name);
    HintView* hint_view() const;
    PromptView* prompt_view() const;
    
    // Render method
    void Render(const std::string& view_name);
    void Render(View* view);
    void RenderHintView();
    void RenderPromptView();
    
    // View Switching methods.
    void NextView();
    void PrevView();

    // View Scrolling method.
    void Scroll(bool is_up);

private:
    // Hide constructors and copy operator for singleton patterns.
    ViewRenderer();
    ViewRenderer(const ViewRenderer&);
    ViewRenderer& operator=(const ViewRenderer&);

    // Core render methods
    void RenderAll_(bool render_main_view);
    void RenderCurrView_();
    void RenderView_(const ViewInfo& vi, bool cursor_back) const;
    void RenderHintView_() const;
    void RenderPromptView_() const;

    // Update window_height_ and window_width_.
    int UpdateWindowSize_();

    // Retrieves the current view
    ViewInfo & GetCurrentViewInfo_();

    // Singleton.
    static std::shared_ptr<ViewRenderer> renderer_;
    static std::once_flag once_flag_;

    // prompt view class
    std::shared_ptr<PromptView> prompt_view_;
    // hint view class
    std::shared_ptr<HintView> hint_view_;

    // Stores latest window width and height.
    short window_height_;
    short window_width_;

    // mutual excludes the rendering from input and output thread
    std::mutex render_mut_;
    
    // view update
    std::map<std::string, ViewInfo> view_info_map_;
    std::vector<std::string> view_names_;
    
    // view switching.
    size_t curr_view_idx_;
};

#endif