
# include <Siv3D.hpp>
#include <set>

// * マス (x, y) を数値 (y * 3 + x) に対応させる
// 状態		| フェーズ[2] | ターン[1] | 青の直線移動[4] | 赤の直前移動[4] | 盤面[18] |
// フェーズ	{ 置く, 動かす(1ターン目), 動かす(2ターン目), 動かす(3ターン目以降) }
// ターン	{ 赤, 青 }
// 直前移動	{ 01, 03, 04, 12, 14, 24, 25, 34, 36, 45, 46, 47, 48, 58, 67, 78 }
// 盤面		| マス8[2] | ... | マス0[2] |
// マス		{ なし, 赤, 青, (未使用) }

#pragma region コマ

enum Piece {
	blank = 0,
	red = 1,
	blue = 2,
};

Piece piece(int s, int x, int y) {
	const int shift = (y * 3 + x) * 2;
	return static_cast<Piece>((s >> shift) & 3);
}

int setPiece(int s, int x, int y, Piece p) {
	const int shift = (y * 3 + x) * 2;
	s &= ~(3 << shift);
	s |= (static_cast<int>(p) << shift);
	return s;
}

#pragma endregion

#pragma region ターン

enum Turn {
	first = 0,
	second = 1,
};

Turn turn(int s) {
	return static_cast<Turn>((s >> 26) & 1);
}

int setTurn(int s, Turn t) {
	s &= ~(1 << 26);
	s |= (static_cast<int>(t) << 26);
	return s;
}

#pragma endregion

#pragma region フェーズ

enum Phase {
	put = 0,
	move1 = 1,
	move2 = 2,
	move = 3,
};

Phase phase(int s) {
	return static_cast<Phase>((s >> 27) & 3);
}

int setPhase(int s, Phase ph) {
	s &= ~(3 << 27);
	s |= (static_cast<int>(ph) << 27);
	return s;
}

#pragma endregion

#pragma region 直前移動

typedef int LastMove;
typedef Turn Player;

struct Move {
	int x1, y1, x2, y2;
};

Move lmTbl[16] = {
	{ 0, 0, 1, 0 },
	{ 0, 0, 0, 1 },
	{ 0, 0, 1, 1 },
	{ 1, 0, 2, 0 },
	{ 1, 0, 1, 1 },
	{ 2, 0, 1, 1 },
	{ 2, 0, 2, 1 },
	{ 0, 1, 1, 1 },
	{ 0, 1, 0, 2 },
	{ 1, 1, 2, 1 },
	{ 1, 1, 0, 2 },
	{ 1, 1, 1, 2 },
	{ 1, 1, 2, 2 },
	{ 2, 1, 2, 2 },
	{ 0, 2, 1, 2 },
	{ 1, 2, 2, 2 },
};

LastMove lastmove(int s, Player pl) {
	const int shift = 18 + static_cast<int>(pl) * 4;
	return (s >> shift) & 15;
}

int setLastmove(int s, Player pl, LastMove lm) {
	const int shift = 18 + static_cast<int>(pl) * 4;
	s &= ~(15 << shift);
	s |= (lm << shift);
	return s;
}

#pragma endregion

const Color colPiece[4] = {
	{ 255, 255, 255, 255 },
	{ 255, 0, 0, 255 },
	{ 0, 0, 255, 255 },
	{ 0, 0, 0, 0 }
};

const Color colTurn[2] = { colPiece[red], colPiece[blue] };

void Main()
{
	int s = 0;

	// スロット
	std::set<int> slot[10];
	Key keys[10];
	String slotPaths[10];
	for (int i = 0; i < 10; ++i) {
		keys[i] = Key(static_cast<wchar>(L'0' + i));
		slotPaths[i] = Format(L"slot/slot", i);
	}
	Font font(10);

	// マス
	Circle c[3][3];
	for (int y = 0; y < 3; ++y) for (int x = 0; x < 3; ++x) c[x][y] = { { 50 + x * 100, 50 + y * 100 }, 20 };

	// グリッド
	Quad grid[16];
	for (int i = 0; i < 16; ++i) {
		Move m = lmTbl[i];
		Vec2 a = c[m.x1][m.y1].center, b = c[m.x2][m.y2].center;
		Vec2 v = b - a, t = { -v.y, v.x };
		double norm = Sqrt(v.x * v.x + v.y * v.y);
		Vec2 u = (5 / norm) * t;
		grid[i] = { a + u, b + u, b - u, a - u };
	}
	Color gridCol[3] = { { 255, 255, 255, 64 }, { 255, 0, 0, 255 }, { 0, 0, 255, 255 } };

	// ターン、フェーズ
	GUI gui(GUIStyle::Default);
	gui.setPos({ 350, 30 });
	gui.setTitle(L"");
	gui.add(L"turn", GUIRadioButton::Create({ L"先攻(赤)", L"後攻(青)" }, 0u));
	gui.add(L"phase", GUIRadioButton::Create({ L"置く", L"動かす(1)", L"動かす(2)", L"動かす" }, 0u));

	while (System::Update())
	{
		// スロット
		for (int i = 0; i < 10; ++i) {
			if (keys[i].clicked) {
				if (slot[i].count(s) == 0) slot[i].insert(s);
				else slot[i].erase(s);
			}
			font(Format(slot[i].size())).draw({ i * 50, 0 });
		}

		// 保存
		if ((Input::KeyControl + Input::KeyS).clicked) {
			for (int i = 0; i < 10; ++i) {
				BinaryWriter writer(slotPaths[i]);
				for (auto it = slot[i].begin(); it != slot[i].end(); ++it) writer.write(*it);
			}
		}

		// 読み込み
		if ((Input::KeyControl + Input::KeyO).clicked) {
			int temp;
			for (int i = 0; i < 10; ++i) {
				slot[i].clear();
				BinaryReader reader(slotPaths[i]);
				while (reader.read<int>(temp)) slot[i].insert(temp);
			}
		}

		// グリッド
		int gf = -1, gs = -1;
		if (phase(s) >= move2) gf = lastmove(s, first);
		if (phase(s) >= move) gs = lastmove(s, second);
		for (int i = 0; i < 16; ++i) {
			grid[i].draw((i == gf) ? gridCol[1] : (i == gs) ? gridCol[2] : gridCol[0]);
		}

		// コマ
		bool pc = false;
		for (int y = 0; y < 3; ++y) for (int x = 0; x < 3; ++x) {
			Piece p = piece(s, x, y);
			c[x][y].draw(colPiece[p]);
			if (c[x][y].leftClicked) {
				pc = true;
				s = setPiece(s, x, y, (p == red) ? blank : red);
			}
			if (c[x][y].rightClicked) {
				pc = true;
				s = setPiece(s, x, y, (p == blue) ? blank : blue);
			}
		}

		// グリッド(操作)
		if (!pc && phase(s) >= move2) for (int i = 0; i < 16; ++i) {
			if (grid[i].leftClicked) s = setLastmove(s, first, i);
		}
		if (!pc && phase(s) >= move) for (int i = 0; i < 16; ++i) {
			if (grid[i].rightClicked) s = setLastmove(s, second, i);
		}
		
		// ターン
		if (gui.radioButton(L"turn").hasChanged) {
			s = setTurn(s, static_cast<Turn>(gui.radioButton(L"turn").checkedItem.value()));
		}

		// フェーズ
		if (gui.radioButton(L"phase").hasChanged) {
			Phase ph = static_cast<Phase>(gui.radioButton(L"phase").checkedItem.value());
			s = setPhase(s, ph);
			if (ph < move2) s = setLastmove(s, first, 0);
			if (ph < move) s = setLastmove(s, second, 0);
		}
	}
}
