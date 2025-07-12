#include <string>
#include <vector>

std::vector<std::string> split(std::string cmd);

// 0 for Success
// 1 for fork() failure
// 2 for Nothing to do
int summon(std::string cmd);
