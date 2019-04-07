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
	int m_cols = 0;
	Cursor m_cursor;
	bool m_flush = false;

	Cell* get_current_cell()
	{
		auto index = m_cursor.row * m_cols + m_cursor.col;
		if (index < 0 || index >= m_cells.size()) {
			return nullptr;
		}
		return &m_cells[index];
	}

	Cell* advance_cursor()
	{
		++m_cursor.col;
		if (m_cursor.col >= m_cols)
		{
			m_cursor.col = 0;
			++m_cursor.row;
		}
		return get_current_cell();
	}

public:
	const std::vector<Cell> &get_cells()const { return m_cells; }
	int get_cols()const { return m_cols; }
	const Cursor &get_cursor()const { return m_cursor; }
	bool use_flush()
	{
		if (!m_flush)return false;
		m_flush = false;
		return true;
	}

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
	void put(std::string_view text);
	void mode_info_set(bool cursor_style_enabled, msgpackpp::parser map);
	void mode_change(std::string mode, int value);
	void flush() { m_flush = true; }
	void eol_clear();
};
