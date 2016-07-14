
#include "State.hpp"

#define REP_VTX(x, y) for (int y = 0; y < 3; ++y) for (int x = 0; x < 3; ++x)

namespace Morris {

	#define _morris_state_rotrev(fx, fy, tbl) \
		State s(v); \
		REP_VTX(x, y) s = s.vertex({ fx, fy }, this->vertex({ x, y })); \
		const auto ph = s.phase(); \
		if (ph >= move2) s = s.lastMove(first, tbl[s.lastMove(first)]); \
		if (ph >= move3) s = s.lastMove(second, tbl[s.lastMove(second)]); \
		return s

	State State::rot0() const { _morris_state_rotrev(x, y, tblRot0); }
	State State::rot90() const { _morris_state_rotrev(2 - y, x, tblRot90); }
	State State::rot180() const { _morris_state_rotrev(2 - x, 2 - y, tblRot180); }
	State State::rot270() const { _morris_state_rotrev(y, 2 - x, tblRot270); }
	State State::rev0121() const { _morris_state_rotrev(x, 2 - y, tblRev0121); }
	State State::rev0022() const { _morris_state_rotrev(y, x, tblRev0022); }
	State State::rev1012() const { _morris_state_rotrev(2 - x, y, tblRev1012); }
	State State::rev2002() const { _morris_state_rotrev(2 - y, 2 - x, tblRev2002); }

	State State::minimize() const {
		int res = v;
		for (int i = 1; i < 8; ++i) {
			int temp = (this->*allRotRev[i])().v;
			if (temp < res) res = temp;
		}
		return State(res);
	}

	State::Vertex State::winner() const
	{
		if (winMap.count(v) != 0) return winMap[v];
		bool ps[4][3][3] = {};
		REP_VTX(x, y) ps[this->vertex({ x, y })][x][y] = true;
		State::Vertex res = State::blank;
		for (int i = State::red; i <= State::blue; ++i) {
			if ((ps[i][0][0] && ps[i][1][0] && ps[i][2][0])
				|| (ps[i][0][1] && ps[i][1][1] && ps[i][2][1])
				|| (ps[i][0][2] && ps[i][1][2] && ps[i][2][2])
				|| (ps[i][0][0] && ps[i][0][1] && ps[i][0][2])
				|| (ps[i][1][0] && ps[i][1][1] && ps[i][1][2])
				|| (ps[i][2][0] && ps[i][2][1] && ps[i][2][2])
				|| (ps[i][0][0] && ps[i][1][1] && ps[i][2][2])
				|| (ps[i][2][0] && ps[i][1][1] && ps[i][0][2])) {
				res = static_cast<State::Vertex>(i);
			}
		}
		winMap.insert(std::make_pair(v, res));
		return res;
	}

	std::map<State, State::Vertex> State::winMap;

}
