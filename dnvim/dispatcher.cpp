#include "dispatcher.h"
#include <plog/Log.h>


void Dispatcher::push_bytes(const char *bytes, size_t n)
{
    auto pos = m_buffer.size();
    m_buffer.resize(pos + n);
    for (int i = 0; i < n; ++i, ++pos)
    {
        m_buffer[pos] = bytes[i];
    }

    while (!m_buffer.empty()) {
        consume();
    }
}

void Dispatcher::add_method(const std::string &method_name, 
        const msgpackpp::procedurecall &proc)
{
    m_method_map.insert(std::make_pair(method_name, proc));
}

void Dispatcher::redraw(const msgpackpp::parser &args)
{
    if (args.is_array()) {
        LOGD << "[redraw] " << args.count();
        auto child = args[0];
        for (int i = 0; i < args.count(); ++i) {
            
            auto cmd = child[0].get_string();
			LOGD << "[" << i << "] " << child.to_json();

            auto found = m_method_map.find(cmd);
            if (found != m_method_map.end()) {
                // found
                auto proc = found->second;

				for (int i = 1; i < child.count(); ++i) {
					try {
						proc(child[i]);
					}
					catch (const std::exception & ex)
					{
						LOGE << ex.what() << " " << child[1];
					}
				}
            }
            else {
                LOGE << ", unknown " << cmd;
            }

            child = child.next();
        }
    }
    else {

        LOGE << "redraw not array";

    }
}

void Dispatcher::consume()
{
    auto msg = msgpackpp::parser(m_buffer.data(), static_cast<int>(m_buffer.size()));
    auto d = msg.consumed_size();
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

		LOGW
			<< "response: " << msg.to_json()
            ;
    }
    else if (msgType == 2) {
        // notify
        auto method = msg[1].get_string();

        if (method == "redraw") {

            redraw(msg[2]);

        }
        else {

            LOGE
                << "notify: " << msg.to_json()
                ;
        }

    }
    else {
        throw std::exception("invali msgType");
    }
}

