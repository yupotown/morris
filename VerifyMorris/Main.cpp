
# include <Siv3D.hpp>
#include <set>
#include <list>
#include <map>
#include <queue>
#include <vector>
#include "../MorrisLib/State.hpp"
#include "../MorrisLib/Graph.h"
#include "../MorrisLib/Retrograde.h"

using namespace Morris;

#pragma region 画像

void saveImage(State s, String path) {
	const State::Phase ph = s.phase();
	const State::LastMove lmf = s.lastMove(State::first), lms = s.lastMove(State::second);
	const State::Turn t = s.turn();

	Image img(51, 57, { 255, 255, 255 });
	const Color col[4] = { { 128, 128, 128 },{ 255, 0, 0 },{ 0, 0, 255 },{ 0, 0, 0 } };
	Rect(0, 0, img.width, 5).overwrite(img, col[t + 1]);

	Circle c[3][3];
	for (int y = 0; y < 3; ++y) for (int x = 0; x < 3; ++x) c[x][y] = { { 5 + x * 20, 11 + y * 20}, 6 };

	int gf = -1, gs = -1;
	if (ph >= State::move2) gf = lmf;
	if (ph >= State::move3) gs = lms;
	for (int i = 0; i < 16; ++i) {
		const State::Edge e = static_cast<State::Edge>(i);
		const State::EdgeEnds ee = State::edgeEnds(e);
		const Vec2 a = c[ee.a.x()][ee.a.y()].center, b = c[ee.b.x()][ee.b.y()].center;
		const Vec2 u = { a.y - b.y, b.x - a.x };
		const Vec2 w = (1 / Sqrt(u.x * u.x + u.y * u.y)) * u;
		Quad(a + w, b + w, b - w, a - w).overwrite(img, (i == gf) ? col[1] : (i == gs) ? col[2] : col[0]);
	}

	for (int y = 0; y < 3; ++y) for (int x = 0; x < 3; ++x) {
		const auto v = s.vertex({ x, y });
		c[x][y].overwrite(img, col[v]);
	}

	img.save(path);
}

#pragma endregion

#pragma region HTML

// グラフ構造をリンクにした HTML を必勝手を明示して生成する。
void generateGraphHtml(const Retrograde &analyzer) {
	const Graph &g = analyzer.graph();
	std::vector<bool> vted(g.getAll().size(), false);
	std::queue<State> q;
	q.push(0);
	vted[g.getIndexOfAll(0)] = true;
	auto col = [&](int si) { return (analyzer[si] == Retrograde::red) ? L"#ff0000" : (analyzer[si] == Retrograde::blue) ? L"#0000ff" : L"#808080"; };
	while (!q.empty()) {
		State s = q.front(); q.pop();
		saveImage(s, Format(L"out/img/", s.v, L".png"));
		TextWriter writer(Format(L"out/html/", s.v, L".html"));
		const int si = g.getIndexOfAll(s);
		writer.writeln(L"<html>");
		writer.writeln(L"<head><title>", s.v, L"</title></head>");
		writer.writeln(L"<body>");
		writer.writeln(L"<img src=\"../img/", s.v, L".png\" style=\"border: solid 3px ", col(si), L";\" />");
		writer.writeln(L"<hr />");
		const std::set<State> next = g.getNext(s);
		for (auto it = next.begin(); it != next.end(); ++it) {
			const State s2 = (*it);
			const int si2 = g.getIndexOfAll(s2);
			writer.write(L"<a href=\"", s2.v, L".html\" title=\"", s2.v, L"\">");
			writer.write(L"<img src=\"../img/", s2.v, L".png\" style=\"border: solid 3px ", col(si2), L";\" />");
			writer.writeln(L"</a>");
			if (!vted[si2]) {
				vted[si2] = true;
				q.push(s2);
			}
		}
		writer.writeln(L"</body>");
		writer.writeln(L"</html>");
		writer.close();
	}
}

#pragma endregion

void Main()
{
	Graph g;
	State s = 0;

	//g.makeGraph();
	//g.writeAllState("out/all");
	//g.writeGraph("out/graph");
	g.readAllState("out/all");
	g.readGraph("out/graph");

	Retrograde analyzer(g);
	analyzer.analyze();

	generateGraphHtml(analyzer);

	#pragma region GUI 準備

	const Color colPiece[4] = {
		{ 255, 255, 255, 255 },
		{ 255, 0, 0, 255 },
		{ 0, 0, 255, 255 },
		{ 0, 0, 0, 0 }
	};
	const Color colTurn[2] = { colPiece[State::red], colPiece[State::blue] };

	// マス
	Circle c[3][3];
	for (int y = 0; y < 3; ++y) for (int x = 0; x < 3; ++x) c[x][y] = { { 50 + x * 100, 50 + y * 100 }, 20 };

	// グリッド
	Quad grid[16];
	for (int i = 0; i < 16; ++i) {
		const State::EdgeEnds ee = State::edgeEnds(static_cast<State::Edge>(i));
		Vec2 a = c[ee.a.x()][ee.a.y()].center, b = c[ee.b.x()][ee.b.y()].center;
		Vec2 v = b - a, t = { -v.y, v.x };
		double norm = Sqrt(v.x * v.x + v.y * v.y);
		Vec2 u = (5 / norm) * t;
		grid[i] = { a + u, b + u, b - u, a - u };
	}
	Color gridCol[3] = { { 255, 255, 255, 64 }, { 255, 0, 0, 255 }, { 0, 0, 255, 255 } };

	// ターン、フェーズ
	GUI gui(GUIStyle::Default);
	gui.setPos({ 350, 30 });
	gui.setTitle(L"");
	gui.add(L"turn", GUIRadioButton::Create({ L"先攻(赤)", L"後攻(青)" }, 0u));
	gui.add(L"phase", GUIRadioButton::Create({ L"置く", L"動かす(1)", L"動かす(2)", L"動かす" }, 0u));

	#pragma endregion

	while (System::Update())
	{
		// グリッド
		int gf = -1, gs = -1;
		if (s.phase() >= State::move2) gf = s.lastMove(State::first);
		if (s.phase() >= State::move3) gs = s.lastMove(State::second);
		for (int i = 0; i < 16; ++i) {
			grid[i].draw((i == gf) ? gridCol[1] : (i == gs) ? gridCol[2] : gridCol[0]);
		}

		// コマ
		bool pc = false;
		for (int y = 0; y < 3; ++y) for (int x = 0; x < 3; ++x) {
			const State::Vertex v = s.vertex({ x, y });
			c[x][y].draw(colPiece[v]);
			if (c[x][y].leftClicked) {
				pc = true;
				s = s.vertex({ x, y }, v == State::red ? State::blank : State::red);
			}
			if (c[x][y].rightClicked) {
				pc = true;
				s = s.vertex({ x, y }, v == State::blue ? State::blank : State::blue);
			}
		}

		// グリッド(操作)
		if (!pc && s.phase() >= State::move2) for (int i = 0; i < 16; ++i) {
			if (grid[i].leftClicked) s = s.lastMove(State::first, static_cast<State::LastMove>(i));
		}
		if (!pc && s.phase() >= State::move3) for (int i = 0; i < 16; ++i) {
			if (grid[i].rightClicked) s = s.lastMove(State::second, static_cast<State::LastMove>(i));
		}

		// ターン
		if (gui.radioButton(L"turn").hasChanged) {
			s = s.turn(static_cast<State::Turn>(gui.radioButton(L"turn").checkedItem.value()));
		}

		// フェーズ
		if (gui.radioButton(L"phase").hasChanged) {
			const State::Phase ph = static_cast<State::Phase>(gui.radioButton(L"phase").checkedItem.value());
			s = s.phase(ph);
			if (ph < State::move2) s = s.lastMove(State::first, State::e0010);
			if (ph < State::move3) s = s.lastMove(State::second, State::e0010);
		}
	}
}
