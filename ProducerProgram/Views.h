﻿#ifndef VIEWS_H
#define VIEWS_H
#include <iostream>
#include "Utils.h"
#include <mutex>
#include <memory>
#include <algorithm>

//
// class View.
class View {  // interface of any View class
public:
    View(const std::string& title) : title_(title) {}
    virtual ~View() {}

    // Every view must respond a draw event.
    virtual void Draw(const short& width, const short& height) const = 0;
    
    // Each view may respond to a scroll event.
    virtual void ScrollUp(const short& nline_scroll) {}
    virtual void ScrollDown(const short& nline_scroll) {}

protected:
    virtual void DrawTitle_(const short& width) const {
        std::cout << utils::windows::left(title_, width, '=') << std::endl;
    }

private:
    std::string title_;
};

//
// class HintView.
class HintView : public View {
public:
    static const char* kHintDefault;

    static HintView* MakeView(const std::string& title);

    std::string hint() const;
    void set_hint(const std::string& hint);

    virtual void Draw(const short& width, const short& height) const override;

private:
    explicit HintView(const std::string& title);
    
    void DrawHint_(const short& width) const;

    std::string hint_;
    mutable std::mutex m_;
};

//
// class PromptView.
class PromptView : public View {
public:
    static const char* sPromptMark;

    static PromptView* MakeView();

    void AddChar(const char& ch);
    std::string user_buf() const;
    void ClearUserInput();

    void Draw(const short& width, const short& height) const override;

private:
    PromptView(const std::string& title);

    void DrawPrompt_(const short& width) const;

    std::string user_buf_;
};


#define ID_COL_WIDTH            (4)  // Change this to adjust width of id column.
#define TABLE_VIEW_PRESERVED    (2)  // One for title and one for columns.
//
// class TableView.
template<class CellType>
class TableView : public View {
    typedef std::string KeyType;
    typedef std::string ValType;
public:
    ~TableView() {}

    // Only Proactor call this.
    void AddCell(std::shared_ptr<CellType> cell) {
        std::lock_guard<std::mutex> lk(m_);
        cell->set_parent(this);
        cells_.push_back(cell);
        ++cur_cell_idx_;
    }

    // Only Proactor call this.
    void DeleteCellAt(const size_t& idx) {
        std::lock_guard<std::mutex> lk(m_);
        if (idx < 0 || idx > cells_.size()) { return; }
        cells_.erase(cells_.begin() + idx);
        --cur_cell_idx_;
    }

    // Only Proactor call this.
    void DeleteCell(CellType* cell) {
        std::lock_guard<std::mutex> lk(m_);
        auto res = std::find_if(cells_.begin(), cells_.end(), [&cell](std::shared_ptr<CellType> elt)
        {
            return elt.get() == cell;
        });
        if (res != cells_.end()) cells_.erase(res);
    }

    // Proactor will call this
    // Input thread might all call this to refresh screen.
    virtual void Draw(const short& width, const short& height) const override {
        std::lock_guard<std::mutex> lk(m_);
        const short cell_width = (width - ID_COL_WIDTH) / (keys_.size());
        DrawTitle_(width);
        DrawColumnName_(cell_width);
        DrawCells_(cell_width, height - TABLE_VIEW_PRESERVED);
    }

    virtual void ScrollUp(const short& nline_scroll) override {
        std::lock_guard<std::mutex> lk(m_);
        short possible_nscroll = MIN(short(cells_.size()), nline_scroll - TABLE_VIEW_PRESERVED);
        cur_cell_idx_ = MAX(int(cur_cell_idx_ - possible_nscroll), possible_nscroll);
    }
    
    virtual void ScrollDown(const short& nline_scroll) override {
        std::lock_guard<std::mutex> lk(m_);
        cur_cell_idx_ = MIN(cur_cell_idx_ + nline_scroll - TABLE_VIEW_PRESERVED, cells_.size());
    }

    static TableView<CellType>* MakeView(const std::string& title) {
        return new TableView<CellType>(title);
    }

private:
    TableView(const std::string& title) :
        View(title), cur_cell_idx_(0)
    {
        keys_ = std::make_shared<CellType>()->get_keys();
    }

    void DrawColumnName_(const short& cell_w) const {
        std::cout << utils::windows::left("ID", ID_COL_WIDTH, ' ');
        for (size_t i = 0; i < keys_.size(); ++i) {
            std::cout << utils::windows::left(keys_[i], cell_w, ' ');
        }
        std::cout << std::endl;
    }

    void DrawCells_(const short& cell_w, const size_t& num_cells) const {
        size_t greater = MAX(int(cur_cell_idx_ - num_cells), 0);
        for (size_t i = greater; i < cur_cell_idx_; ++i) {
            DrawCell_(i, cell_w);
            std::cout << std::endl;
        }
    }

    void DrawCell_(const size_t& idx, const short& cell_w) const {
        std::cout << utils::windows::left(std::to_string(idx), ID_COL_WIDTH, ' ');
        std::for_each(keys_.begin(), keys_.end(), [&](const KeyType& key) {
            std::cout << utils::windows::left(cells_[idx]->get_value(key), cell_w, ' ');
        });
    }

    std::string title_;
    std::vector<KeyType> keys_;
    std::vector<std::shared_ptr<CellType>> cells_;
    mutable std::mutex m_;
    size_t cur_cell_idx_;
};

//
// Cells Class
//
// class TableViewCell
class TableViewCell {
public:
    TableViewCell() : parent_(nullptr) {}
    virtual ~TableViewCell() {}
    virtual std::vector<std::string> get_keys() = 0;
    virtual std::string get_value(const std::string& key) = 0;
    
    // used to delete itself from parent
    template<typename TableViewType>
    void set_parent(TableViewType* parent) { parent_ = parent; }
    void DeleteFromParent() {
        if (parent_)
            static_cast<TableView<TableViewCell>*>(parent_)->DeleteCell(this);
    }
private:
    void* parent_;
};

//
// class PlayTableViewCell.
class PlayTableViewCell : public TableViewCell {
public:
    PlayTableViewCell();

    PlayTableViewCell(const std::string& director, const std::string& name, bool status);

    void set_director(const std::string& director);
    void set_name(const std::string& name);
    void set_status(const bool status);

    std::vector<std::string> get_keys() override;
    std::string get_value(const std::string& key) override;
private:
    std::string director_;
    std::string name_;
    bool status_;
};

//
// class DebugTableViewCell
class DebugTableViewCell : public TableViewCell {
public:
    DebugTableViewCell();
    DebugTableViewCell(const std::string& message);

    std::vector<std::string> get_keys() override;
    std::string get_value(const std::string& key) override;

private:
    std::string message_;
};

#endif

