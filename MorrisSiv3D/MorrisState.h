#pragma once

struct MorrisState {

	/// <summary>
	/// コマを置くフェーズ・コマを動かすフェーズ
	/// </summary>
	enum Phase {

		/// <summary>
		/// コマを置くフェーズ
		/// </summary>
		Put,

		/// <summary>
		/// コマを動かすフェーズ
		/// </summary>
		Move

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
		first,

		/// <summary>
		/// 後攻
		/// </summary>
		second

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
		/// 移動していなければ (-1, -1)
		/// </summary>
		int prevX, prevY;

	};

	/// <summary>
	/// 先攻のコマ
	/// </summary>
	Piece fp[3];

	/// <summary>
	/// 後攻のコマ
	/// </summary>
	Piece sp[3];

};
