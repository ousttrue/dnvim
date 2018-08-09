#include <process.hpp>
#include <iostream>
#include <string>
#include <chrono>
#include <thread>
#include <msgpackpp.h>


int main(int argc, char **argv)
{
	std::cout << std::endl << "Example 5 - demonstrates Process::try_get_exit_status" << std::endl;

	TinyProcessLib::Process process1("nvim --embed", "", [](const char *bytes, size_t n) {

		auto u = msgpackpp::parser((const uint8_t*)bytes, n);
		std::cout << "Output from stdout: " << u.to_json() << std::endl;

	}, nullptr, true);

	msgpackpp::packer packer;
	packer.pack_array(4);
	packer.pack_integer(0);
	packer.pack_integer(1);
	packer.pack_str("nvim_ui_attach");
	packer.pack_array(3);
	packer.pack_integer(80);
	packer.pack_integer(56);
	packer.pack_map(0);

	// std::vector<std::uint8_t>
	auto p = packer.get_payload();
	process1.write((const char*)p.data(), p.size());

	std::this_thread::sleep_for(std::chrono::seconds(2));
	auto exit_status = process1.get_exit_status();

	return exit_status;
}
