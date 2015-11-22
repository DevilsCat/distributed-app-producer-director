#pragma once
#include <string>
#include <vector>
#include <iterator>
#include <iostream>
#include <algorithm>
#include "Utils.h"
#include <mutex>
#include <memory>
#include "View.h"

using namespace utils;

template<class CellType>
class TableView : public View {
    typedef std::string KeyType;
    typedef std::string ValType;
public:
    ~TableView() {}

    // Only Proactor call this.
    void AddCell(std::shared_ptr<CellType> cell) {
        std::lock_guard<std::mutex> lk(m_);
        cells_.push_back(cell);
    }

    // Only Proactor call this.
    void DeleteCellAt(const size_t& idx) {
        std::lock_guard<std::mutex> lk(m_);
        if (idx < 0 || idx > cells_.size())  return;
        cells_.erase(cells_.begin() + idx);
    }

    // Only Proactor call this.
    void DeleteCell(std::shared_ptr<CellType> cell) {
        std::lock_guard<std::mutex> lk(m_);
        auto res = std::find(cells_.begin(), cells_.end(), cell);
        if (res != cells_.end()) cells_.erase(res);
    }

    // Proactor will call this
    // Input thread might all call this to refresh screen.
    virtual void Draw(const short& width) const override {
        std::lock_guard<std::mutex> lk(m_);
        const short cell_width = width / (keys_.size() + 1);
        DrawTitle(width);
        DrawColumnName(cell_width);
        DrawCells(cell_width);
    }

    static TableView<CellType>* MakeView(const std::string& title) {
        return new TableView<CellType>(title);
    }

private:
    TableView(const std::string& title) :
        View(title)
    {
        keys_ = std::make_shared<CellType>()->get_keys();
    }

    void DrawColumnName(const short& cell_w) const {
        std::cout << windows::left("ID", cell_w, ' ');
        for (size_t i = 0; i < keys_.size(); ++i) {
            std::cout << windows::left(keys_[i], cell_w, ' ');
        }
        std::cout << std::endl;
    }

    void DrawCells(const short& cell_w) const {
        for (size_t i = 0; i < cells_.size(); ++i) {
            DrawCell(i, cell_w);
            std::cout << std::endl;
        }
    }

    void DrawCell(const size_t& idx, const short& cell_w) const {
        std::cout << windows::left(std::to_string(idx), cell_w, ' ');
        std::for_each(keys_.begin(), keys_.end(), [&](const KeyType& key) {
            std::cout << windows::left(cells_[idx]->get_value(key), cell_w, ' ');
        });
    }

    std::string title_;
    std::vector<KeyType> keys_;
    std::vector<std::shared_ptr<CellType>> cells_;
    mutable std::mutex m_;
};


class TableViewCell {
public:
    virtual ~TableViewCell() {}
    virtual std::vector<std::string> get_keys() = 0;
    virtual std::string get_value(const std::string& key) = 0;
};

class PlayTableViewCell : public TableViewCell {
public:
    PlayTableViewCell() : director_(), name_(), status_(false) {}

    PlayTableViewCell(const std::string& director, const std::string& name, bool status) : 
        director_(director), name_(name), status_(status) 
    {}

    void set_director(const std::string& director) { director_ = director; }
    void set_name(const std::string& name) { name_ = name; }
    void set_status(const bool status) { status_ = status; }

    std::vector<std::string> get_keys() override {
        const char* keys[] = { "Director", "Name", "Status" };
        return std::vector<std::string>(std::begin(keys), std::end(keys));
    }

    std::string get_value(const std::string& key) override {
        if (key == "Director") return director_;
        if (key == "Name")     return name_;
        if (key == "Status")   return status_ ? "available" : "inavailable";
        return std::string();
    }

private:
    std::string director_;
    std::string name_;
    bool status_;
};

class PersonTableViewCell : public TableViewCell {
public:
    PersonTableViewCell() {}

    PersonTableViewCell(const std::string& name, unsigned age, const std::string& addr, const std::string& phone_num) : 
        name_(name), age_(age), addr_(addr), phone_num_(phone_num) 
    {}

    std::vector<std::string> get_keys() override {
        const char* keys[] = { "Name", "Age", "Address", "Phone Number" };
        return std::vector<std::string>(std::begin(keys), std::end(keys));
    }

    std::string get_value(const std::string& key) override {
        if (key == "Name")          return name_;
        if (key == "Age")           return std::to_string(age_);
        if (key == "Address")       return addr_;
        if (key == "Phone Number")  return phone_num_;
        return std::string();
    }

private:
    std::string name_;
    unsigned age_;
    std::string addr_;
    std::string phone_num_;
};
