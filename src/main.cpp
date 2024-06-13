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
    pwd,
    invalid,
};
validCommands isValid(std::string command) {
    command = command.substr(0, command.find(" "));
    if (command == "echo") return validCommands::echo;
    if (command == "cd") return validCommands::cd;
    if (command == "exit") return validCommands::exit0;
    if (command == "type") return validCommands::type;
    if (command == "pwd") return validCommands::pwd;
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
void get_info(std::string path)
{
    try{
    std::string targetPath;

        if (path.empty() || path == "~") {
            // Get the home directory from the HOME environment variable
            const char* homeDir = getenv("HOME");
            if (homeDir == nullptr) {
                std::cerr << "cd: Could not determine home directory. HOME environment variable not set." << std::endl;
                return;
            }
            targetPath = homeDir;
        } else if (path[0] == '~') {
            // Replace ~ with the home directory path
            const char* homeDir = getenv("HOME");
            if (homeDir == nullptr) {
                std::cerr << "cd: Could not determine home directory. HOME environment variable not set." << std::endl;
                return;
            }
            targetPath = std::string(homeDir) + path.substr(1); // Append the rest of the path
        } else {
            // Use the provided path as-is for relative and absolute paths
            targetPath = path;
        }

        // Resolve to an absolute path
        std::filesystem::path resolvedPath = std::filesystem::absolute(targetPath);

        // Check if the target path exists and is a directory
        if (!std::filesystem::exists(resolvedPath)) {
            std::cerr << "cd: " << path << ": No such file or directory" << std::endl;
            return;
        }

        if (!std::filesystem::is_directory(resolvedPath)) {
            std::cerr << "cd: " << path << ": Not a directory" << std::endl;
            return;
        }

        // Set the current path
        std::filesystem::current_path(resolvedPath);
    } catch (const std::filesystem::filesystem_error& e) {
        std::cerr << "cd: " << path << ": " << e.what() << std::endl;
    }
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
        std::istringstream iss(input);
        std::string command, arg;
        std::filesystem::path path;
        switch (isValid(input)) {
        case cd:
            iss>>arg;
            iss>>arg;
            get_info(arg);
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
        case pwd:
            std::cout<<std::filesystem::current_path().string()<<"\n";
            break;
        default:
            
            
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
                // std::cout<<p/ath;
                int exitCode = std::system(path.c_str());
            }

            break;
        }
    }
}