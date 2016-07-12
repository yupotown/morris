#pragma once

namespace Morris {

	struct State {

		/// <summary>
		/// ��Ԃ̕\��
		/// | ���g�p[3] | �t�F�[�Y[2] | �^�[��[1] | ���(��)�̒��O�̈ړ�[4] | ���(��)�̒��O�̈ړ�[4] | �Ֆ�[18] |
		/// </summary>
		int v;

#pragma region �t�F�[�Y

		/// <summary>
		/// �t�F�[�Y(shift: 27)
		/// { 0: �u��, 1: ������(1�^�[����), 2: ������(2�^�[����), 3: ������(3�^�[���ڈȍ~) }
		/// </summary>
		enum Phase {

			/// <summary>
			/// �u���t�F�[�Y
			/// </summary>
			put = 0,

			/// <summary>
			/// �������t�F�[�Y(1�^�[����)
			/// </summary>
			move1 = 1,

			/// <summary>
			/// �������t�F�[�Y(2�^�[����)
			/// </summary>
			move2 = 2,

			/// <summary>
			/// �������t�F�[�Y(3�^�[���ڈȍ~)
			/// </summary>
			move3 = 3,

		};

		/// <summary>
		/// �t�F�[�Y
		/// </summary>
		inline Phase phase() const { return static_cast<Phase>((v >> 27) & 3); }

		/// <summary>
		/// �t�F�[�Y
		/// </summary>
		inline State phase(Phase ph) const {
			State res(v);
			res.v &= ~(3 << 27);
			res.v |= (static_cast<int>(ph) << 27);
			return res;
		}

#pragma endregion

#pragma region �^�[��(�v���C���[)

		/// <summary>
		/// �^�[��(�v���C���[)(shift: 26)
		/// { 0: ���, 1: ��� }
		/// </summary>
		enum Turn {

			/// <summary>
			/// ���(��)
			/// </summary>
			first = 0,

			/// <summary>
			/// ���(��)
			/// </summary>
			second = 1,

		};
		typedef Turn Player;

		/// <summary>
		/// �^�[��
		/// </summary>
		inline Turn turn() const { return static_cast<Turn>((v >> 26) & 1); }

		/// <summary>
		/// �^�[��
		/// </summary>
		inline State turn(Turn t) const {
			State res(v);
			res.v &= ~(1 << 26);
			res.v |= (static_cast<int>(t) << 26);
			return res;
		}

#pragma endregion

#pragma region ���_

		/// <summary>
		/// ���_�̈ʒu
		/// </summary>
		struct Point {

			/// <summary>
			/// ���_�̈ʒu
			/// { 00, 10, 20, 01, 11, 21, 02, 12, 22 }
			/// </summary>
			int v;

			Point() = default;
			Point(int v) : v(v) {}
			Point(int x, int y) : v(y * 3 + x) {}

			inline int x() const { return v % 3; }
			inline int y() const { return v / 3; }

			inline void set(int x, int y) { v = y * 3 + x; }

			inline operator int() const { return v; }
			inline const Point &operator =(int rhs) { v = rhs; return *this; }
			inline const Point &operator =(const Point &rhs) { v = rhs.v; return *this; }
			inline bool operator ==(const Point &rhs) const { return v == rhs.v; }
			inline bool operator !=(const Point &rhs) const { return v != rhs.v; }

		};

		/// <summary>
		/// ���_(shift: 0, 2, 4, ..., 16)
		/// { 0: �����u���Ă��Ȃ�, 1: ���̃R�}, 2: ���̃R�}, 3: ���g�p }
		/// </summary>
		enum Vertex {

			/// <summary>
			/// �����u���Ă��Ȃ����_
			/// </summary>
			blank = 0,

			/// <summary>
			/// ���̃R�}���u���Ă��钸�_
			/// </summary>
			red = 1,

			/// <summary>
			/// ���̃R�}���u���Ă��钸�_
			/// </summary>
			blue = 2,

			/// <summary>
			/// ���_(���g�p)
			/// </summary>
			invalid_vertex = 3,

		};

		inline int vertexShift(const Point &pt) const { return pt.v * 2; }

		/// <summary>
		/// ���_
		/// </summary>
		inline Vertex vertex(const Point &pt) const { return static_cast<Vertex>((v >> vertexShift(pt)) & 3); }

		/// <summary>
		/// ���_
		/// </summary>
		inline State vertex(const Point &pt, const Vertex &vx) const {
			State res(v);
			res.v &= ~(3 << vertexShift(pt));
			res.v |= (static_cast<int>(vx) << vertexShift(pt));
			return res;
		}

#pragma endregion

#pragma region ���O�̈ړ�(��)

		/// <summary>
		/// ��(���O�̈ړ�)(shift: 18, 22)
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

		inline int lastMoveShift(Player pl) const {
			return pl == first ? 18 : 22;
		}

		/// <summary>
		/// ���O�̈ړ�
		/// </summary>
		inline LastMove lastMove(Player pl) const { return static_cast<LastMove>((v >> lastMoveShift(pl)) & 4); }

		/// <summary>
		/// ���O�̈ړ�
		/// </summary>
		inline State lastMove(Player pl, LastMove lm) const {
			State res(v);
			res.v &= ~(15 << lastMoveShift(pl));
			res.v |= (static_cast<int>(lm) << lastMoveShift(pl));
			return res;
		}

		/// <summary>
		/// �ӂ̗��[
		/// </summary>
		struct EdgeEnds {
			Point a, b;

			EdgeEnds() = default;
			EdgeEnds(const Point &a, const Point &b) : a(a), b(b) {}
			EdgeEnds(const EdgeEnds &obj) : a(obj.a), b(obj.b) {}

			inline const EdgeEnds &operator =(const EdgeEnds &rhs) {
				a = rhs.a;
				b = rhs.b;
			}
			inline bool operator ==(const EdgeEnds &rhs) const { return a == rhs.a && b == rhs.b; }
			inline bool operator !=(const EdgeEnds &rhs) const { return a != rhs.a || b != rhs.b; }
		};

		constexpr static int exTbl[9] = { 0, 1, 2, 2, 2, 1, 0, 0, 0 };
		constexpr static int eyTbl[9] = { 0, 0, 0, 1, 2, 2, 2, 1, 0 };

		/// <summary>
		/// �ӂ̗��[
		/// </summary>
		inline static EdgeEnds edgeEnds(Edge e) {
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

		#pragma region ��]�Ɣ��]

		#define _morris_state_rotrev(fx, fy, tbl) \
			State s2 = s; \
			for (int y = 0; y < 3; ++y) for (int x = 0; x < 3; ++x) s2 = s2.vertex({ fx, fy }, s.vertex({ x, y })); \
			const auto ph = s2.phase(); \
			if (ph >= move2) s2 = s2.lastMove(first, tbl[s2.lastMove(first)]); \
			if (ph >= move3) s2 = s2.lastMove(second, tbl[s2.lastMove(second)]); \
			return s2
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
		constexpr static Edge tblRev0022[16] = _morris_state_combine_tbl(tblRev0121, tblRot90);
		constexpr static Edge tblRev1012[16] = _morris_state_combine_tbl(tblRev0121, tblRot180);
		constexpr static Edge tblRev2002[16] = _morris_state_combine_tbl(tblRev0121, tblRot270);

		/// <summary>
		/// �������Ȃ��B
		/// </summary>
		State rot0(State s) const { _morris_state_rotrev(x, y, tblRot0); }

		/// <summary>
		/// �Ֆʂ�90�x��]����B
		/// </summary>
		State rot90(State s) const { _morris_state_rotrev(2 - y, x, tblRot90); }

		/// <summary>
		/// �Ֆʂ�180�x��]����B
		/// </summary>
		State rot180(State s) const { _morris_state_rotrev(2 - x, 2 - y, tblRot180); }

		/// <summary>
		/// �Ֆʂ�270�x��]����B
		/// </summary>
		State rot270(State s) const { _morris_state_rotrev(y, 2 - x, tblRot270); }

		/// <summary>
		/// 01-21 �����ɔ��](�㉺���])����B
		/// </summary>
		State rev0121(State s) const { _morris_state_rotrev(x, 2 - y, tblRev0121); }

		/// <summary>
		/// 00-22 �����ɔ��]����B
		/// </summary>
		State rev0022(State s) const { _morris_state_rotrev(y, x, tblRev0022); }

		/// <summary>
		/// 10-12 �����ɔ��](���E���])����B
		/// </summary>
		State rev1012(State s) const { _morris_state_rotrev(2 - x, y, tblRev1012); }

		/// <summary>
		/// 20-02 �����ɔ��]����B
		/// </summary>
		State rev2002(State s) const { _morris_state_rotrev(2 - y, 2 - x, tblRev2002); }

		#undef _morris_state_combine_tbl
		#undef _morris_state_rotrev

		typedef State (State::*RotRevFunc)(State) const;
		//static constexpr RotRevFunc allRotRev[8] = { rot0, rot90, rot180, rot270, rev0121, rev0022, rev1012, rev2002 };
		static constexpr State (State::*allRotRev[8])(State) const = { &rot0, &rot90, &rot180, &rot270, &rev0121, &rev0022, &rev1012, &rev2002 };

		/// <summary>
		/// ��]�┽�]��K�p�����Ֆʂ̂�����Ԃ��ŏ��̂��̂��擾����B
		/// </summary>
		State minimize() {
			int res = v;
			for (int i = 1; i < 8; ++i) {
				int temp = (this->*allRotRev[i])(*this).v;
				if (temp < res) res = temp;
			}
			return State(res);
		}

		#pragma endregion

	};

}
