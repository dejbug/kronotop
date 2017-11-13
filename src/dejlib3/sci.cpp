#include "sci.h"
#include "win.h"
#include "dbg.h"

#include <Scintilla.h>
#include <SciLexer.h>

#include <sstream>
#include <algorithm>

#include "scinti.h"

namespace dejlib3 {
namespace sci {

// 	some nice RGB colors:
// 	"#3f3f3f", (63, 63, 63),
// 	"#af3f3f", (175, 63, 63),
// 	"#8B8B83", (139, 139, 131),
// 	"#E37170", (227, 113, 112),
// 	"#7F9F7F", (127, 159, 127),
// 	"#008000", (0, 128, 0),
// 	"#CCAA33", (204, 170, 51),
// 	"#CF6A4C", (207, 106, 76),
// 	"#DDF2A4", (221, 242, 164),

Sci Sci::from_handle(HWND h)
{
	Sci sci;
	sci.handle = h;
	sci.defproc = (WNDPROC) GetProp(h, "sci.defproc");
	return sci;
}

HWND Sci::create(HWND parent, UINT id)
{
	handle = dejlib3::win::create_window(
		"Scintilla", parent, id,
		WS_CHILD|WS_CLIPCHILDREN);
	// assert(NULL != handle);

	defproc = (WNDPROC) GetWindowLong(handle, GWL_WNDPROC);
	// assert(NULL != defproc);
	SetProp(handle, "sci.defproc", (HANDLE) defproc);

	return handle;
}

void Sci::set_defaults()
{
	smsg(SCI_CLEARALL);
	smsg(EM_EMPTYUNDOBUFFER);

	/// Docs say we should disable this for newer
	/// platforms. I'm on WinXP and it works much
	/// better with double-buffering turned on.
	smsg(SCI_SETBUFFEREDDRAW, 1);
	smsg(SCI_SETPHASESDRAW, SC_PHASES_MULTIPLE);
	// smsg(SCI_SETTECHNOLOGY, SC_TECHNOLOGY_DEFAULT);
	// smsg(SCI_SETFONTQUALITY, SC_EFF_QUALITY_ANTIALIASED);

	// smsg(SCI_SETLEXER, SCLEX_CONTAINER);
	smsg(SCI_SETLEXER, SCLEX_NULL);

	smsg(SCI_SETCARETPERIOD, 0);
	// smsg(SCI_SETCARETSTYLE, CARETSTYLE_INVISIBLE);

	/// Line wrapping.

	smsg(SCI_SETWRAPMODE, SC_WRAP_CHAR);
	smsg(SCI_SETWRAPVISUALFLAGS, SC_WRAPVISUALFLAG_END);
	smsg(SCI_SETWRAPVISUALFLAGSLOCATION, SC_WRAPVISUALFLAGLOC_DEFAULT);
	smsg(SCI_SETWRAPSTARTINDENT, 0);
	// smsg(SCI_SETLAYOUTCACHE, SC_CACHE_NONE);
	smsg(SCI_SETLAYOUTCACHE, SC_CACHE_DOCUMENT);

	smsg(SCI_SETMARGINWIDTHN, 1, 0);
	// smsg(SCI_SETVSCROLLBAR, false);

	smsg(SCI_SETCARETFORE, 0x3f3faf);
	smsg(SCI_STYLESETBACK, STYLE_DEFAULT, 0x3f3f3f);
	smsg(SCI_STYLESETFORE, STYLE_DEFAULT, 0x838B8B);

	smsg(SCI_STYLESETEOLFILLED, STYLE_DEFAULT, true);
	smsg(SCI_STYLESETSIZE, STYLE_DEFAULT, 16);
	smsg(SCI_STYLESETFONT, STYLE_DEFAULT, (LPARAM) "Lucida Console");

	smsg(SCI_STYLECLEARALL);

	// smsg(SCI_SETMARGINLEFT, 0, 32);
	// smsg(SCI_SETMARGINRIGHT, 0, 32);
	smsg(SCI_SETCARETWIDTH, 3);
	smsg(SCI_SETTABWIDTH, 4);
	// smsg(SCI_SETUSETABS, 0);

	smsg(SCI_SETMARGINOPTIONS, SC_MARGINOPTION_SUBLINESELECT);
}

void Sci::show()
{
	UpdateWindow(handle);
	ShowWindow(handle, SW_SHOW);
}

LRESULT Sci::smsg(UINT m, WPARAM w, LPARAM l)
{
	return SendMessage(handle, m, w, l);
}

WNDPROC Sci::subclass(WNDPROC proc)
{
	return (WNDPROC)SetWindowLong(handle, GWL_WNDPROC, (LONG)proc);
}

LRESULT Sci::calldef(UINT m, WPARAM w, LPARAM l)
{
	return CallWindowProc(defproc, handle, m, w, l);
}

void Sci::get_row_widths(std::vector<long> & yy)
{
	long const cw = smsg(SCI_TEXTWIDTH, STYLE_DEFAULT, (LPARAM) "X");
	long const extra = 3;

	scinti::Scintilla(handle).get_column_counts(yy);
	// DUMP_VEC("%ld", yy);

	std::for_each(yy.begin(), yy.end(), [&](long & y){
		y = y * cw + (y > 0 ? extra : -1);
	});
}

void Sci::get_last_wrapped_rows_y(std::vector<long> & yy)
{
	bool const all_lines = true;

	long const rh = smsg(SCI_TEXTHEIGHT);
	long const rc = smsg(SCI_LINESONSCREEN);
	long const fvr = smsg(SCI_GETFIRSTVISIBLELINE);

	long ly = 0;

	for (long i=fvr; i < fvr + rc; ++i)
	{
		row_info ri(*this, i);

		// std::string s;
		// ri.print(s);
		// printf("%s\n", s.c_str());

		if(all_lines || ri.rows > 1)
		{
			int const y = rh * (ri.rows - (fvr - ri.first_row));
			if(y != ly) yy.push_back(y);
			ly = y;
		}
	}
}

void Sci::draw_col_separators(std::vector<long> const & xx)
{
	COLORREF const f = RGB(133, 166, 250);

	auto dc = dejlib3::win::Dc::for_client(handle);
	dc.set_pen_color(f);

	long const th = smsg(SCI_TEXTHEIGHT);

	long y = 0;

	for (size_t i=0; i<xx.size(); ++i, y += th)
	{
		dejlib3::win::draw_line(dc.handle, {xx[i], y}, {xx[i], y + th});
	}
}

void Sci::clip_col_separators(std::vector<long> const & xx)
{
	long const th = smsg(SCI_TEXTHEIGHT);
	long y = 0;

	for (size_t i=0; i<xx.size(); ++i, y += th)
	{
		RECT const r{xx[i], y, xx[i]+1, y + th};
		ValidateRect(handle, &r);
	}
}

void Sci::draw_row_separators(std::vector<long> const & yy)
{
	COLORREF const f = smsg(SCI_STYLEGETFORE);

	SIZE s;
	dejlib3::win::get_client_size(handle, s);

	auto dc = dejlib3::win::Dc::for_client(handle);
	dc.set_pen_color(f);

	for(long y : yy)
	{
		dejlib3::win::draw_line(dc.handle, {0, y-1}, {s.cx, y-1});
	}
}

void Sci::clip_row_separators(std::vector<long> const & yy)
{
	SIZE s;
	dejlib3::win::get_client_size(handle, s);

	for(long y : yy)
	{
		RECT const r{0, y-1, s.cx, y};
		ValidateRect(handle, &r);
	}
}

void get_row_positions(Sci & sci, std::vector<long> & vv)
{
	scinti::Scintilla(sci.handle).get_column_counts(vv);
}

long row_from_pos(Sci & sci, long pos)
{
	std::vector<long> pp;
	get_row_positions(sci, pp);

	long row = 0;
	for (long p : pp)
	{
		if (p >= pos)
			return row - 1;
		++row;
	}

	return 0;
}

long col_from_pos(Sci & sci, long pos)
{
	std::vector<long> pp;
	get_row_positions(sci, pp);

	long col = pos;
	long row = 0;
	for (long p : pp)
	{
		if (p >= pos)
			return col;
		col -= p;
		++row;
	}

	return 0;
}

void Sci::draw_cursor()
{
	/// What we've got is the current cursor pos. The pos is
	/// the index of the character besides which to draw the
	/// cursor. What we need to know are the row and column.
	/// ...

	std::vector<long> pp;
	get_row_positions(*this, pp);



	return;

	// long const row_count = smsg(SCI_LINESONSCREEN);
	// long const fvr = smsg(SCI_GETFIRSTVISIBLELINE);
	long const cur = smsg(SCI_GETCURRENTPOS);
	// long const line = smsg(SCI_LINEFROMPOSITION, cur);
	long const cw = smsg(SCI_TEXTWIDTH, STYLE_DEFAULT, (LPARAM) "X");
	long const ch = smsg(SCI_TEXTHEIGHT, 0);
	// long const col = smsg(SCI_GETCOLUMN, cur);
	long const col = col_from_pos(*this, cur);
	// long const cury = smsg(SCI_POINTYFROMPOSITION, cur);
	long const row = row_from_pos(*this, cur);
	long const x = col * cw;
	long const y = row * ch;

	printf("cur %ld col %ld row %ld x %ld y %ld\n", cur, col, row, x, y);

	COLORREF const f = RGB(0xaf, 0x3f, 0x3f); // 0x003f3faf;

	auto dc = dejlib3::win::Dc::for_client(handle);
	dc.set_pen_color(f);

	dejlib3::win::draw_line(dc.handle, {x, y}, {x, y+ch});
}

long Sci::get_line_char_count(int line, bool include_eol)
{
	if (!include_eol)
	{
		long const b = smsg(SCI_POSITIONFROMLINE, line);
		long const e = smsg(SCI_GETLINEENDPOSITION, line);
		return e - b;
	}
	return smsg(SCI_LINELENGTH, line);
}

void Sci::print_notification_label(std::string & out, UINT code)
{
	switch(code)
	{
		case 2000: out = "SCN_STYLENEEDED"; break;
		case 2001: out = "SCN_CHARADDED"; break;
		case 2002: out = "SCN_SAVEPOINTREACHED"; break;
		case 2003: out = "SCN_SAVEPOINTLEFT"; break;
		case 2004: out = "SCN_MODIFYATTEMPTRO"; break;
		case 2005: out = "SCN_KEY"; break;
		case 2006: out = "SCN_DOUBLECLICK"; break;
		case 2007: out = "SCN_UPDATEUI"; break;
		case 2008: out = "SCN_MODIFIED"; break;
		case 2009: out = "SCN_MACRORECORD"; break;
		case 2010: out = "SCN_MARGINCLICK"; break;
		case 2011: out = "SCN_NEEDSHOWN"; break;
		case 2013: out = "SCN_PAINTED"; break;
		case 2014: out = "SCN_USERLISTSELECTION"; break;
		case 2015: out = "SCN_URIDROPPED"; break;
		case 2016: out = "SCN_DWELLSTART"; break;
		case 2017: out = "SCN_DWELLEND"; break;
		case 2018: out = "SCN_ZOOM"; break;
		case 2019: out = "SCN_HOTSPOTCLICK"; break;
		case 2020: out = "SCN_HOTSPOTDOUBLECLICK"; break;
		case 2021: out = "SCN_CALLTIPCLICK"; break;
		case 2022: out = "SCN_AUTOCSELECTION"; break;
		case 2023: out = "SCN_INDICATORCLICK"; break;
		case 2024: out = "SCN_INDICATORRELEASE"; break;
		case 2025: out = "SCN_AUTOCCANCELLED"; break;
		case 2026: out = "SCN_AUTOCCHARDELETED"; break;
		case 2027: out = "SCN_HOTSPOTRELEASECLICK"; break;
		case 2028: out = "SCN_FOCUSIN"; break;
		case 2029: out = "SCN_FOCUSOUT"; break;
		default: out = "?"; break;
	}
}

row_info::row_info(Sci & sci, long row) : row(row)
{
	long const lc = sci.smsg(SCI_GETLINECOUNT);

	line = sci.smsg(SCI_DOCLINEFROMVISIBLE, row);

	if (line >= lc) { line = lc; rows = 0; }
	else rows = sci.smsg(SCI_WRAPCOUNT, line);

	first_row = sci.smsg(SCI_VISIBLEFROMDOCLINE, line);
	last_row = first_row + rows - 1;
}

void row_info::print(std::string & out)
{
	std::ostringstream ss;
	ss << "row: " << row << " | " << "rows: " << rows << " | " << "line: " << line << " | " << "first_row: " << first_row << " | " << "last_row: " << last_row;
	ss.str().swap(out);
}

} // :: sci
} // :: dejlib3
