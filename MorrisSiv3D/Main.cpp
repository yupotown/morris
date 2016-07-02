
# include <Siv3D.hpp>

#include "Morris.h"
#include "Config.h"
#include <functional>
#include <algorithm>

void Main()
{
	Window::SetTitle(L"三目並べ by ゆぽ");

	// 設定ファイル
	const String iniFile = L"morris.ini";
	Config conf;
	conf.read(iniFile);
	conf.write(iniFile);

	// フルスクリーン、ウィンドウサイズ
	if (conf.fullscreen) {
		const Array<Size> resos = Graphics::GetFullScreenSize();
		if (std::find(resos.begin(), resos.end(), conf.resolution) == resos.end()) {
			Window::Resize(conf.resolution);
		}
		else {
			Window::SetFullscreen(true, conf.resolution);
		}
	}
	else {
		Window::Resize(conf.resolution);
	}

	// リセットボタン
	GUIStyle style = GUIStyle::Default;
	style.showTitle = false;
	style.background.color = none;
	style.borderColor = none;
	style.padding = { 0, 0, 0, 0 };
	style.shadowColor = none;
	GUI gui(style);
	WidgetStyle wstyle;
	wstyle.font = conf.resetFont;
	wstyle.margin = { 0, 0, 0, 0 };
	wstyle.padding = { 0, 0, 0, 0 };
	gui.add(L"reset", GUIButton::Create(L"リセット", wstyle));
	gui.setPos(conf.resetPos);

	// 掴んでいるコマ
	MorrisState::Player holdingPlayer = MorrisState::first;
	int holdingPiece = -1;

	Morris game;

	while (System::Update())
	{
		const Point mouse = Mouse::Pos();
		const MorrisState &s = game.state();

		// 操作
		if (Input::MouseL.clicked) {
			if (holdingPiece < 0 && !s.finished) {
				// コマを掴む
				if (s.phase == MorrisState::put) {
					for (int i = 0; i < 3; ++i) {
						const auto &p = s.pieces[s.playing][i];
						if (p.x < 0 && Circle(conf.piecePos[s.playing][i], conf.pieceRad).leftClicked) {
							holdingPlayer = static_cast<MorrisState::Player>(s.playing);
							holdingPiece = i;
						}
					}
				}
				else {
					for (int i = 0; i < 3; ++i) {
						const auto &p = s.pieces[s.playing][i];
						if (p.x >= 0 && Circle(conf.gridPos(p.x, p.y), conf.pieceRad).leftClicked) {
							holdingPlayer = static_cast<MorrisState::Player>(s.playing);
							holdingPiece = i;
						}
					}
				}
			}
			else if (holdingPiece >= 0) {
				// コマを放す
				for (int y = 0; y < 3; ++y) for (int x = 0; x < 3; ++x) {
					if (Circle(conf.gridPos(x, y), conf.pieceRad).leftClicked) {
						game.move(holdingPlayer, holdingPiece, x, y);
					}
				}
				holdingPiece = -1;
			}
		}

		// リセット
		if (gui.button(L"reset").pushed) {
			game.reset();
		}

		// グリッド描画
		Line(conf.gridPos(0, 0), conf.gridPos(2, 0)).draw(conf.gridCol);
		Line(conf.gridPos(0, 1), conf.gridPos(2, 1)).draw(conf.gridCol);
		Line(conf.gridPos(0, 2), conf.gridPos(2, 2)).draw(conf.gridCol);
		Line(conf.gridPos(0, 0), conf.gridPos(0, 2)).draw(conf.gridCol);
		Line(conf.gridPos(1, 0), conf.gridPos(1, 2)).draw(conf.gridCol);
		Line(conf.gridPos(2, 0), conf.gridPos(2, 2)).draw(conf.gridCol);
		Line(conf.gridPos(0, 0), conf.gridPos(2, 2)).draw(conf.gridCol);
		Line(conf.gridPos(2, 0), conf.gridPos(0, 2)).draw(conf.gridCol);

		// 移動したコマの元の場所を表示
		for (int i = 0; i < 2; ++i) for (int j = 0; j < 3; ++j) {
			const auto &p = s.pieces[i][j];
			if (p.prevX >= 0) {
				Circle(conf.gridPos(p.prevX, p.prevY), conf.pieceRad).drawFrame(2, 0, conf.pieceCol[i]);
				Line(conf.gridPos(p.prevX, p.prevY), conf.gridPos(p.x, p.y)).draw(conf.pieceCol[i]);
			}
		}

		// 盤面上や置く前のコマを表示
		for (int i = 0; i < 2; ++i) for (int j = 0; j < 3; ++j) {
			const Color col = (holdingPlayer == i && holdingPiece == j) ? conf.pieceColHold[i] : conf.pieceCol[i];
			const auto &p = s.pieces[i][j];
			if (p.x < 0) {
				Circle(conf.piecePos[i][j], conf.pieceRad).draw(col);
			}
			else {
				Circle(conf.gridPos(p.x, p.y), conf.pieceRad).draw(col);
			}
		}

		// どちらの番かを表示
		Circle({ conf.turnPos.x + conf.pieceRad, conf.turnPos.y + conf.pieceRad }, conf.pieceRad).draw(conf.pieceCol[s.playing]);
		if (s.finished) {
			conf.turnFont(L"の勝ち！").draw({ conf.turnPos.x + conf.pieceRad * 2 + 1, conf.turnPos.y }, conf.turnCol);
		}
		else {
			conf.turnFont(L"の番").draw({ conf.turnPos.x + conf.pieceRad * 2 + 1, conf.turnPos.y }, conf.turnCol);
		}

		// マウスカーソルの位置に移動中のコマを描画
		if (holdingPiece >= 0) {
			Point pos = mouse;
			for (int y = 0; y < 3; ++y) for (int x = 0; x < 3; ++x) {
				if (Circle(conf.gridPos(x, y), conf.pieceRad).contains(mouse)
					&& game.checkMove(holdingPlayer, holdingPiece, x, y) == Morris::ok) {
					pos = conf.gridPos(x, y);
				}
			}
			Circle(pos, conf.pieceRad).draw(conf.pieceCol[holdingPlayer]);
		}
	}
}

