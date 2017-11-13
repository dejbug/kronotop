#ifndef SCINTI_HPP
#define SCINTI_HPP

#include <windows.h>
#include <string>
#include <vector>

#include <dejlib3/win.h>

#include <Scintilla.h>

namespace dejlib3 {
namespace scinti {

struct Scintilla
{
	HWND const handle;

	Scintilla(HWND handle);

	LRESULT smsg(UINT m, WPARAM w=0, LPARAM l=0) const;

	long get_row_count() const;
	void get_column_counts(std::vector<long> & vv) const;
};

struct RowInfo
{
	long const row = 0;
	long const line = 0;
	long const rows = 0;
	long const index = 0;

	RowInfo(Scintilla const & sci, long row);
	std::string to_string() const;
};

struct RowInfo2 : public RowInfo
{
	bool is_last_index = false;
	bool has_wrap_flag_start = false;
	bool has_wrap_flag_end = false;

	RowInfo2(Scintilla const & sci, long row);
	std::string to_string() const;
};

} // namespace scinti
} // namespace dejlib3

#endif // SCINTI_HPP
