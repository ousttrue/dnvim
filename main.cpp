#include <process.hpp>
#include <iostream>
#include <string>
#include <chrono>
#include <thread>


int main(int argc, char **argv)
{
	std::cout << std::endl << "Example 5 - demonstrates Process::try_get_exit_status" << std::endl;

	TinyProcessLib::Process process1("nvim.exe --embed", "", [](const char *bytes, size_t n) {
		std::cout << "Output from stdout: " << std::string(bytes, n);
	});
	auto exit_status = process1.get_exit_status();
	std::this_thread::sleep_for(std::chrono::seconds(2));

	return exit_status;
}
