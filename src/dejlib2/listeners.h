#pragma once
#include <vector>

// turns this:
//
//		DEFINE_LISTENERS(mouse_tracker_move_i) mouse_listeners;
//
// into this:
//
//		std::vector<mouse_tracker_move_i*> move_listeners;
//
#define DEFINE_LISTENERS(tt) std::vector<tt*>
	

// turn this:
//
//		NOTIFY_LISTENERS(move_listeners)->on_mouse_move(x, y);
//
// into this:
//
//		for(mouse_tracker_move_i::::pvector::iterator
//				it=move_listeners.begin();
//				it < move_listeners.end();
//				++it)
//
//			(*it)->on_mouse_move(x, y);
//

#define NOTIFY_LISTENERS(ll) \
	for(auto it=ll.begin(); it < ll.end(); ++it) (*it)
