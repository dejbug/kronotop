#include "scinti.h"

#include <assert.h>
#include <string>
#include <sstream>
#include <iostream>
#include <Scintilla.h>

dejlib3::scinti::Scintilla::Scintilla(HWND handle) : handle(handle)
{
}

LRESULT dejlib3::scinti::Scintilla::smsg(UINT m, WPARAM w, LPARAM l) const
{
	return SendMessage(handle, m, w, l);
}

long dejlib3::scinti::Scintilla::get_row_count() const
{
	long const lines = smsg(SCI_GETLINECOUNT);
	long const rows = smsg(SCI_LINESONSCREEN);

	long row = 0;

	for (; row < rows; ++row)
	{
		long const line = smsg(SCI_DOCLINEFROMVISIBLE, row);
		if (line >= lines) break;

		long const wc = smsg(SCI_WRAPCOUNT, line);
		assert(wc > 0);

		row += wc - 1;
	}

	return row;
}

long dejlib3::scinti::Scintilla::get_column_count(long row) const
{
	long const row_height = smsg(SCI_TEXTHEIGHT, 0);

	// dejlib3::scinti::RowInfo ri(*this, row);
	// std::cout << row << " | " << ri << std::endl;

	long const y = row * row_height;

	long const start_pos = smsg(SCI_CHARPOSITIONFROMPOINT, 0, y);
	long const start_col = smsg(SCI_GETCOLUMN, start_pos);
	// std::cout << "getColumnCount(" << row << ") :  start_pos=" << start_pos << "  start_col=" << start_col << std::endl;

	SIZE s; dejlib3::win::get_client_size(handle, s);
	// std::cout << s.cx << ':' << s.cy << std::endl;

	long const end_pos = smsg(SCI_CHARPOSITIONFROMPOINT, s.cx, y);
	long const end_col = smsg(SCI_GETCOLUMN, end_pos);

	return end_col - start_col;
}

void dejlib3::scinti::Scintilla::get_column_counts(std::vector<long> & vv) const
{
	SIZE s;
	dejlib3::win::get_client_size(handle, s);

	long const row_height = smsg(SCI_TEXTHEIGHT, 0);
	long const row_count = smsg(SCI_LINESONSCREEN);

	for (long row=0, y=0; row < row_count; ++row, y += row_height)
	{
		// RowInfo ri(*this, row);

		long const start_pos = smsg(SCI_CHARPOSITIONFROMPOINT, 0, y);
		long const start_col = smsg(SCI_GETCOLUMN, start_pos);

		long const end_pos = smsg(SCI_CHARPOSITIONFROMPOINT, s.cx, y);
		long const end_col = smsg(SCI_GETCOLUMN, end_pos);

		// printf("%ld - %ld | %ld -- %s\n", start_pos, end_pos, end_col - start_col, ri.to_string().c_str());

		vv.push_back(end_col - start_col);
	}
}

dejlib3::scinti::RowInfo::RowInfo(dejlib3::scinti::Scintilla const & sci, long row) :
	row(row),
	line(sci.smsg(SCI_DOCLINEFROMVISIBLE, row)),
	rows(sci.smsg(SCI_WRAPCOUNT, line)),
	index(row - sci.smsg(SCI_VISIBLEFROMDOCLINE, line))
{}

dejlib3::scinti::RowInfo2::RowInfo2(dejlib3::scinti::Scintilla const & sci, long row) :
	dejlib3::scinti::RowInfo(sci, row),
	is_last_index(index == rows - 1),
	has_wrap_flag_start(index > 0 && index < rows ? sci.smsg(SCI_GETWRAPVISUALFLAGS) == SC_WRAPVISUALFLAG_START : false),
	has_wrap_flag_end(rows > 1 && index < rows - 1 ? sci.smsg(SCI_GETWRAPVISUALFLAGS) == SC_WRAPVISUALFLAG_END : false)
{}

std::ostream & operator<<(std::ostream & os, dejlib3::scinti::RowInfo const & ri) {
	os << "RowInfo(row=" << ri.row << ", line=" << ri.line << ", rows=" << ri.rows << ", index=" << ri.index << ")";	
	return os;
}

std::ostream & operator<<(std::ostream & os, dejlib3::scinti::RowInfo2 const & ri) {
	os << "RowInfo2(row=" << ri.row << ", line=" << ri.line << ", rows=" << ri.rows << ", index=" << ri.index << ", is_last_index=" << ri.is_last_index << ", has_wrap_flag_start=" << ri.has_wrap_flag_start << ", has_wrap_flag_end=" << ri.has_wrap_flag_end << ")";
	return os;
}
