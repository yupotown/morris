#pragma once

#include "MorrisState.h"
#include "MorrisException.h"

class Morris {
public:
	Morris();

	/// <summary>
	/// ゲームの状態を初期状態に戻す。
	/// </summary>
	void reset();

	/// <summary>
	/// 現在のゲームの状態を取得する。
	/// </summary>
	const MorrisState &state() const;

	/// <summary>
	/// move の結果
	/// </summary>
	enum MoveResult {

		/// <summary>
		/// 正しく処理が終了した
		/// </summary>
		ok = 0,

		/// <summary>
		/// エラー: 既にゲームが終了している
		/// </summary>
		finished = 1,

		/// <summary>
		/// エラー: 相手の番にコマを置こうとした
		/// </summary>
		badPlayerPut = 2,

		/// <summary>
		/// エラー: 相手の番にコマを動かそうとした
		/// </summary>
		badPlayerMove = 4,

		/// <summary>
		/// エラー: 既にコマがある場所にコマを置こうとした
		/// </summary>
		alreadyPut = 8,

		/// <summary>
		/// エラー: 既にコマがある場所にコマを動かそうとした
		/// </summary>
		alreadyMove = 16,

		/// <summary>
		/// エラー: 1つ前にあった場所にコマを動かそうとした
		/// </summary>
		returnMove = 32,

		/// <summary>
		/// エラー: 既に置いてあるコマを置き直そうとした
		/// </summary>
		reput = 64,

		/// <summary>
		/// エラー: 隣接するマス以外にコマを動かそうとした
		/// </summary>
		invalidMove = 128,

	};

	/// <summary>
	/// プレイヤー player のコマ piece を (x, y) に動かす。
	/// </summary>
	MoveResult move(MorrisState::Player player, int piece, int x, int y);

	/// <summary>
	/// プレイヤー player のコマ piece を (x, y) に動かせるか否かを確認する。
	/// </summary>
	MoveResult checkMove(MorrisState::Player player, int piece, int x, int y);

private:
	Morris(const Morris &obj);
	const Morris &operator =(const Morris &rhs);

	MorrisState s;
	static const bool neighbors[3][3][3][3];
};

