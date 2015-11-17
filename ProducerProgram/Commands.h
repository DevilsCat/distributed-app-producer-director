#ifndef COMMANDS_H
#define COMMANDS_H

struct Command {
    enum CommandType {
        kStart = 0,
        kStop,
        kQuit,
        kUnknown
    };
    CommandType cmd_type;
    Command(const CommandType& cmd_type_);
    virtual ~Command();
    virtual void ExecuteOn(class Producer& producer) = 0;
};

struct StartCommand : Command {
    int number;
    StartCommand(const int& number_);
    virtual void ExecuteOn(Producer& producer) override;
};

struct StopCommand : Command {
    int number;
    StopCommand(const int& number_);
    virtual void ExecuteOn(Producer& producer) override;
};

struct QuitCommand : Command {
    QuitCommand();
    virtual void ExecuteOn(Producer& producer) override;
};

#endif