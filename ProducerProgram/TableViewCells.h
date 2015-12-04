// TableViewCell.h -- This files declares Table View Cell class family.
// Created by Yu Xiao, Anqi Zhang, Yuhan Hao
//
#ifndef TABLE_VIEW_CELLS_H
#define TABLE_VIEW_CELLS_H
#include <string>
#include <vector>


// Interface defining routines for customizing table view cell.
class TableViewCell {
public:
    virtual ~TableViewCell() {}

    // Every customized cell should provides keys (columns) for {TableView}.
    virtual std::vector<std::string> get_keys() = 0;

    // Every customized cell should provides stringify value associate with key.
    virtual std::string get_value(const std::string& key) = 0;

    // Every customized cell should define the weight for each key (column).
    virtual double get_weight(const std::string&key) = 0;
};

// Customized cell for the play information from director.
class PlayTableViewCell : public TableViewCell {
public:
    // Declares the type of play status.
    enum StatusType {
        kAvailable, kUnavailable, kInProgress
    };

    // An empty cell constructor which used to provide key information to {TableView}.
    PlayTableViewCell();

    // Initializes the play 
    PlayTableViewCell(const int& director_id, const int& play_id, const std::string& name, StatusType status);

    // Getter and Setter for member variables.
    void set_director_id(const int& director_id);
    void set_play_id(const int& play_id);
    void set_name(const std::string& name);
    void set_status(const StatusType status);
    int director_id() const;
    int play_id() const;
    std::string name() const;
    StatusType status() const;

    // Implements cell routines for {TableView}.
    virtual std::vector<std::string> get_keys() override;
    virtual std::string get_value(const std::string& key) override;
    virtual double get_weight(const std::string& key) override;
private:
    // Stores the director id.
    int director_id_;
    // Stores the play id in tha director.
    int play_id_;
    // Stores the script name of play.
    std::string name_;
    // Stores the available status of play.
    StatusType status_;
};

// Customized cell for Debugging information
class DebugTableViewCell : public TableViewCell {
public:
    // Empty constructor for table view to get keys information
    DebugTableViewCell();

    // Passes in debug message string.
    DebugTableViewCell(const std::string& message);

    // Implements cell routines for  {TableView}
    virtual std::vector<std::string> get_keys() override;
    virtual std::string get_value(const std::string& key) override;
    virtual double get_weight(const std::string& key) override;

private:
    // Stores debugging message string.
    std::string message_;
};

#endif

