#include "Morris.h"

Morris::Morris()
{
	reset();
}

void Morris::reset()
{
	s.phase = MorrisState::put;
	s.playing = MorrisState::first;
	for (int i = 0; i < 3; ++i) {
		s.fp[i] = { -1, -1, -1, -1 };
		s.sp[i] = { -1, -1, -1, -1 };
	}
	s.finished = false;
}

const MorrisState & Morris::state() const
{
	return s;
}

Morris::MoveResult Morris::move(MorrisState::Player player, int piece, int x, int y)
{
#pragma region エラー処理

	int res = ok;

	// 既にゲームが終了している
	if (s.finished) res |= finished;

	// 相手の番にコマを置こうと/動かそうとした
	if (player != s.playing) {
		res |= (s.phase == MorrisState::put) ? badPlayerPut : badPlayerMove;
	}

	// 位置指定が範囲外
	if (x < 0 || x >= 3 || y < 0 || y >= 3) {
		throw MorrisException(s, "コマの移動位置の指定が範囲外です。");
	}

	// 既にコマが置いてある場所にコマを置こうと/動かそうとした
	for (int i = 0; i < 3; ++i) {
		if ((s.fp[i].x == x && s.fp[i].y == y) || (s.sp[i].x == x && s.sp[i].y == y)) {
			res |= (s.phase == MorrisState::put) ? alreadyPut : alreadyMove;
		}
	}

	// コマ番号の指定が範囲外
	if (piece < 0 || piece >= 3) throw MorrisException(s, "コマ番号が不正です。");

	// 既に置いたコマを置き直そうとした
	// 1つ前の場所にコマを移動しようとした
	// 隣接したマス以外にコマを動かそうとした
	// プレイヤーの指定が不正
	switch (player)
	{
	case MorrisState::first:
		if (s.phase == MorrisState::put && s.fp[piece].x >= 0) res |= reput;
		if (s.fp[piece].prevX == x && s.fp[piece].prevY == y) res |= returnMove;
		if (s.phase == MorrisState::move && !neighbors[s.fp[piece].x][s.fp[piece].y][x][y]) res |= invalidMove;
		break;
	case MorrisState::second:
		if (s.phase == MorrisState::put && s.sp[piece].x >= 0) res |= reput;
		if (s.sp[piece].prevX == x && s.sp[piece].prevY == y) res |= returnMove;
		if (s.phase == MorrisState::move && !neighbors[s.sp[piece].x][s.sp[piece].y][x][y]) res |= invalidMove;
		break;
	default:
		throw MorrisException(s, "プレイヤー指定が不正です。");
		break;
	}

	if (res != ok) {
		return static_cast<Morris::MoveResult>(res);
	}

#pragma endregion

	// コマを置く/動かす
	for (int i = 0; i < 3; ++i) {
		if (player == MorrisState::first) {
			s.fp[i].prevX = -1;
			s.fp[i].prevY = -1;
		}
		else {
			s.sp[i].prevX = -1;
			s.sp[i].prevY = -1;
		}
	}
	MorrisState::Piece &p = (player == MorrisState::first) ? s.fp[piece] : s.sp[piece];
	p.prevX = p.x;
	p.prevY = p.y;
	p.x = x;
	p.y = y;

	// 決着判定
	bool t[3][3] = { false };
	if (player == MorrisState::first) {
		if (s.fp[0].x >= 0 && s.fp[1].x >= 0 && s.fp[2].x >= 0) {
			for (int i = 0; i < 3; ++i) {
				t[s.fp[i].x][s.fp[i].y] = true;
			}
		}
	}
	else {
		if (s.sp[0].x >= 0 && s.sp[1].x >= 0 && s.sp[2].x >= 0) {
			for (int i = 0; i < 3; ++i) {
				t[s.sp[i].x][s.sp[i].y] = true;
			}
		}
	}
	if ((t[0][0] && t[0][1] && t[0][2])
		|| (t[1][0] && t[1][1] && t[1][2])
		|| (t[2][0] && t[2][1] && t[2][2])
		|| (t[0][0] && t[1][0] && t[2][0])
		|| (t[0][1] && t[1][1] && t[2][1])
		|| (t[0][2] && t[1][2] && t[2][2])
		|| (t[0][0] && t[1][1] && t[2][2])
		|| (t[2][0] && t[1][1] && t[0][2])
		) {
		s.finished = true;
		return ok;
	}

	// 全部置き終わったら動かすフェーズに入る
	if (s.phase == MorrisState::put) {
		if (s.sp[0].x >= 0 && s.sp[1].x >= 0 && s.sp[2].x >= 0) {
			s.phase = MorrisState::move;
		}
	}

	// プレイヤーの番を交代
	s.playing = (player == MorrisState::first) ? MorrisState::second : MorrisState::first;

	return ok;
}

Morris::Morris(const Morris &)
{
}

const Morris & Morris::operator=(const Morris &)
{
	return *this;
}

const bool Morris::neighbors[3][3][3][3] = {
	{
		// [0][0]
		{
			{ false, true, false },
			{ true, true, false },
			{ false, false, false },
		},
	// [0][1]
	{
		{ true, false, true },
		{ false, true, false },
		{ false, false, false },
	},
	// [0][2]
	{
		{ false, true, false },
		{ false, true, true },
		{ false, false, false },
	},
},
{
	// [1][0]
	{
		{ true, false, false },
		{ false, true, false },
		{ true, false, false },
	},
	// [1][1]
	{
		{ true, true, true },
		{ true, false, true },
		{ true, true, true },
	},
	// [1][2]
	{
		{ false, false, true },
		{ false, true, false },
		{ false, false, true },
	},
},
{
	// [2][0]
	{
		{ false, false, false },
		{ true, true, false },
		{ false, true, false },
	},
	// [2][1]
	{
		{ false, false, false },
		{ false, true, false },
		{ true, false, true },
	},
	// [2][2]
	{
		{ false, false, false },
		{ false, true, true },
		{ false, true, false },
	},
}
};
