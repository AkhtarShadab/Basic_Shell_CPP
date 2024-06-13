#include <iostream>
#include <vector>
#include <string>
#include <cstring>  // For strtok
#include <cstdlib>  // For getenv
#include <dirent.h> // For opendir, readdir, closedir
#include <sys/stat.h> // For stat


enum commands
{
  echo,
  cd,
  quit,
  type,
  invalid
};



std::vector<std::string> getPaths() {
    std::vector<std::string> paths;
    char* pathEnv = getenv("PATH");

    if (pathEnv) {
        char* pathCopy = strdup(pathEnv);
        char* token = strtok(pathCopy, ":"); // Use ":" for Unix-like systems. Use ";" for Windows.

        while (token != nullptr) {
            paths.push_back(token);
            token = strtok(nullptr, ":");
        }

        free(pathCopy);
    }

    return paths;
}

bool isExecutable(const std::string& filePath) {
    struct stat sb;
    if (stat(filePath.c_str(), &sb) == 0) {
        return sb.st_mode & S_IXUSR;
    }
    return false;
}


commands string_to_command(std::string str)
{
  
  std::string check;
  int pos=str.find(" ");
  if(pos==-1)
  {
    return invalid;
  }
  else{
    check = str.substr(0,pos);
    if(check=="exit") return quit;
    if(check=="cd") return cd;
    if(check=="type") return type;
    if(check=="echo") return echo;
  }
  
  return invalid;
}
std::string builtin_type(std::string str)
{
  std::string ans;
  std::vector<std::string> paths=getPaths();
  if(str=="echo"||str=="exit"||str=="type"||str=="cd")
  {
    ans=" is a shell builtin";
    ans=str.append(ans);
    return ans;
  }
  bool found = false;

    for (const std::string& dir : paths) {
        std::string fullPath = dir + "/" + str;

        if (isExecutable(fullPath)) {
            ans = " is ";
            ans = ans.append(fullPath);
            found=true;
        }
    }

    if (!found) {
        ans=": not found";
    }
    ans=str.append(ans);
    return ans;
} 
int main()
{
  // Flush after every std::cout / std:cerr
  std::cout << std::unitbuf;
  std::cerr << std::unitbuf;
  
  std::string input = " ";
  while(!input.empty())
  {
    std::string temp="";
    std::cout << "$ ";
    std::getline(std::cin, input);
    
    switch(string_to_command(input))
    {
    case echo:
      std::cout << input.substr(5) << '\n';
      break;
    case quit:
      return 0;
    case type:
      temp=input.substr(5);
      temp = builtin_type(temp);
      std::cout<<temp<<"\n";
      break;
    default:
      std::cout << input << ": command not found" << '\n';
      break;
    }
  }
  return 0;
}
