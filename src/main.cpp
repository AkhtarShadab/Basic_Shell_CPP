#include <cstddef>
#include <cstdlib>
#include <filesystem>
#include <iostream>
#include <pthread.h>
#include <vector>

enum class CommandType
{
    BUILTIN,
    EXECUTABLE,
    ILLEGAL,
    NONE,
};
struct CommandToken
{
    CommandType type;
    // If type is EXECUTABLE, then lexeme will be the path
    std::string lexeme;
};
std::vector<std::string> splitString(std::string, char = ' ');
std::string getCommandPath(std::string);
CommandToken tokenize(std::string);
int main()
{
    // Flush after every std::cout / std:cerr
    std::cout << std::unitbuf;
    std::cerr << std::unitbuf;
    // Uncomment this block to pass the first stage
    std::vector<std::string> inputBuffer;
    while (true)
    {
        std::cout << "$ ";
        std::string input;
        std::getline(std::cin, input);
        inputBuffer = splitString(input);
        CommandToken command = tokenize(inputBuffer[0]);
        switch (command.type)
        {
        case CommandType::BUILTIN:
            if (command.lexeme == "exit")
            {
                if (inputBuffer.size() > 1)
                {
                    return std::stoi(inputBuffer[1]);
                }
                return 0;
            }
            else if (command.lexeme == "echo")
            {
                if (inputBuffer.size() > 1)
                {
                    std::string outputValue;
                    for (int i = 1; i < inputBuffer.size() - 1; i++)
                    {
                        outputValue = inputBuffer[i];
                        if (inputBuffer[i][0] == '\\')
                        {
                            outputValue = inputBuffer[i].substr(1);
                        }
                        if (inputBuffer[i][0] == '"' || inputBuffer[i][0] == '\'')
                        {
                            outputValue = inputBuffer[i].substr(1, inputBuffer[i].size() - 2);
                        }
                        std::cout << outputValue; // << ' ';
                    }
                    outputValue = inputBuffer[inputBuffer.size() - 1];
                    if (inputBuffer[inputBuffer.size() - 1][0] == '\\')
                    {
                        outputValue = inputBuffer[inputBuffer.size() - 1].substr(1);
                    }
                    else if (inputBuffer[inputBuffer.size() - 1][0] == '"' ||
                             inputBuffer[inputBuffer.size() - 1][0] == '\'')
                    {
                        outputValue = inputBuffer[inputBuffer.size() - 1].substr(
                            1, inputBuffer[inputBuffer.size() - 1].size() - 2);
                    }
                    std::cout << outputValue << std::endl;
                }
                else
                {
                    std::cout << std::endl;
                }
            }
            else if (command.lexeme == "type")
            {
                if (inputBuffer.size() > 1)
                {
                    CommandToken checkType = tokenize(inputBuffer[1]);
                    switch (checkType.type)
                    {
                    case CommandType::BUILTIN:
                        std::cout << checkType.lexeme << " is a shell builtin" << std::endl;
                        break;
                    case CommandType::EXECUTABLE:
                        std::cout << inputBuffer[1] << " is " << checkType.lexeme
                                  << std::endl;
                        break;
                    case CommandType::ILLEGAL:
                        std::cout << checkType.lexeme << ": not found" << std::endl;
                        break;
                    case CommandType::NONE:
                        std::cout << std::endl;
                        break;
                    }
                }
            }
            else if (command.lexeme == "pwd")
            {
                std::string cwd = std::filesystem::current_path();
                std::cout << cwd << std::endl;
            }
            else if (command.lexeme == "cd")
            {
                if (inputBuffer.size() > 1)
                {
                    if (inputBuffer[1] == "~")
                    {
                        std::filesystem::current_path(std::getenv("HOME"));
                    }
                    else if (std::filesystem::exists(inputBuffer[1]))
                    {
                        std::filesystem::current_path(inputBuffer[1]);
                    }
                    else
                    {
                        std::cout << inputBuffer[1] << ": No such file or directory"
                                  << std::endl;
                    }
                }
                else
                {
                    std::filesystem::current_path(std::getenv("HOME"));
                }
            }
            break;
        case CommandType::ILLEGAL:
            std::cout << command.lexeme << ": command not found" << std::endl;
            break;
        default:
            std::cout << inputBuffer[0] << ": command not found" << std::endl;
            break;
        case CommandType::NONE:
            break;
        case CommandType::EXECUTABLE:
            std::string executableCommand = command.lexeme + " ";
            for (int i = 1; i < inputBuffer.size(); i++)
            {

                executableCommand += inputBuffer[i];
            }
            std::system(executableCommand.c_str());
            break;
        }
    }
}
std::vector<std::string> splitString(std::string inputString, char delimiter)
{
    std::vector<std::string> splitInputs;
    if (inputString.empty())
    {
        return splitInputs = {""};
    }
    size_t index = 0;
    std::string toInput = "";
    while (index < inputString.size())
    {
        if (inputString[index] == '\\')
        {
            if (toInput.empty())
            {
                toInput += inputString[index];
            }
            index++;
            toInput += inputString[index];
            index++;
        }
        else if (inputString[index] == '\'' || inputString[index] == '"')
        {
            char quote = inputString[index++];
            toInput = quote;
            while (inputString[index] != quote)
            {
                if (inputString[index] == '\\')
                {
                    if (inputString[index + 1] == quote)
                    {
                        index++;
                    }
                    else if (quote == '"' && (inputString[index + 1] == '$' ||
                                              inputString[index + 1] == '\\'))
                    {
                        index++;
                    }
                }
                toInput += inputString[index++];
            }
            toInput += inputString[index++];
            splitInputs.push_back(toInput);
            toInput = "";
        }
        else if (inputString[index] == delimiter)
        {
            if (!toInput.empty())
            {
                splitInputs.push_back(toInput);
            }
            if (delimiter == ' ' && splitInputs.size() > 1 &&
                splitInputs[splitInputs.size() - 1] != " ")
            {
                splitInputs.push_back(" ");
            }
            toInput = "";
            index++;
        }
        else
        {
            toInput += inputString[index++];
        }
    }
    if (!toInput.empty())
    {
        splitInputs.push_back(toInput);
    }
    return splitInputs;
}
std::string getCommandPath(std::string command)
{
    std::vector<std::string> paths = splitString(std::getenv("PATH"), ':');
    std::string path;
    for (int i = 0; i < paths.size(); i++)
    {
        path = paths[i] + '/' + command;
        if (std::filesystem::exists(path))
            return path;
    }
    return "";
}
CommandToken tokenize(std::string lexeme)
{
    CommandToken command;
    command.lexeme = lexeme;
    if (lexeme == "echo" || lexeme == "type" || lexeme == "exit" ||
        lexeme == "pwd" || lexeme == "cd")
    {
        command.type = CommandType::BUILTIN;
    }
    else if (lexeme.empty())
    {
        command.type = CommandType::NONE;
    }
    else
    {
        std::string path = getCommandPath(lexeme);
        if (!path.empty())
        {
            command.type = CommandType::EXECUTABLE;
            command.lexeme = path;
        }
        else
        {
            command.type = CommandType::ILLEGAL;
        }
    }
    return command;
}