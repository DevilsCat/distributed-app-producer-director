#include "stdafx.h"
#include "Commands.h"
#include "Producer.h"

Command::Command(const CommandType& cmd_type_) : 
    cmd_type(cmd_type_) 
{}

Command::~Command() {}

StartCommand::StartCommand(const int& number_) : 
    Command(kStart), number(number_) 
{}

void StartCommand::ExecuteOn(Producer& producer) {
    producer.Start(number);
}

StopCommand::StopCommand(const int& number_) : 
    Command(kStop), number(number_) 
{}

void StopCommand::ExecuteOn(Producer& producer) {
    producer.Stop(number);
}

QuitCommand::QuitCommand(): 
    Command(kQuit) 
{}

void QuitCommand::ExecuteOn(Producer& producer) {
    producer.Quit();
}
