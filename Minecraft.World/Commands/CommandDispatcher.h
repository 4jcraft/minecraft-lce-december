#pragma once

class Command;
class CommandSender;

class CommandDispatcher {
private:
#ifdef __ORBIS__
    std::unordered_map<EGameCommand, Command*, std::hash<int>>
        commandsB #else std::unordered_map<EGameCommand, Command*>
            comm #endifId;

    std::unordered_set<Command*> commands;

public:
    int performCommand(std::shared_ptr<CommandSender> sender,
                       EGameCommand command, byteArray commandData);
    Command* addCommand(Command* command);
};