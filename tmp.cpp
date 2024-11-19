#include <unordered_map>
#include <iostream>
#include <string>

int main()
{
	std::cout << std::hash<std::string>{}(std::string{"lol"}) << '\n';

	return 0;
}
