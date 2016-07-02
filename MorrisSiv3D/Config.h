#pragma once

#include <Siv3D.hpp>
#include "MorrisState.h"

struct Config {

	/// <summary>
	/// フルスクリーンにするか否か
	/// [Window.fullscreen]
	/// </summary>
	bool fullscreen;

	/// <summary>
	/// ウィンドウサイズ・解像度
	/// [Window.size]
	/// </summary>
	Size resolution;

	/// <summary>
	/// 盤面の中心の位置
	/// [Grid.center]
	/// </summary>
	Point gridCenter;

	/// <summary>
	/// 盤面の1マスの大きさ
	/// [Grid.size]
	/// </summary>
	Size gridSize;

	/// <summary>
	/// グリッド線の色
	/// [Grid.color]
	/// </summary>
	Color gridCol;

	/// <summary>
	/// 置く前のコマを描画する位置
	/// [Piece.point1] - [Piece.point6]
	/// </summary>
	Point piecePos[2][3];

	/// <summary>
	/// コマの半径
	/// [Piece.radius]
	/// </summary>
	double pieceRad;

	/// <summary>
	/// コマの色
	/// [Piece.color1] [Piece.color2]
	/// </summary>
	Color pieceCol[2];

	/// <summary>
	/// 掴まれているコマの元の場所に描画する色
	/// </summary>
	Color pieceColHold[2];

	/// <summary>
	/// 番や勝ち負けのフォント
	/// [Turn.font] [Turn.fontsize]
	/// </summary>
	Font turnFont;

	/// <summary>
	/// 番や勝ち負けの位置
	/// [Turn.point]
	/// </summary>
	Point turnPos;

	/// <summary>
	/// 番や勝ち負けの色
	/// [Turn.color]
	/// </summary>
	Color turnCol;

	/// <summary>
	/// リセットボタンの位置
	/// [Reset.point]
	/// </summary>
	Point resetPos;

	/// <summary>
	/// リセットボタンのフォント
	/// [Reset.font]
	/// </summary>
	Font resetFont;

	Config() = default;

	/// <summary>
	/// 設定を ini ファイルに書き込む。
	/// </summary>
	void write(String file) const;

	/// <summary>
	/// 設定を ini ファイルから読み込む。
	/// 読み込みに失敗した部分はデフォルト値となる。
	/// </summary>
	void read(String file);
	
	/// <summary>
	/// グリッドの各点の座標を求める。
	/// </summary>
	Point gridPos(int x, int y) const;
};
