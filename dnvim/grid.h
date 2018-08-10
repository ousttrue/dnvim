#include <string>
#include <vector>
#include <memory>
#include <msgpackpp.h>


struct Highlight
{
	bool bold = false;
	int foreground = 255;
};
MPPP_MAP_SERIALIZER(Highlight, 2, bold, foreground);


struct Cell
{
	char32_t code;
};


struct Cursor
{
	int col=0;
	int row=0;
};


///
/// https://github.com/neovim/neovim/blob/master/runtime/doc/ui.txt
///
class Grid
{
	std::vector<Cell> m_cells;
	int m_cols=0;
	Cursor m_cursor;

	Cell* get_current_cell()
	{
		auto index = m_cursor.row * m_cols + m_cursor.col;
		return &m_cells[index];
	}

public:
	void option_set(std::string key, bool enable);
	void default_colors_set(
		int rgb_fg, int rgb_bg, int rgb_sp, 
		int cterm_fg, int cterm_bg);
	void update_fg(int color);
	void update_bg(int color);
	void update_sp(int color);
	void resize(int cols, int rows);
	void clear();
	void cursor_goto(int col, int row);
	void highlight_set(Highlight hl);
	void put(std::string text);
	void mode_info_set(bool cursor_style_enabled, msgpackpp::parser map);
	void mode_change(std::string mode, int value);
};
