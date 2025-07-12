#include <string>
#include <unistd.h>
#include <vector>

std::vector<std::string> split(std::string cmd) {
  std::vector<std::string> args;
  size_t start = 0, end = 0;
  while ((end = cmd.find(' ', start)) != std::string::npos) {
    args.push_back(cmd.substr(start, end - start));
    start = end + 1;
  }
  args.push_back(cmd.substr(start));
  return args;
}

int summon(std::string cmd) {
  auto args = split(cmd);
  if (args.empty())
    return 2;
  std::vector<char *> argv;
  for (auto &arg : args) {
    argv.push_back(arg.data());
  }
  argv.push_back(nullptr);

  pid_t pid = fork();
  if (pid == -1) {
    return 1;
  } else if (pid == 0) {
    execvp(argv[0], argv.data());
  }
  return 0;
}
