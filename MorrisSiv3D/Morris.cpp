#include "Morris.h"

Morris::Morris()
{
	reset();
}

void Morris::reset()
{
	s.phase = MorrisState::put;
	s.playing = MorrisState::first;
	for (int i = 0; i < 2; ++i) for (int j = 0; j < 3; ++j) {
		s.pieces[i][j] = { -1, -1, -1, -1 };
	}
	s.finished = false;
}

const MorrisState & Morris::state() const
{
	return s;
}

Morris::MoveResult Morris::move(MorrisState::Player player, int piece, int x, int y)
{
	// �G���[����
	MoveResult res = checkMove(player, piece, x, y);
	if (res != ok) {
		return res;
	}

	// �R�}��u��/������
	for (int i = 0; i < 3; ++i) {
		s.pieces[player][i].prevX = -1;
		s.pieces[player][i].prevY = -1;
	}
	MorrisState::Piece &p = s.pieces[player][piece];
	p.prevX = p.x;
	p.prevY = p.y;
	p.x = x;
	p.y = y;

	// ��������
	bool t[3][3] = { false };
	if (s.pieces[player][0].x >= 0 && s.pieces[player][1].x >= 0 && s.pieces[player][2].x >= 0) {
		for (int i = 0; i < 3; ++i) {
			t[s.pieces[player][i].x][s.pieces[player][i].y] = true;
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

	// �S���u���I������瓮�����t�F�[�Y�ɓ���
	if (s.phase == MorrisState::put) {
		if (s.pieces[MorrisState::second][0].x >= 0 && s.pieces[MorrisState::second][1].x >= 0 && s.pieces[MorrisState::second][2].x >= 0) {
			s.phase = MorrisState::move;
		}
	}

	// �v���C���[�̔Ԃ����
	s.playing = (player == MorrisState::first) ? MorrisState::second : MorrisState::first;

	return ok;
}

Morris::MoveResult Morris::checkMove(MorrisState::Player player, int piece, int x, int y)
{
	// �ʒu�w�肪�͈͊O
	if (x < 0 || x >= 3 || y < 0 || y >= 3) {
		throw MorrisException(s, "�R�}�̈ړ��ʒu�̎w�肪�͈͊O�ł��B");
	}

	// �R�}�ԍ��̎w�肪�͈͊O
	if (piece < 0 || piece >= 3) throw MorrisException(s, "�R�}�ԍ����s���ł��B");

	// �v���C���[�̎w�肪�s��
	if (player != MorrisState::first && player != MorrisState::second) throw MorrisException(s, "�v���C���[�w�肪�s���ł��B");

	int res = ok;

	// ���ɃQ�[�����I�����Ă���
	if (s.finished) res |= finished;

	// ����̔ԂɃR�}��u������/���������Ƃ���
	if (player != s.playing) {
		res |= (s.phase == MorrisState::put) ? badPlayerPut : badPlayerMove;
	}

	// ���ɃR�}���u���Ă���ꏊ�ɃR�}��u������/���������Ƃ���
	for (int i = 0; i < 2; ++i) for (int j = 0; j < 3; ++j) {
		if (s.pieces[i][j].x == x && s.pieces[i][j].y == y) {
			res |= (s.phase == MorrisState::put) ? alreadyPut : alreadyMove;
		}
	}

	// ���ɒu�����R�}��u���������Ƃ���
	if (s.phase == MorrisState::put && s.pieces[s.playing][piece].x >= 0) res |= reput;

	// 1�O�̏ꏊ�ɃR�}���ړ����悤�Ƃ���
	if (s.pieces[s.playing][piece].prevX == x && s.pieces[s.playing][piece].prevY == y) res |= returnMove;

	// �אڂ����}�X�ȊO�ɃR�}�𓮂������Ƃ���
	if (s.phase == MorrisState::move && !neighbors[s.pieces[s.playing][piece].x][s.pieces[s.playing][piece].y][x][y]) res |= invalidMove;

	return static_cast<Morris::MoveResult>(res);
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
