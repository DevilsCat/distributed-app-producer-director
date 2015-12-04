#ifndef TABLE_VIEW_CELLS_H
#define TABLE_VIEW_CELLS_H
#include <string>
#include <vector>

//
// Cells Class
//
// class TableViewCell
class TableViewCell {
public:
    virtual ~TableViewCell() {}
    virtual std::vector<std::string> get_keys() = 0;
    virtual std::string get_value(const std::string& key) = 0;
    virtual double get_weight(const std::string&key) = 0;
};

//
// class PlayTableViewCell.
class PlayTableViewCell : public TableViewCell {
public:
    enum StatusType {
        kAvailable, kUnavailable, kInProgress
    };

    PlayTableViewCell();

    PlayTableViewCell(const int& director_id, const int& play_id, const std::string& name, StatusType status);

    void set_director_id(const int& director_id);
    void set_play_id(const int& play_id);
    void set_name(const std::string& name);
    void set_status(const StatusType status);
    int director_id() const;
    int play_id() const;
    std::string name() const;
    StatusType status() const;

    virtual std::vector<std::string> get_keys() override;
    virtual std::string get_value(const std::string& key) override;
    virtual double get_weight(const std::string& key) override;
private:
    int director_id_;
    int play_id_;
    std::string name_;
    StatusType status_;
};

//
// class DebugTableViewCell
class DebugTableViewCell : public TableViewCell {
public:
    DebugTableViewCell();
    DebugTableViewCell(const std::string& message);

    virtual std::vector<std::string> get_keys() override;
    virtual std::string get_value(const std::string& key) override;
    virtual double get_weight(const std::string& key) override;

private:
    std::string message_;
};

#endif

