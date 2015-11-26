//
//Define several objects of user input commands.
//
#ifndef COMMANDS_H
#define COMMANDS_H

//Base command object
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
	//
	//ExecuteOn()
	//an virtual function that will call producer's function to send commands to director.
	//
    virtual void ExecuteOn(class Producer& producer) = 0;
};

//Derived command object for start instruction.
struct StartCommand : Command {
    int number;
    StartCommand(const int& number_);
    virtual void ExecuteOn(Producer& producer) override;
};


//Derived command object for stop instruction.
struct StopCommand : Command {
    int number;
    StopCommand(const int& number_);
    virtual void ExecuteOn(Producer& producer) override;
};

//Derived command object for quit instruction.
struct QuitCommand : Command {
    QuitCommand();
    virtual void ExecuteOn(Producer& producer) override;
};

#endif