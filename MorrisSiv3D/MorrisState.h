#pragma once

struct MorrisState {

	/// <summary>
	/// �R�}��u���t�F�[�Y�E�R�}�𓮂����t�F�[�Y
	/// </summary>
	enum Phase {

		/// <summary>
		/// �R�}��u���t�F�[�Y
		/// </summary>
		put,

		/// <summary>
		/// �R�}�𓮂����t�F�[�Y
		/// </summary>
		move

	};

	/// <summary>
	/// �R�}��u���t�F�[�Y���A�R�}�𓮂����t�F�[�Y��
	/// </summary>
	Phase phase;

	/// <summary>
	/// ��U�E��U
	/// </summary>
	enum Player {

		/// <summary>
		/// ��U
		/// </summary>
		first = 0,

		/// <summary>
		/// ��U
		/// </summary>
		second = 1

	};

	/// <summary>
	/// ��U�̔Ԃ���U�̔Ԃ�
	/// </summary>
	Player playing;

	/// <summary>
	/// �R�}
	/// </summary>
	struct Piece {

		/// <summary>
		/// ���݂̈ʒu
		/// �܂��{�[�h�O�Ȃ� (-1, -1)
		/// </summary>
		int x, y;

		/// <summary>
		/// 1�^�[���O�̈ʒu
		/// �������Ă��Ȃ���� (-1, -1)
		/// </summary>
		int prevX, prevY;

	};

	/// <summary>
	/// pieces[player]: player �̃R�}
	/// </summary>
	Piece pieces[2][3];

	/// <summary>
	/// �Q�[�����I�����Ă��邩�ۂ�
	/// <summary>
	bool finished;

};
