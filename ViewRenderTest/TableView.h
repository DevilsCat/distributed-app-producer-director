#pragma once
#include <string>
#include <vector>
#include <iterator>
#include <iostream>
#include <algorithm>
#include <sstream>
#include "Utils.h"

using namespace utils;

template<class CellType>
class TableView {
    typedef std::string KeyType;
    typedef std::string ValType;
public:
    TableView(const std::string& title) :
        title_(title)
    {
        CellType* cell = new CellType;
        keys_ = cell->get_keys();
        delete cell;
    }

    ~TableView() {
        for (CellType* cell : cells_)  delete cell;
    }

    void AddCell(CellType* cell) {
        cells_.push_back(cell);
    }

    void DeleteCellAt(const size_t& idx) {
        if (idx < 0 || idx > cells_.size())  return;
        delete cells_[idx];
        cells_.erase(cells_.begin() + idx);
    }

    void Draw(const short& width) const {
        const short cell_width = width / (keys_.size() + 1);
        DrawTitle(width);
        DrawColumnName(cell_width);
        DrawCells(cell_width);
    }

private:
    void DrawTitle(const short& width) const {
        std::cout << windows::center(title_, width, '=') << std::endl;
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
    std::vector<CellType*> cells_;
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
