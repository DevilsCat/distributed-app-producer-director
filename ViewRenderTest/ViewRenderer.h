#pragma once
#include <mutex>
#include <iostream>
#include <vector>
#include "TableView.h"
#include <memory>
#include <map>
#include "PromptView.h"

#define PROMPT_OFFSET    1

#define PROMPT_MARK      ">>"

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
    PromptView* GetPrompt() const;

    void Render();
    void Render(const std::string& view_name);
    void RenderPrompt();
    
    //void NextView();
    //void PrevView();

private:
    ViewRenderer();
    ViewRenderer(const ViewRenderer&);
    ViewRenderer& operator=(const ViewRenderer&);

    // Core render methods
    void RenderAll_();
    void RenderViews_();
    void RenderView_(const ViewInfo& vi) const;
    void RenderPrompt_();

    // Update window_height_ and window_width_.
    void OnWindowChanged_();
    void UpdateViewStartPos_();
    int UpdateWindowSize_();

    // "Goto" function, for specific position on the window.
    void GoToPromptPos(WidthType x = 0);

    static ViewRenderer* renderer_;
    static std::once_flag once_flag_;

    std::ostream& std_out_;

    // prompt view class
    std::shared_ptr<PromptView> prompt_;

    // Stores latest window width and height.
    HeightType window_height_;
    WidthType window_width_;

    // mutual excludes the rendering from input and output thread
    std::mutex render_mut_;
    
    // view update
    std::map<std::string, ViewInfo> view_info_map_;
    std::vector<std::string> view_names_;

//    size_t curr_view_idx_;
};
