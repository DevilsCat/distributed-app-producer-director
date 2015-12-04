// Views.h -- This file declares View class family which customize draw routine on screen.
// Created by Anqi Zhang, Yu Xiao, Yuhan Hao, all rights reserved.
//
#ifndef VIEWS_H
#define VIEWS_H
#include <iostream>
#include <mutex>
#include <memory>
#include <algorithm>
#include "Utils.h"

using namespace utils;

// Interface for View class family.
class View {
public:
    // Passes in the string as title.
    View(const std::string& title);

    // First virtual function to make sure polymophism.
    virtual ~View();

    // Every view must respond a draw event.
    virtual void Draw(const short& width, const short& height) const = 0;
    
    // Each view may respond to a scroll up event, no operation of not overrided.
    virtual void ScrollUp(const short& nline_scroll);

    // Each view may respond to a scroll down event, no operation of not overrided.
    virtual void ScrollDown(const short& nline_scroll);

protected:
    // Provides a standard way to draw title.
    // Sample:
    //   |MyTitle============================|
    virtual void DrawTitle_(const short& width) const;

private:
    // Stores the title of this {View}.
    std::string title_;
};

// Defines a routine to draw hint view.
class HintView : public View {
public:
    // Defines the default hint message.
    static const char* kHintDefault;

    // Factory method that allocates a view on heap
    // Please manage the view memroy deallocation manually.
    static HintView* MakeView(const std::string& title);

    // Getter for hint member variable.
    std::string hint() const;

    // Setter for hint member variable.
    void set_hint(const std::string& hint);

    // Determines the concrete routine to draw hint view.
    // This funtion is a "Thread-safe Interface".
    virtual void Draw(const short& width, const short& height) const override;

private:
    // Hides this so that program can only use factory method to create a view.
    explicit HintView(const std::string& title);
    
    // Concrete method to draw the hint.
    void DrawHint_(const short& width) const;

    // Stores the hint message.
    std::string hint_;

    // Provides the mutual exclusion of hint message manipulation and draw.
    mutable std::mutex m_;
};

// Defines a routine to draw prompt view.
class PromptView : public View {
public:
    // Defines the default mark shape of prompt.
    static const char* sPromptMark;

    // Factory method that allocates a view on heap
    // Please manage the view memroy deallocation manually.
    static PromptView* MakeView();

    // Adds a char from user to prompt.
    void AddChar(const char& ch);

    // Returns the user input buffer (before cariage return).
    std::string user_buf() const;

    // Clears user input buffer.
    void ClearUserInput();

    // Determines the concrete routine to draw prompt view.
    // This funtion is a "Thread-safe Interface".
    void Draw(const short& width, const short& height) const override;

private:
    // Hides this so that program can only use factory method to create a view. 
    PromptView(const std::string& title);

    // Concrete method to draw the {PromptView}.
    void DrawPrompt_(const short& width) const;

    // Stores the user input which need to echo back to screen.
    std::string user_buf_;
};

#define ID_COL_WIDTH            (4)  // Change this to adjust width of id column.
#define TABLE_VIEW_PRESERVED    (2)  // One for title and one for columns.


// Defines a routine to draw a table view
//
// This view also support CRUD database-like query.
//
// Template are used to costomize the content of a cell, it
// accepets all {TableViewCell} family objects.
template<class CellType>
class TableView : public View {
    // The type of column attributes.
    typedef std::string KeyType;
    // The type of value for column.
    typedef std::string ValType;
public:
    ~TableView() {}

    // Adds a cell to this table, given a shared pointer
    void AddCell(std::shared_ptr<CellType> cell) {
        std::lock_guard<std::mutex> lk(data_m_);
        cells_.push_back(cell);
        ++cur_cell_idx_;
    }

    // Adds a raw pointer of cell to this table.
    void AddCell(CellType* cell) {
        AddCell(std::shared_ptr<CellType>(cell));
    }

    // Returns the cell given index in cell.
    std::shared_ptr<CellType> GetCellAt(const size_t& idx) {
        std::lock_guard<std::mutex> lk(data_m_);
        if (idx >= cells_.size()) { return nullptr; }
        return cells_[idx];
    }

    // Queries the database and returns a vector of cell which matches the where clause.
    std::vector<std::shared_ptr<CellType>> Query(std::function<bool(const CellType&)>WhereFunc) {
        std::lock_guard<std::mutex> lk(data_m_);
        return Query_(WhereFunc);
    }

    // Updates the selected cells in database defined by Update clause, returns 
    size_t Update(std::function<bool(const CellType&)>WhereFunc, std::function<void(CellType&)>UpdateFunc) {
        std::lock_guard<std::mutex> lk(data_m_);
        std::vector<std::shared_ptr<CellType>> query_res = Query_(WhereFunc);
        for (std::shared_ptr<CellType> cell : query_res)
            UpdateFunc(*cell);
        return query_res.size();
    }

    // Deletes the selected cells in database 
    size_t Delete(std::function<bool(const CellType&)>WhereFunc) {
        std::lock_guard<std::mutex> lk(data_m_);
        std::vector<std::shared_ptr<CellType>> query_res = Query_(WhereFunc);
        for (std::shared_ptr<CellType> cell : query_res)
            DeleteCell_(cell);
        return query_res.size();
    }

    // Returns the number of cells stores in the view.
    size_t Size() const {
        std::lock_guard<std::mutex> lk(data_m_);
        return cells_.size();
    }

    // Defines the routine of drawing this view.
    virtual void Draw(const short& width, const short& height) const override {
        std::lock_guard<std::mutex> lk(data_m_);
        DrawTitle_(width);
        DrawColumnName_(width);
        DrawCells_(width, height - TABLE_VIEW_PRESERVED);
    }

    // Defines the routine for responding a scroll up request.
    virtual void ScrollUp(const short& nline_scroll) override {
        std::lock_guard<std::mutex> lk(data_m_);
        short possible_nscroll = MIN(short(cells_.size()), nline_scroll - TABLE_VIEW_PRESERVED);
        cur_cell_idx_ = MAX(int(cur_cell_idx_ - possible_nscroll), possible_nscroll);
    }
    
    // Defines the routine for responding a scroll down request.
    virtual void ScrollDown(const short& nline_scroll) override {
        std::lock_guard<std::mutex> lk(data_m_);
        cur_cell_idx_ = MIN(cur_cell_idx_ + nline_scroll - TABLE_VIEW_PRESERVED, cells_.size());
    }

    // Fatory method to 
    static TableView<CellType>* MakeView(const std::string& title) {
        return new TableView<CellType>(title);
    }

private:
    // Hide constructor to adapt factory method.
    // 
    TableView(const std::string& title) : View(title), cur_cell_idx_(0) {
        keys_ = std::make_shared<CellType>()->get_keys();
    }

    // Acutual "do-work" query function, that retrieves data from database.
    std::vector<std::shared_ptr<CellType>> Query_(std::function<bool(const CellType&)>FindFunc) {
        std::vector<std::shared_ptr<CellType>> res;
        for (std::shared_ptr<CellType> cell : cells_)
            if (FindFunc(*cell))  res.push_back(cell);
        return res;
    }

    // Acutal "do-work" delete cell function, that delete the cell from database.
    void DeleteCell_(std::shared_ptr<CellType> cell) {
        auto res = std::find(cells_.begin(), cells_.end(), cell);
        if (res == cells_.end()) { return; }  // not cell found.
        cells_.erase(res);
        --cur_cell_idx_;
    }
    
    // Draw the column name as the first row, adapt the width
    // and keys automatically.
    // Sample:
    //    ID  Col1  Col2  Col3 ... ColN
    void DrawColumnName_(const short& width) const {
        std::cout << windows::left("ID", ID_COL_WIDTH, ' ');
        const short rest_w = width - ID_COL_WIDTH;
        CellType* empty_cell = new CellType;  // used to get weight.
        for (size_t i = 0; i < keys_.size(); ++i) {
            short cell_w = short(rest_w * empty_cell->get_weight(keys_[i]));
            std::cout << windows::truncate(windows::left(keys_[i], cell_w, ' '), cell_w);
        }
        std::cout << std::endl;
        delete empty_cell;
    }

    // Draw other cells as the the rest rows, it draws the latest N numbers
    // of cells.
    void DrawCells_(const short& width, const size_t& num_cells) const {
        size_t greater = MAX(int(cur_cell_idx_ - num_cells), 0);  // Display the latest cells.
        for (size_t i = greater; i < cur_cell_idx_; ++i) {
            DrawCell_(i, width);
            std::cout << std::endl;
        }
    }

    // Defines the routine to draw single cell based on cell's weight and if the value
    // name is too long, a truncation will perform.
    void DrawCell_(const size_t& idx, const short& width) const {
        std::cout << windows::left(std::to_string(idx), ID_COL_WIDTH, ' ');  // left align
        const short rest_w = width - ID_COL_WIDTH;
        int col_idx = 0;
        std::for_each(keys_.begin(), keys_.end(), [&](const KeyType& key) {
            short cell_w = short(rest_w * cells_[idx]->get_weight(keys_[col_idx++]));  // determine the width by cell weight.
            std::cout << windows::truncate(windows::left(cells_[idx]->get_value(key), cell_w, ' '), cell_w);
        });
    }

    // Stores the all keys of the cell.
    std::vector<KeyType> keys_;

    // Stores the added cells.
    std::vector<std::shared_ptr<CellType>> cells_;
    
    // Provides multual exclusion for accessing cell data.
    mutable std::mutex data_m_;

    // Traces the current last cell that can show on screen.
    size_t cur_cell_idx_;
};



#endif

