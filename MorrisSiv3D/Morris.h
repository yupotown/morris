#pragma once

#include "MorrisState.h"
#include "MorrisException.h"

class Morris {
public:
	Morris();

	/// <summary>
	/// �Q�[���̏�Ԃ�������Ԃɖ߂��B
	/// </summary>
	void reset();

	/// <summary>
	/// ���݂̃Q�[���̏�Ԃ��擾����B
	/// </summary>
	const MorrisState &state() const;

	/// <summary>
	/// move �̌���
	/// </summary>
	enum MoveResult {

		/// <summary>
		/// �������������I������
		/// </summary>
		ok = 0,

		/// <summary>
		/// �G���[: ���ɃQ�[�����I�����Ă���
		/// </summary>
		finished = 1,

		/// <summary>
		/// �G���[: ����̔ԂɃR�}��u�����Ƃ���
		/// </summary>
		badPlayerPut = 2,

		/// <summary>
		/// �G���[: ����̔ԂɃR�}�𓮂������Ƃ���
		/// </summary>
		badPlayerMove = 4,

		/// <summary>
		/// �G���[: ���ɃR�}������ꏊ�ɃR�}��u�����Ƃ���
		/// </summary>
		alreadyPut = 8,

		/// <summary>
		/// �G���[: ���ɃR�}������ꏊ�ɃR�}�𓮂������Ƃ���
		/// </summary>
		alreadyMove = 16,

		/// <summary>
		/// �G���[: 1�O�ɂ������ꏊ�ɃR�}�𓮂������Ƃ���
		/// </summary>
		returnMove = 32,

		/// <summary>
		/// �G���[: ���ɒu���Ă���R�}��u���������Ƃ���
		/// </summary>
		reput = 64,

		/// <summary>
		/// �G���[: �אڂ���}�X�ȊO�ɃR�}�𓮂������Ƃ���
		/// </summary>
		invalidMove = 128,

	};

	/// <summary>
	/// �v���C���[ player �̃R�} piece �� (x, y) �ɓ������B
	/// </summary>
	MoveResult move(MorrisState::Player player, int piece, int x, int y);

private:
	Morris(const Morris &obj);
	const Morris &operator =(const Morris &rhs);

	MorrisState s;
	static const bool neighbors[3][3][3][3];
};

