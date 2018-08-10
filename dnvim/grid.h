#include <string>
#include <msgpackpp.h>


struct Highlight
{
	bool bold = false;
	int foreground = 255;
};
MPPP_MAP_SERIALIZER(Highlight, 2, bold, foreground);


///
/// https://github.com/neovim/neovim/blob/master/runtime/doc/ui.txt
///
class Grid
{
public:
	void option_set(std::string key, bool enable);
	void default_colors_set(int i0, int i1, int i2, int i3, int i4);
	void update_fg(int g);
	void update_bg(int g);
	void update_sp(int g);
	void resize(int cols, int rows);
	void clear();
	void cursor_goto(int col, int row);
	void highlight_set(Highlight hl);
	void put(std::string str);
	void mode_info_set(bool cursor_style_enabled, msgpackpp::parser map);
	void mode_change(std::string mode, int value);
};

