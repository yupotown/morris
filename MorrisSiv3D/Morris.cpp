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
#pragma region �G���[����

	int res = ok;

	// ���ɃQ�[�����I�����Ă���
	if (s.finished) res |= finished;

	// ����̔ԂɃR�}��u������/���������Ƃ���
	if (player != s.playing) {
		res |= (s.phase == MorrisState::put) ? badPlayerPut : badPlayerMove;
	}

	// �ʒu�w�肪�͈͊O
	if (x < 0 || x >= 3 || y < 0 || y >= 3) {
		throw MorrisException(s, "�R�}�̈ړ��ʒu�̎w�肪�͈͊O�ł��B");
	}

	// ���ɃR�}���u���Ă���ꏊ�ɃR�}��u������/���������Ƃ���
	for (int i = 0; i < 3; ++i) {
		if ((s.fp[i].x == x && s.fp[i].y == y) || (s.sp[i].x == x && s.sp[i].y == y)) {
			res |= (s.phase == MorrisState::put) ? alreadyPut : alreadyMove;
		}
	}

	// �R�}�ԍ��̎w�肪�͈͊O
	if (piece < 0 || piece >= 3) throw MorrisException(s, "�R�}�ԍ����s���ł��B");

	// ���ɒu�����R�}��u���������Ƃ���
	// 1�O�̏ꏊ�ɃR�}���ړ����悤�Ƃ���
	// �אڂ����}�X�ȊO�ɃR�}�𓮂������Ƃ���
	// �v���C���[�̎w�肪�s��
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
		throw MorrisException(s, "�v���C���[�w�肪�s���ł��B");
		break;
	}

	if (res != ok) {
		return static_cast<Morris::MoveResult>(res);
	}

#pragma endregion

	// �R�}��u��/������
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

	// ��������
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

	// �S���u���I������瓮�����t�F�[�Y�ɓ���
	if (s.phase == MorrisState::put) {
		if (s.sp[0].x >= 0 && s.sp[1].x >= 0 && s.sp[2].x >= 0) {
			s.phase = MorrisState::move;
		}
	}

	// �v���C���[�̔Ԃ����
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
