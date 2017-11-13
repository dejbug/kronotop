#include "scinti.h"

#include <assert.h>
#include <string>
#include <sstream>
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

void dejlib3::scinti::Scintilla::get_column_counts(std::vector<long> & vv) const
{
	SIZE s;
	dejlib3::win::get_client_size(handle, s);

	long const row_height = smsg(SCI_TEXTHEIGHT, 0);
	// long const row_count = get_row_count();
	long const row_count = smsg(SCI_LINESONSCREEN);

	for (long row=0; row < row_count; ++row)
	{
		RowInfo ri(*this, row);

		long const y = row * row_height;

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

std::string dejlib3::scinti::RowInfo::to_string() const
{
	std::stringstream ss;
	ss << "RowInfo(row=" << row << ", line=" << line << ", rows=" << rows << ", index=" << index << ")";
	return ss.str();
}

dejlib3::scinti::RowInfo2::RowInfo2(dejlib3::scinti::Scintilla const & sci, long row) :
	dejlib3::scinti::RowInfo(sci, row),
	is_last_index(index == rows - 1),
	has_wrap_flag_start(index > 0 && index < rows ? sci.smsg(SCI_GETWRAPVISUALFLAGS) == SC_WRAPVISUALFLAG_START : false),
	has_wrap_flag_end(rows > 1 && index < rows - 1 ? sci.smsg(SCI_GETWRAPVISUALFLAGS) == SC_WRAPVISUALFLAG_END : false)
{}

std::string dejlib3::scinti::RowInfo2::to_string() const
{
	std::stringstream ss;
	ss << "RowInfo2(row=" << row << ", line=" << line << ", rows=" << rows << ", index=" << index << ", is_last_index=" << is_last_index << ", has_wrap_flag_start=" << has_wrap_flag_start << ", has_wrap_flag_end=" << has_wrap_flag_end << ")";
	return ss.str();
}
