#pragma once
#include <windows.h>


struct mkeys_t
{
	enum {
		MK_BUTTON_MASK = (MK_LBUTTON | MK_MBUTTON | MK_RBUTTON),
	};

	DWORD w;

	mkeys_t(DWORD w);

	bool is_button_down() const;
	int get_button_down_count() const;
	int get_button_up_count() const;
	int get_main_button_down_lmr() const;
	int get_main_button_down_lrm() const;
	int get_main_button_down_mlr() const;
	int get_main_button_down_mrl() const;
	int get_main_button_down_rlm() const;
	int get_main_button_down_rml() const;
	bool is_left_button_down() const;
	bool is_only_left_button_down() const;
	bool is_middle_button_down() const;
	bool is_only_middle_button_down() const;
	bool is_right_button_down() const;
	bool is_only_right_button_down() const;
	bool is_control_down() const;
	bool is_shift_down() const;
};
