
# include <Siv3D.hpp>

#include "Morris.h"
#include <functional>

void Main()
{
	Morris game;

	// 盤面を描画する中心の位置
	Point center = Window::Size() / 2;

	// 盤面の1マスを描画する大きさ
	Size grid = { 120, 120 };

	// グリッドの色
	Color colLine = { 255, 255, 255, 255 };

	// グリッドの各点の座標
	std::function<Point(int, int)> gridPos = [&](int x, int y) {
		return Point(center.x + (x - 1) * grid.x, center.y + (y - 1) * grid.y);
	};

	// 置く前のコマを描画する位置
	Point posP[2][3];
	for (int i = 0; i < 2; ++i) for (int j = 0; j < 3; ++j) {
		posP[i][j] = { center.x + (4 * i - 2) * grid.x, center.y + (j - 1) * grid.y };
	}

	// コマを描画する半径
	double rp = 24;

	// コマの色
	Color colP[2] = { {255, 0, 0, 255}, {0, 0, 255, 255} };

	// 掴まれているコマを元の場所に描画するときの色
	Color colHolding[2] = { {255, 0, 0, 128}, {0, 0, 255, 128} };

	// 番や勝ち負けを表示するフォント
	Font fontTurn(20), fontWin(20);

	// 番や勝ち負けを表示する位置
	Point posTurn = { 10, 10 };

	// 番屋勝ち負けを表示する色
	Color colTurn = { 255, 255, 255, 255 }, colWin = { 255, 255, 255, 255 };

	// 掴んでいるコマ
	MorrisState::Player holdingPlayer = MorrisState::first;
	int holdingPiece = -1;

	while (System::Update())
	{
		const Point mouse = Mouse::Pos();
		const MorrisState &s = game.state();
		const Point lt = center - grid, rb = center + grid;

		// 操作
		if (Input::MouseL.clicked) {
			if (holdingPiece < 0 && !s.finished) {
				// コマを掴む
				for (int j = 0; j < 3; ++j) {
					const auto &p = s.pieces[s.playing][j];
					if ((p.x < 0 && Circle(posP[s.playing][j], rp).contains(mouse))
						|| (p.x >= 0 && Circle(gridPos(p.x, p.y), rp).contains(mouse))) {
						holdingPlayer = static_cast<MorrisState::Player>(s.playing);
						holdingPiece = j;
					}
				}
			}
			else if (holdingPiece >= 0) {
				// コマを放す
				for (int y = 0; y < 3; ++y) for (int x = 0; x < 3; ++x) {
					if (Circle(gridPos(x, y), rp).leftClicked) {
						game.move(holdingPlayer, holdingPiece, x, y);
					}
				}
				holdingPiece = -1;
			}
		}

		// グリッド描画
		Line(gridPos(0, 0), gridPos(2, 0)).draw(colLine);
		Line(gridPos(0, 1), gridPos(2, 1)).draw(colLine);
		Line(gridPos(0, 2), gridPos(2, 2)).draw(colLine);
		Line(gridPos(0, 0), gridPos(0, 2)).draw(colLine);
		Line(gridPos(1, 0), gridPos(1, 2)).draw(colLine);
		Line(gridPos(2, 0), gridPos(2, 2)).draw(colLine);
		Line(gridPos(0, 0), gridPos(2, 2)).draw(colLine);
		Line(gridPos(2, 0), gridPos(0, 2)).draw(colLine);

		// 移動したコマの元の場所を表示
		for (int i = 0; i < 2; ++i) for (int j = 0; j < 3; ++j) {
			const auto &p = s.pieces[i][j];
			if (p.prevX >= 0) {
				Circle(gridPos(p.prevX, p.prevY), rp).drawFrame(2, 0, colP[i]);
				Line(gridPos(p.prevX, p.prevY), gridPos(p.x, p.y)).draw(colP[i]);
			}
		}

		// 盤面上や置く前のコマを表示
		for (int i = 0; i < 2; ++i) for (int j = 0; j < 3; ++j) {
			const Color col = (holdingPlayer == i && holdingPiece == j) ? colHolding[i] : colP[i];
			const auto &p = s.pieces[i][j];
			if (p.x < 0) {
				Circle(posP[i][j], rp).draw(col);
			}
			else {
				Circle(gridPos(p.x, p.y), rp).draw(col);
			}
		}

		// どちらの番かを表示
		Circle({ posTurn.x + rp, posTurn.y + rp }, rp).draw(colP[s.playing]);
		if (s.finished) {
			fontWin(L"の勝ち！").draw({ posTurn.x + rp * 2 + 1, posTurn.y }, colWin);
		}
		else {
			fontTurn(L"の番").draw({ posTurn.x + rp * 2 + 1, posTurn.y }, colTurn);
		}

		// マウスカーソルの位置に移動中のコマを描画
		if (holdingPiece >= 0) {
			Point pos = mouse;
			for (int y = 0; y < 3; ++y) for (int x = 0; x < 3; ++x) {
				if (Circle(gridPos(x, y), rp).contains(mouse) && game.checkMove(holdingPlayer, holdingPiece, x, y) == Morris::ok) {
					pos = gridPos(x, y);
				}
			}
			Circle(pos, rp).draw(colP[holdingPlayer]);
		}
	}
}

