#pragma once
#include <windows.h>
#include <vector>
#include <string>

namespace dejlib3 {
namespace sci {

struct Sci
{
	HWND handle = NULL;
	WNDPROC defproc = NULL;

	static Sci from_handle(HWND);

	HWND create(HWND parent, UINT id);
	void set_defaults();
	void show();

	LRESULT smsg(UINT, WPARAM=0, LPARAM=0);
	WNDPROC subclass(WNDPROC);
	LRESULT calldef(UINT, WPARAM, LPARAM);

	void get_row_widths(std::vector<long> &);
	void draw_col_separators(std::vector<long> const &);
	void clip_col_separators(std::vector<long> const &);

	void get_last_wrapped_rows_y(std::vector<long> &);
	void draw_row_separators(std::vector<long> const &);
	void clip_row_separators(std::vector<long> const &);

	void draw_cursor();

	long get_line_char_count(int line, bool include_eol=false);

	void print_notification_label(std::string &, UINT);

};

struct row_info
{
	long row;
	long line;
	long rows;
	long first_row;
	long last_row;

	row_info(Sci &, long row);

	void print(std::string &);
};

} // :: sci
} // :: dejlib3
