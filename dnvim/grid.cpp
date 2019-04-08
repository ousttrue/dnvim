#include "grid.h"
#include <plog/Log.h>
#include <codecvt>


// ["option_set",["arabicshape",true],["ambiwidth","single"],["emoji",true],["guifont",""],["guifontset",""],["guifontwide",""],["linespace",0],["pumblend",0],["showtabline",1],["termguicolors",false],["ext_linegrid",false],["ext_multigrid",false],["ext_hlstate",false],["ext_termcolors",false]]
// ["option_set", ["ext_cmdline", false], ["ext_popupmenu", false], ["ext_tabline", false], ["ext_wildmenu", false], ["ext_messages", false]]
void Grid::option_set(std::string key, msgpackpp::parser enable)
{
}

// ["default_colors_set", [1.67772e+07, 0, 1.67117e+07, 0, 0]]
void Grid::default_colors_set(int rgb_fg, 
	int rgb_bg, int rgb_sp,
	int cterm_fg, int cterm_bg) 
{

}

// ["update_fg",[1.67772e+07]]
void Grid::update_fg(int color) 
{
}

// ["update_bg", [0]]
void Grid::update_bg(int color) 
{
}

// ["update_sp",[1.67117e+07]]
void Grid::update_sp(int color) 
{
}

// ["resize", [80, 20]]
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

// ["highlight_set", [{"italic":true, "foreground" : 4.60911e+06}]]
void Grid::highlight_set(Highlight hl) 
{
}

// ["put", ["<"], ["E"], ["n"], ["t"], ["e"], ["r"], [">"]]
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

// ["mode_info_set",[true,[{"mouse_shape":0,"cursor_shape":"block","cell_percentage":0,"blinkwait":0,"blinkon":0,"blinkoff":0,"hl_id":0,"id_lm":0,"attr_id":0,"attr_id_lm":0,"name":"normal","short_name":"n"},{"mouse_shape":0,"cursor_shape":"block","cell_percentage":0,"blinkwait":0,"blinkon":0,"blinkoff":0,"hl_id":0,"id_lm":0,"attr_id":0,"attr_id_lm":0,"name":"visual","short_name":"v"},{"mouse_shape":0,"cursor_shape":"vertical","cell_percentage":25,"blinkwait":0,"blinkon":0,"blinkoff":0,"hl_id":0,"id_lm":0,"attr_id":0,"attr_id_lm":0,"name":"insert","short_name":"i"},{"mouse_shape":0,"cursor_shape":"horizontal","cell_percentage":20,"blinkwait":0,"blinkon":0,"blinkoff":0,"hl_id":0,"id_lm":0,"attr_id":0,"attr_id_lm":0,"name":"replace","short_name":"r"},{"mouse_shape":0,"cursor_shape":"block","cell_percentage":0,"blinkwait":0,"blinkon":0,"blinkoff":0,"hl_id":0,"id_lm":0,"attr_id":0,"attr_id_lm":0,"name":"cmdline_normal","short_name":"c"},{"mouse_shape":0,"cursor_shape":"vertical","cell_percentage":25,"blinkwait":0,"blinkon":0,"blinkoff":0,"hl_id":0,"id_lm":0,"attr_id":0,"attr_id_lm":0,"name":"cmdline_insert","short_name":"ci"},{"mouse_shape":0,"cursor_shape":"horizontal","cell_percentage":20,"blinkwait":0,"blinkon":0,"blinkoff":0,"hl_id":0,"id_lm":0,"attr_id":0,"attr_id_lm":0,"name":"cmdline_replace","short_name":"cr"},{"mouse_shape":0,"cursor_shape":"horizontal","cell_percentage":20,"blinkwait":0,"blinkon":0,"blinkoff":0,"hl_id":0,"id_lm":0,"attr_id":0,"attr_id_lm":0,"name":"operator","short_name":"o"},{"mouse_shape":0,"cursor_shape":"vertical","cell_percentage":25,"blinkwait":0,"blinkon":0,"blinkoff":0,"hl_id":0,"id_lm":0,"attr_id":0,"attr_id_lm":0,"name":"visual_select","short_name":"ve"},{"mouse_shape":0,"name":"cmdline_hover","short_name":"e"},{"mouse_shape":0,"name":"statusline_hover","short_name":"s"},{"mouse_shape":0,"name":"statusline_drag","short_name":"sd"},{"mouse_shape":0,"name":"vsep_hover","short_name":"vs"},{"mouse_shape":0,"name":"vsep_drag","short_name":"vd"},{"mouse_shape":0,"name":"more","short_name":"m"},{"mouse_shape":0,"name":"more_lastline","short_name":"ml"},{"cursor_shape":"block","cell_percentage":0,"blinkwait":0,"blinkon":0,"blinkoff":0,"hl_id":0,"id_lm":0,"attr_id":0,"attr_id_lm":0,"name":"showmatch","short_name":"sm"}]]]
void Grid::mode_info_set(bool cursor_style_enabled, msgpackpp::parser map)
{
    //std::cout << "mode_info_set: " << map;
}

// ["mode_change",["normal",0]]
// ["mode_change",["insert",2]]
void Grid::mode_change(std::string mode, int value)
{

}
