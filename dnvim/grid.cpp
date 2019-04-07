#include "grid.h"
#include <plog/Log.h>
#include <codecvt>


void Grid::option_set(std::string key, bool enable) 
{
}


void Grid::default_colors_set(int rgb_fg, 
	int rgb_bg, int rgb_sp,
	int cterm_fg, int cterm_bg) 
{

}

void Grid::update_fg(int color) 
{
}

void Grid::update_bg(int color) 
{
}

void Grid::update_sp(int color) 
{
}

void Grid::resize(int cols, int rows) 
{
	m_cells.resize(cols * rows);
	m_cols = cols;
}

void Grid::clear()
{
	for (auto &cell : m_cells)
	{
		cell.code = 0; // space
	}
}

void Grid::eol_clear()
{
	auto cell = get_current_cell();
	if (!cell) {
		return;
	}

	for (int i = m_cursor.col; i < m_cols; ++i)
	{
		cell->code = 0;
	}
}

void Grid::cursor_goto(int row, int col) 
{
	m_cursor.col = col;
	m_cursor.row = row;
}

void Grid::highlight_set(Highlight hl) 
{
}

void Grid::put(std::string_view text)
{
	auto cell = get_current_cell();
	if (!cell) {
		return;
	}

	std::wstring_convert<std::codecvt_utf8<std::uint32_t>, std::uint32_t> conv;
	auto unicode = conv.from_bytes(text); // UTF8 -> UTF32

	for (char32_t c : unicode)
	{
		cell->code = c;
		cell = advance_cursor();
	}
}

void Grid::mode_info_set(bool cursor_style_enabled, msgpackpp::parser map)
{
    //std::cout << "mode_info_set: " << map;
}

void Grid::mode_change(std::string mode, int value)
{

}
