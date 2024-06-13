#include <iostream>
#include <string>
enum commands
{
  echo,
  cd,
  quit,
  type,
  invalid
};
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
std::string builtin_check(std::string str)
{
  std::string ans;
  if(str=="echo"||str=="exit"||str=="type"||str=="cat"||str=="cd")
  {
    ans=" is a shell builtin";
    ans=str.append(ans);
  }
  else ans="nonexistent: not found";
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
      temp = builtin_check(temp);
      std::cout<<temp<<"\n";
      break;
    default:
      std::cout << input << ": command not found" << '\n';
      break;
    }
  }
  return 0;
}
