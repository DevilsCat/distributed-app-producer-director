#ifndef VIEWS_H
#define VIEWS_H
#include <iostream>
#include <mutex>
#include <memory>
#include <algorithm>
#include "Utils.h"

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
        std::lock_guard<std::mutex> lk(data_m_);
        cells_.push_back(cell);
        ++cur_cell_idx_;
    }

	void AddCell(CellType* cell) {
		AddCell(std::shared_ptr<CellType>(cell));
    }

	std::shared_ptr<CellType> GetCellAt(const size_t& idx) {
		std::lock_guard<std::mutex> lk(data_m_);
		if (idx >= cells_.size()) { return nullptr; }
		return cells_[idx];
    }

    // Only Proactor call this.
    void DeleteCellAt(const size_t& idx) {
        std::lock_guard<std::mutex> lk(data_m_);
        if (idx >= cells_.size()) { return; }
        cells_.erase(cells_.begin() + idx);
        --cur_cell_idx_;
    }

    // Only Proactor call this.
    void DeleteCell(std::shared_ptr<CellType> cell) {
        std::lock_guard<std::mutex> lk(data_m_);
        auto res = std::find(cells_.begin(), cells_.end(), cell);
        if (res == cells_.end()) { return; }  // not cell found.
        cells_.erase(res);
        --cur_cell_idx_;
    }

	std::vector<std::shared_ptr<CellType>> Query(std::function<bool(const CellType&)>FindFunc) {
		std::lock_guard<std::mutex> lk(data_m_);
		return Query_(FindFunc);
    }

	size_t Update(std::function<bool(const CellType&)>WhereFunc, std::function<void(CellType&)>UpdateFunc) {
		std::lock_guard<std::mutex> lk(data_m_);
		std::vector<std::shared_ptr<CellType>> query_res = Query_(WhereFunc);
		for (std::shared_ptr<CellType> cell : query_res)
			UpdateFunc(*cell);
		return query_res.size();
    }

	size_t Size() const { return cells_.size(); }

    // Proactor will call this
    // Input thread might all call this to refresh screen.
    virtual void Draw(const short& width, const short& height) const override {
        std::lock_guard<std::mutex> lk(data_m_);
        const short cell_width = (width - ID_COL_WIDTH) / (keys_.size());
        DrawTitle_(width);
        DrawColumnName_(cell_width);
        DrawCells_(cell_width, height - TABLE_VIEW_PRESERVED);
    }

    virtual void ScrollUp(const short& nline_scroll) override {
        std::lock_guard<std::mutex> lk(data_m_);
        short possible_nscroll = MIN(short(cells_.size()), nline_scroll - TABLE_VIEW_PRESERVED);
        cur_cell_idx_ = MAX(int(cur_cell_idx_ - possible_nscroll), possible_nscroll);
    }
    
    virtual void ScrollDown(const short& nline_scroll) override {
        std::lock_guard<std::mutex> lk(data_m_);
        cur_cell_idx_ = MIN(cur_cell_idx_ + nline_scroll - TABLE_VIEW_PRESERVED, cells_.size());
    }

    static TableView<CellType>* MakeView(const std::string& title) {
        return new TableView<CellType>(title);
    }

private:
	std::vector<std::shared_ptr<CellType>> Query_(std::function<bool(const CellType&)>FindFunc) {
		std::vector<std::shared_ptr<CellType>> res;
		for (std::shared_ptr<CellType> cell : cells_) {
			if (FindFunc(*cell))  res.push_back(cell);
		}
		return res;
	}

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
    mutable std::mutex data_m_;
    size_t cur_cell_idx_;
};

//
// Cells Class
//
// class TableViewCell
class TableViewCell {
public:
    virtual ~TableViewCell() {}
    virtual std::vector<std::string> get_keys() = 0;
    virtual std::string get_value(const std::string& key) = 0;
};

//
// class PlayTableViewCell.
class PlayTableViewCell : public TableViewCell {
public:
    PlayTableViewCell();

    PlayTableViewCell(const int& director_id, const int& play_id, const std::string& name, bool status);

    void set_director_id(const int& director_id);
	void set_play_id(const int& play_id);
    void set_name(const std::string& name);
    void set_status(const bool status);
	int director_id() const;
	int play_id() const;
	std::string name() const;
	bool status() const;

    std::vector<std::string> get_keys() override;
    std::string get_value(const std::string& key) override;		//Fixme director id
private:
    //std::string director_;
	int director_id_;
	int play_id_;
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

