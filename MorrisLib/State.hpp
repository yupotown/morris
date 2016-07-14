#pragma once

#include <map>

namespace Morris {

	struct State {

		/// <summary>
		/// 状態の表現
		/// | 未使用[3] | フェーズ[2] | ターン[1] | 後手(青)の直前の移動[4] | 先手(赤)の直前の移動[4] | 盤面[18] |
		/// </summary>
		int v;

		#pragma region フェーズ

		/// <summary>
		/// フェーズ(shift: 27)
		/// { 0: 置く, 1: 動かす(1ターン目), 2: 動かす(2ターン目), 3: 動かす(3ターン目以降) }
		/// </summary>
		enum Phase {

			/// <summary>
			/// 置くフェーズ
			/// </summary>
			put = 0,

			/// <summary>
			/// 動かすフェーズ(1ターン目)
			/// </summary>
			move1 = 1,

			/// <summary>
			/// 動かすフェーズ(2ターン目)
			/// </summary>
			move2 = 2,

			/// <summary>
			/// 動かすフェーズ(3ターン目以降)
			/// </summary>
			move3 = 3,

		};

		/// <summary>
		/// フェーズ
		/// </summary>
		Phase phase() const { return static_cast<Phase>((v >> 27) & 3); }

		/// <summary>
		/// フェーズ
		/// </summary>
		State phase(Phase ph) const {
			State res(v);
			res.v &= ~(3 << 27);
			res.v |= (static_cast<int>(ph) << 27);
			return res;
		}

		#pragma endregion

		#pragma region ターン(プレイヤー)

		/// <summary>
		/// ターン(プレイヤー)(shift: 26)
		/// { 0: 先手, 1: 後手 }
		/// </summary>
		enum Turn {

			/// <summary>
			/// 先手(赤)
			/// </summary>
			first = 0,

			/// <summary>
			/// 後手(青)
			/// </summary>
			second = 1,

		};
		typedef Turn Player;

		/// <summary>
		/// ターン
		/// </summary>
		Turn turn() const { return static_cast<Turn>((v >> 26) & 1); }

		/// <summary>
		/// ターン
		/// </summary>
		State turn(Turn t) const {
			State res(v);
			res.v &= ~(1 << 26);
			res.v |= (static_cast<int>(t) << 26);
			return res;
		}

		#pragma endregion

		#pragma region 頂点

		/// <summary>
		/// 頂点の位置
		/// </summary>
		struct Point {

			/// <summary>
			/// 頂点の位置
			/// { 00, 10, 20, 01, 11, 21, 02, 12, 22 }
			/// </summary>
			int v;

			Point() = default;
			Point(int v) : v(v) {}
			Point(int x, int y) : v(y * 3 + x) {}

			int x() const { return v % 3; }
			int y() const { return v / 3; }

			void set(int x, int y) { v = y * 3 + x; }

			operator int() const { return v; }
			const Point &operator =(int rhs) { v = rhs; return *this; }
			const Point &operator =(const Point &rhs) { v = rhs.v; return *this; }
			bool operator ==(const Point &rhs) const { return v == rhs.v; }
			bool operator !=(const Point &rhs) const { return v != rhs.v; }

		};

		/// <summary>
		/// 頂点(shift: 0, 2, 4, ..., 16)
		/// { 0: 何も置いていない, 1: 先手のコマ, 2: 後手のコマ, 3: 未使用 }
		/// </summary>
		enum Vertex {

			/// <summary>
			/// 何も置いていない頂点
			/// </summary>
			blank = 0,

			/// <summary>
			/// 先手のコマが置いてある頂点
			/// </summary>
			red = 1,

			/// <summary>
			/// 後手のコマが置いてある頂点
			/// </summary>
			blue = 2,

			/// <summary>
			/// 頂点(未使用)
			/// </summary>
			invalid_vertex = 3,

		};

		int vertexShift(const Point &pt) const { return pt.v * 2; }

		/// <summary>
		/// 頂点
		/// </summary>
		Vertex vertex(const Point &pt) const { return static_cast<Vertex>((v >> vertexShift(pt)) & 3); }

		/// <summary>
		/// 頂点
		/// </summary>
		State vertex(const Point &pt, const Vertex &vx) const {
			State res(v);
			res.v &= ~(3 << vertexShift(pt));
			res.v |= (static_cast<int>(vx) << vertexShift(pt));
			return res;
		}

		#pragma endregion

		#pragma region 直前の移動(辺)

		/// <summary>
		/// 辺(直前の移動)(shift: 18, 22)
		/// { 00-10, 00-11, 10-20, 10-11, 20-21, 20-11, 21-22, 21-11,
		///   22-12, 22-11, 12-02, 12-11, 02-01, 02-11, 01-00, 01-11 }
		/// </summary>
		enum Edge {
			e0010 = 0, e1000 = 0,
			e0011 = 1, e1100 = 1,
			e1020 = 2, e2010 = 2,
			e1011 = 3, e1110 = 3,
			e2021 = 4, e2120 = 4,
			e2011 = 5, e1120 = 5,
			e2122 = 6, e2221 = 6,
			e2111 = 7, e1121 = 7,
			e2212 = 8, e1222 = 8,
			e2211 = 9, e1122 = 9,
			e1202 = 10, e0212 = 10,
			e1211 = 11, e1112 = 11,
			e0201 = 12, e0102 = 12,
			e0211 = 13, e1102 = 13,
			e0100 = 14, e0001 = 14,
			e0111 = 15, e1101 = 15,
		};
		typedef Edge LastMove;

		int lastMoveShift(Player pl) const {
			return pl == first ? 18 : 22;
		}

		/// <summary>
		/// 直前の移動
		/// </summary>
		LastMove lastMove(Player pl) const { return static_cast<LastMove>((v >> lastMoveShift(pl)) & 15); }

		/// <summary>
		/// 直前の移動
		/// </summary>
		State lastMove(Player pl, LastMove lm) const {
			State res(v);
			res.v &= ~(15 << lastMoveShift(pl));
			res.v |= (static_cast<int>(lm) << lastMoveShift(pl));
			return res;
		}

		/// <summary>
		/// 辺の両端
		/// </summary>
		struct EdgeEnds {
			Point a, b;

			EdgeEnds() = default;
			EdgeEnds(const Point &a, const Point &b) : a(a), b(b) {}
			EdgeEnds(const EdgeEnds &obj) : a(obj.a), b(obj.b) {}

			const EdgeEnds &operator =(const EdgeEnds &rhs) {
				a = rhs.a;
				b = rhs.b;
			}
			bool operator ==(const EdgeEnds &rhs) const { return a == rhs.a && b == rhs.b; }
			bool operator !=(const EdgeEnds &rhs) const { return a != rhs.a || b != rhs.b; }
		};

		constexpr static int exTbl[9] = { 0, 1, 2, 2, 2, 1, 0, 0, 0 };
		constexpr static int eyTbl[9] = { 0, 0, 0, 1, 2, 2, 2, 1, 0 };

		/// <summary>
		/// 辺の両端
		/// </summary>
		static EdgeEnds edgeEnds(Edge e) {
			const int en = static_cast<int>(e);
			const int d = en / 2, m = en % 2;
			EdgeEnds res;
			res.a = { exTbl[d], eyTbl[d] };
			if (m == 0) res.b = { exTbl[d + 1], eyTbl[d + 1] };
			else res.b = { 1, 1 };
			return res;
		}

		#pragma endregion

		State() = default;
		State(const State &obj) : v(obj.v) {}
		State(int v) : v(v) {}

		operator int() const { return v; }
		const State &operator =(int rhs) { v = rhs; }
		const State &operator =(const State &rhs) { v = rhs.v; return *this; }

		#pragma region 回転と反転

		#define _morris_state_combine_tbl(t2, t1) \
			{ \
				t2[t1[0]], t2[t1[1]], t2[t1[2]], t2[t1[3]], \
				t2[t1[4]], t2[t1[5]], t2[t1[6]], t2[t1[7]], \
				t2[t1[8]], t2[t1[9]], t2[t1[10]], t2[t1[11]], \
				t2[t1[12]], t2[t1[13]], t2[t1[14]], t2[t1[15]], \
			}

		constexpr static Edge tblRot90[16] = {
			e2021, e2011, e2122, e2111, e2212, e2211, e1202, e1211,
			e0201, e0211, e0100, e0111, e0010, e0011, e1020, e1011,
		};
		constexpr static Edge tblRot180[16] = _morris_state_combine_tbl(tblRot90, tblRot90);
		constexpr static Edge tblRot270[16] = _morris_state_combine_tbl(tblRot180, tblRot90);
		constexpr static Edge tblRot0[16] = _morris_state_combine_tbl(tblRot180, tblRot180);
		constexpr static Edge tblRev0121[16] = {
			e1202, e0211, e2212, e1211, e2122, e2211, e2021, e2111,
			e1020, e2011, e0010, e1011, e0100, e0011, e0201, e0111,
		};
		constexpr static Edge tblRev0022[16] = _morris_state_combine_tbl(tblRot90, tblRev0121);
		constexpr static Edge tblRev1012[16] = _morris_state_combine_tbl(tblRot180, tblRev0121);
		constexpr static Edge tblRev2002[16] = _morris_state_combine_tbl(tblRot270, tblRev0121);

		#undef _morris_state_combine_tbl

		/// <summary>
		/// 何もしない。
		/// </summary>
		State rot0() const;

		/// <summary>
		/// 盤面を90度回転する。
		/// </summary>
		State rot90() const;

		/// <summary>
		/// 盤面を180度回転する。
		/// </summary>
		State rot180() const;

		/// <summary>
		/// 盤面を270度回転する。
		/// </summary>
		State rot270() const;

		/// <summary>
		/// 01-21 を軸に反転(上下反転)する。
		/// </summary>
		State rev0121() const;

		/// <summary>
		/// 00-22 を軸に反転する。
		/// </summary>
		State rev0022() const;

		/// <summary>
		/// 10-12 を軸に反転(左右反転)する。
		/// </summary>
		State rev1012() const;

		/// <summary>
		/// 20-02 を軸に反転する。
		/// </summary>
		State rev2002() const;

		static constexpr State (State::*allRotRev[8])() const = { &rot0, &rot90, &rot180, &rot270, &rev0121, &rev0022, &rev1012, &rev2002 };

		/// <summary>
		/// 回転や反転を適用した盤面のうち状態が最小のものを取得する。
		/// </summary>
		State minimize() const;

		#pragma endregion

		#pragma region 勝ち負け

		/// <summary>
		/// 勝者(3つ並んでいるコマ)
		/// まだ決着がついていない場合は blank を返す。
		/// </summary>
		Vertex winner() const;

		static std::map<State, Vertex> winMap;

		#pragma endregion

	};

}
