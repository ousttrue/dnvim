#include <process.hpp>
#include <iostream>
#include <string>
#include <chrono>
#include <thread>
#include <msgpackpp.h>
#include <vector>


class NVimDispatcher
{
	std::vector<uint8_t> m_buffer;

public:
	NVimDispatcher()
	{
	}

	void push_bytes(const char *bytes, size_t n)
	{
		auto pos = m_buffer.size();
		m_buffer.resize(pos + n);
		for (int i = 0; i < n; ++i, ++pos)
		{
			m_buffer[pos] = bytes[i];
		}

		while (!m_buffer.empty()) {
			if (!consume()) {
				break;
			}
		}
	}

private:
	bool consume()
	{
		auto msg = msgpackpp::parser(m_buffer.data(), m_buffer.size());
		auto d = msg.consumed_size();
		std::cout << "msg " << d << " bytes" << std::endl;
		m_buffer.erase(m_buffer.begin(), m_buffer.begin() + d);

		if (!msg.is_array()) {
			throw std::exception("is not error");
		}

		auto msgType = msg[0].get_number<int>();
		if (msgType == 0) {
			// request
			throw std::exception("not implemented");
		}
		else if (msgType == 1) {
			// response
			auto msgId = msg[1].get_number<int>();
			auto error = msg[2];
			auto payload = msg[3];

			std::cout
				<< "response: " << msgId
				//<< " => " << payload 
				<< std::endl;
		}
		else if (msgType == 2) {
			// notify
			auto method = msg[1].get_string();
			auto payload = msg[2];

			std::cout
				<< "notify: " << method
				//<<  " => " << payload 
				<< std::endl;

		}
		else {
			throw std::exception("invali msgType");
		}
	}
};


int main(int argc, char **argv)
{
	std::cout << std::endl << "Example 5 - demonstrates Process::try_get_exit_status" << std::endl;

	NVimDispatcher dispatcher;

	TinyProcessLib::Process process1("nvim --embed", "", [&dispatcher](const char *bytes, size_t n) {

		dispatcher.push_bytes(bytes, n);

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
