#include "grid.h"
#include <plog/Log.h>


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
	LOGD << "resize: " << cols << ", " << rows;
	m_cells.resize(cols * rows);
	m_cols = cols;
}

void Grid::clear()
{
	LOGD << "clear";
	for (auto &cell : m_cells)
	{
		cell.code = 0; // space
	}
}

void Grid::cursor_goto(int col, int row) 
{
	LOGD << "cursor_goto: " << col << ", " << row;
	m_cursor.col = col;
	m_cursor.row = row;
}

void Grid::highlight_set(Highlight hl) 
{
}

void Grid::put(std::string text)
{
	LOGD << "put: '" << text << "'";
	auto cell = get_current_cell();
	for (char c : text)
	{
		cell->code = c;
		++cell;
	}
}

void Grid::mode_info_set(bool cursor_style_enabled, msgpackpp::parser map)
{
    //std::cout << "mode_info_set: " << map;
}

void Grid::mode_change(std::string mode, int value)
{

}
