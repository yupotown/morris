#pragma once

struct MorrisState {

	/// <summary>
	/// コマを置くフェーズ・コマを動かすフェーズ
	/// </summary>
	enum Phase {

		/// <summary>
		/// コマを置くフェーズ
		/// </summary>
		put,

		/// <summary>
		/// コマを動かすフェーズ
		/// </summary>
		move

	};

	/// <summary>
	/// コマを置くフェーズか、コマを動かすフェーズか
	/// </summary>
	Phase phase;

	/// <summary>
	/// 先攻・後攻
	/// </summary>
	enum Player {

		/// <summary>
		/// 先攻
		/// </summary>
		first = 0,

		/// <summary>
		/// 後攻
		/// </summary>
		second = 1

	};

	/// <summary>
	/// 先攻の番か後攻の番か
	/// </summary>
	Player playing;

	/// <summary>
	/// コマ
	/// </summary>
	struct Piece {

		/// <summary>
		/// 現在の位置
		/// まだボード外なら (-1, -1)
		/// </summary>
		int x, y;

		/// <summary>
		/// 1ターン前の位置
		/// 動かしていなければ (-1, -1)
		/// </summary>
		int prevX, prevY;

	};

	/// <summary>
	/// pieces[player]: player のコマ
	/// </summary>
	Piece pieces[2][3];

	/// <summary>
	/// ゲームが終了しているか否か
	/// <summary>
	bool finished;

};
