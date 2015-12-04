// ViewRenderer.h -- This file declares a ViewRenderer class, used to render the view.
// Created by Yu Xiao, Anqi Zhang, Yuhan Hao, all rights reserved.
//
#ifndef VIEW_RENDER_H
#define VIEW_RENDER_H

#include <mutex>
#include <vector>
#include <memory>
#include <map>
#include "Views.h"

// !IMPORTANT! Instead of using { ACE_DEBUG }, use this { PROGRAM_DEBUG } to make sure
// output is UI clean.
#ifndef PROGRAM_DEBUG
#define PROGRAM_DEBUG(X, ...) \
    do { \
        HintView* view = ViewRenderer::instance()->hint_view(); \
        std::string msg = utils::windows::StringPrintf(X, __VA_ARGS__); \
        view->set_hint(msg); \
        TableView<DebugTableViewCell>* debug_view = ViewRenderer::instance()->GetView<TableView<DebugTableViewCell>>("Debug"); \
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

// Stores and Renders the added {View} class according to current window configuration.
//
// View Render will follow the hierarchy below to render the screen
//   Main View   (Customized by user)
//   Hint View   (Built-in, shows program hint message)
//   Prompt View (Built-in, shows command prompt and user input)
//
// Sample Usage:
//   // Add a {View} class which is a {TableView} with cell type {MyViewCell}
//   ViewRenderer::instance()->AddView("MyView", TableView<MyViewCell>::MakeView("My View Title"));
//   //...Add cell to the table view somewhere
//   ViewRenderer::instance()->Render("MyView");
class ViewRenderer {
    // Stores information for the views in renderer
    struct ViewInfo {
        // Stores the reference to {View} object.
        std::shared_ptr<View> view;
        // @Deprecated -- Stores the weight when multiple views are visible in current screen.
        double weight;
        // @Deprecated -- Stores the start point when multiple views are visible in current screen.
        short start_height;
        // Stores the maximum numbers of line this view are allowed to use.
        short nline_max;

        // Default constructor sets all member functions to zero.
        ViewInfo();
        // Constructor passes in the {View} object and weight.
        ViewInfo(View* view_, double weight_);
    };

public:
    // Default strings for view rendering.
    // Main view will not be rendered if Render() function passes in this string.
    static std::string sNoMainView;
    // All views will be rendered if Render() function passes in this string.
    static std::string sAllViews;

    // Returns the singleton of this class.
    static ViewRenderer* instance();

    // Adds a view to renderer, passes a string as name to uniquely determine the view
    // for later retrieving.
    //
    // {weight} parameter is deprecated.
    void AddView(const std::string& name, View* view, double weight = 0);

    // Retrieves a view from this renderer, needs to provide {View} object template
    // so that this function will dynamically cast to that type.
    // This function implements "Strategy Pattern",
    template<class ConcreteView>
    ConcreteView* GetView(const std::string& name) {
        return dynamic_cast<ConcreteView *>(GetView_(name));
    }

    // Returns a reference to built-in {HintView}.
    HintView* hint_view() const;

    // Returns a reference to built-in {PromptView} object
    PromptView* prompt_view() const;
    
    // Renders a view in this renderer given a name, if the present view is not
    // the given name, it is equivalent to Render(sNoMainView).
    void Render(const std::string& view_name);

    // Renders a veiw in this renderer given a {View} object, if the present view
    // is not the given {View}, it is equivatant to Render(sNoMainView).
    void Render(View* view);

    // Renders built-in {HintView} only.
    // This function implements "Thread Safe Interface Pattern".
    void RenderHintView();

    // Renders built-in {PromptView} only.
    // This function implements "Thread Safe Interface Pattern".
    void RenderPromptView();
    
    // Navigates to next view (if has) and renders the next view.
    void NextView();

    // Navigates to previous view (if has) and renders the previous view.
    void PrevView();

    // Advices the present view to scroll a full screen of contents.
    void Scroll(bool is_up);

private:
    // Hide constructors and copy operator for singleton patterns.
    // @Update -- Instead using = delete key word, which is not surpported
    // in VS2012.
    ViewRenderer();
    ViewRenderer(const ViewRenderer&);
    ViewRenderer& operator=(const ViewRenderer&);

    // Return a raw {View} reference given the view name, which should be previously
    // defined by user.
    View* GetView_(const std::string& name);

    // Renders the entire screen if true is passed in, otherwise only render
    // {HintView} and {PromptView}.
    void RenderAll_(bool render_main_view);

    // Renders the current main view.
    void RenderCurrView_();

    // Renders the view given the {ViewInfo}, cursor will go back
    // after rendering if {cursor_back} is true.
    void RenderView_(const ViewInfo& vi, bool cursor_back) const;

    // Renders the {HintView}.
    void RenderHintView_() const;

    // Renders the {PromptView}.
    void RenderPromptView_() const;

    // Update window_height_ and window_width_.
    int UpdateWindowSize_();

    // Retrieves the current view
    ViewInfo & GetCurrentViewInfo_();

    // Reference to the singleton
    static std::shared_ptr<ViewRenderer> renderer_;
    static std::once_flag once_flag_;

    // prompt view class
    std::shared_ptr<PromptView> prompt_view_;
    // hint view class
    std::shared_ptr<HintView> hint_view_;

    // Stores latest window width and height.
    short window_height_;
    short window_width_;

    // Mutual excludes the rendering from input and output thread
    std::mutex render_mut_;
    
    // Stores the {ViewInfo} objects, and can be track using string
    std::map<std::string, ViewInfo> view_info_map_;

    // Stores the view name in chronological order.
    std::vector<std::string> view_names_;
    
    // Keeps tracking current view to support view switching.
    size_t curr_view_idx_;
};

#endif