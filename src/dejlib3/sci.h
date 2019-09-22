#pragma once
#include <windows.h>
#include <vector>
#include <string>
#include <ostream>

namespace dejlib3 {
namespace sci {

struct Sci
{
	HWND handle = NULL;
	WNDPROC defproc = NULL;

	static Sci from_handle(HWND);

	HWND create(HWND parent, UINT id);
	void set_defaults() const;
	void show() const;

	LRESULT smsg(UINT, WPARAM=0, LPARAM=0) const;
	WNDPROC subclass(WNDPROC) const;
	LRESULT calldef(UINT, WPARAM, LPARAM) const;

	long get_row_width(long row) const;
	void get_row_widths(std::vector<long> &) const;
	void draw_col_separators(std::vector<long> const &) const;
	void clip_col_separators(std::vector<long> const &) const;

	void get_last_wrapped_rows_y(std::vector<long> &) const;
	void draw_row_separators(std::vector<long> const &) const;
	void clip_row_separators(std::vector<long> const &) const;

	void draw_cursor() const;

	long get_line_char_count(int line, bool include_eol=false) const;

	void print_notification_label(std::string &, UINT) const;

};

struct row_info
{
	long row;
	long line;
	long rows;
	long first_row;
	long last_row;

	row_info(Sci const &, long row);

	void print(std::string &) const;
};

} // :: sci
} // :: dejlib3

std::ostream & operator<<(std::ostream & os, dejlib3::sci::row_info const & ri);
