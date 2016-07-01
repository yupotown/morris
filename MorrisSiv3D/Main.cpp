
# include <Siv3D.hpp>

#include "Morris.h"

void Main()
{
	Morris game;

	// 描画位置
	Point center = Window::Size() / 2;
	Size grid = { 100, 100 };
	Point posF[3], posS[3];
	for (int i = 0; i < 3; ++i) {
		posF[i] = { center.x - grid.x * 2, center.y + (i - 1) * grid.y };
		posS[i] = { center.x + grid.x * 2, center.y + (i - 1) * grid.y };
	}
	double rp = 20;
	Color
		colF = { 255, 0, 0, 255 },
		colFHolding = { 255, 0, 0, 128 },
		colS = { 0, 0, 255, 255 },
		colSHolding = { 0, 0, 255, 128 },
		colLine = { 255, 255, 255, 255 };
	Font fontTurn(20), fontWin(20);
	Point posTurn = { 10, 10 };
	Color colTurn = { 255, 255, 255, 255 };

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
			if (holdingPiece < 0) {
				for (int i = 0; i < 3; ++i) {
					if ((s.fp[i].x < 0 && Circle(posF[i], rp).contains(mouse))
						|| (s.fp[i].x >= 0 && Circle({ lt.x + s.fp[i].x * grid.x, lt.y + s.fp[i].y * grid.y }, rp).contains(mouse))) {
						holdingPlayer = MorrisState::first;
						holdingPiece = i;
					}
					if ((s.sp[i].x < 0 && Circle(posS[i], rp).contains(mouse))
						|| (s.sp[i].x >= 0 && Circle({ lt.x + s.sp[i].x * grid.x, lt.y + s.sp[i].y * grid.y }, rp).contains(mouse))) {
						holdingPlayer = MorrisState::second;
						holdingPiece = i;
					}
				}
			}
			else {
				for (int y = 0; y < 3; ++y) for (int x = 0; x < 3; ++x) {
					if (Circle({ lt.x + x * grid.x, lt.y + y * grid.y }, rp).leftClicked) {
						game.move(holdingPlayer, holdingPiece, x, y);
					}
				}
				holdingPiece = -1;
			}
		}

		// 描画
		Line(lt.x, lt.y, rb.x, lt.y).draw(colLine);
		Line(lt.x, lt.y, lt.x, rb.y).draw(colLine);
		Line(rb.x, lt.y, rb.x, rb.y).draw(colLine);
		Line(lt.x, rb.y, rb.x, rb.y).draw(colLine);
		Line(lt.x, lt.y, rb.x, rb.y).draw(colLine);
		Line(rb.x, lt.y, lt.x, rb.y).draw(colLine);
		Line(lt.x, center.y, rb.x, center.y).draw(colLine);
		Line(center.x, lt.y, center.x, rb.y).draw(colLine);
		for (int i = 0; i < 3; ++i) {
			if (s.fp[i].prevX >= 0) {
				Circle({ lt.x + s.fp[i].prevX * grid.x, lt.y + s.fp[i].prevY * grid.y }, rp).drawFrame(2, 0, colF);
				Line(lt.x + s.fp[i].prevX * grid.x, lt.y + s.fp[i].prevY * grid.y, lt.x + s.fp[i].x * grid.x, lt.y + s.fp[i].y * grid.y).draw(colF);
			}
			if (s.sp[i].prevX >= 0) {
				Circle({ lt.x + s.sp[i].prevX * grid.x, lt.y + s.sp[i].prevY * grid.y }, rp).drawFrame(2, 0, colS);
				Line(lt.x + s.sp[i].prevX * grid.x, lt.y + s.sp[i].prevY * grid.y, lt.x + s.sp[i].x * grid.x, lt.y + s.sp[i].y * grid.y).draw(colS);
			}
		}
		for (int i = 0; i < 3; ++i) {
			Color cf, cs;
			if (holdingPlayer == MorrisState::first && holdingPiece == i) {
				cf = colFHolding;
			}
			else {
				cf = colF;
			}
			if (holdingPlayer == MorrisState::second && holdingPiece == i) {
				cs = colSHolding;
			}
			else {
				cs = colS;
			}
			if (s.fp[i].x < 0) {
				Circle(posF[i], rp).draw(cf);
			}
			else {
				Circle({ lt.x + s.fp[i].x * grid.x, lt.y + s.fp[i].y * grid.y }, rp).draw(cf);
			}
			if (s.sp[i].x < 0) {
				Circle(posS[i], rp).draw(cs);
			}
			else {
				Circle({ lt.x + s.sp[i].x * grid.x, lt.y + s.sp[i].y * grid.y }, rp).draw(cs);
			}
		}

		Circle({ posTurn.x + rp, posTurn.y + rp }, rp).draw(s.playing == MorrisState::first ? colF : colS);
		if (s.finished) {
			fontWin(L"の勝ち！").draw({ posTurn.x + rp * 2 + 1, posTurn.y }, colTurn);
		}
		else {
			fontTurn(L"の番").draw({ posTurn.x + rp * 2 + 1, posTurn.y }, colTurn);
		}

		if (holdingPiece >= 0) {
			const Color col = (holdingPlayer == MorrisState::first) ? colF : colS;
			Point pos = mouse;
			for (int y = 0; y < 3; ++y) for (int x = 0; x < 3; ++x) {
				Point c = { lt.x + x * grid.x, lt.y + y * grid.y };
				if (Circle(c, rp).contains(mouse)) {
					pos = c;
				}
			}
			Circle(pos, rp).draw(col);
		}
	}
}

