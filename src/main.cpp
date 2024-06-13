#include <iostream>
#include <string>
#include <sstream>
#include <filesystem>
#include <vector>
using namespace std;
enum validCommands
{
    echo,
    cd,
    exit0,
    type,
    invalid,
};
validCommands isValid(std::string command) {
    command = command.substr(0, command.find(" "));
    if (command == "echo") return validCommands::echo;
    if (command == "cd") return validCommands::cd;
    if (command == "exit") return validCommands::exit0;
    if (command == "type") return validCommands::type;
    return invalid;
}
std::string valid[4] = {"echo", "cd", "exit0"};
std::string get_path(std::string command) {
    std::string path_env = std::getenv("PATH");
    std::stringstream ss(path_env);
    std::string path;
    while (!ss.eof()) {
        getline(ss, path, ':');
        string abs_path = path + '/' + command;
        if (filesystem::exists(abs_path)) {
            return abs_path;
        }
    }
    return "";
}
int main() {
    // You can use print statements as follows for debugging, they'll be visible when running tests.
    // std::cout << "Logs from your program will appear here!\n";

    bool exit = false;
    while (!exit) {
        // Flush after every std::cout / std:cerr
        std::cout << std::unitbuf;
        std::cerr << std::unitbuf;
        std::cout << "$ ";
        std::string input;
        std::getline(std::cin, input);
        switch (isValid(input)) {
        case cd:
            break;
        case echo:
            input.erase(0, input.find(" ") + 1);
            std::cout << input << "\n";
            break;
        case exit0:
            exit = true;
            break;
        case type:
            input.erase(0, input.find(" ") + 1);
            if (isValid(input) != invalid) {
                std::cout << input << " is a shell builtin\n";
            }
            else {

                std::string path = get_path(input);
                if (path.empty()) {
                    std::cout << input << ": not found\n";
                }
                else {
                    std::cout << input << " is " << path << std::endl;
                }
            }
            break;
        default:
            std::istringstream iss(input);
            std::string command, arg;
            iss >> command;
            std::vector<std::string> arguments;
            while (iss >> arg)
            {
                arguments.push_back(arg);
            }

            std::string path = get_path(command);
            if (path.empty())
            {
                std::cout << command << ": command not found\n";
            }
            else
            {
                for (const auto& arg : arguments) {
                    path += " " + arg;  // Append arguments to the command string
                }
                std::cout<<path;
                int exitCode = std::system(command.c_str());
            }

            break;
        }
    }
}