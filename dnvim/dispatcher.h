#include <msgpackpp.h>
#include <vector>
#include <unordered_map>
#include <string>


class Dispatcher
{
	std::vector<uint8_t> m_buffer;
	std::unordered_map<std::string, msgpackpp::procedurecall> m_method_map;

public:
	void push_bytes(const char *bytes, size_t n);

	void add_method(const std::string &method_name, 
            const msgpackpp::procedurecall &proc);

private:
	void redraw(const msgpackpp::parser &args);
	void consume();
};
