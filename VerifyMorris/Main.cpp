
# include <Siv3D.hpp>
#include <set>
#include <list>
#include <map>
#include <queue>
#include <vector>
#include "../MorrisLib/State.hpp"
#include "../MorrisLib/Blute.h"

using namespace Morris;

Blute blute;

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

#pragma region グラフ解析

// 先手/後手が既に勝っている状態のリスト
std::vector<State> win1[2];

// 先手/後手が確実に勝てる状態のリスト
std::set<State> win[2];

enum Winner {
	unknown = 0,
	fwin = 1,
	swin = 2,
};

// 先手/後手が確実に勝てるか
std::vector<Winner> winners;

// 既に勝っている状態のリストを生成する。
void makeWinnersList1() {
	win1[State::first].clear();
	win1[State::second].clear();
	winners.assign(blute.getAll().size(), unknown);
	for (std::size_t si = 0; si < blute.getAll().size(); ++si) {
		State::Vertex winner = blute.getAll()[si].winner();
		if (winner == State::red) {
			win1[State::first].push_back(si);
			winners[si] = fwin;
		}
		if (winner == State::blue) {
			win1[State::second].push_back(si);
			winners[si] = swin;
		}
	}
}

// 確実に勝てる状態のリストを生成する。
void makeWinnersList(State::Player pl) {
	const Winner w = (pl == State::first) ? fwin : swin;
	win[pl].clear();
	std::set<int> ss[2]; // ss[0]: 相手の番で確実に勝てる状態, ss[1]: 自分の番で確実に勝てる状態
	std::vector<bool> vted(blute.getAll().size(), false);
	for (std::size_t i = 0; i < win1[pl].size(); ++i) {
		ss[0].insert(win1[pl][i]);
		vted[win1[pl][i]] = true;
	}
	while (ss[0].size() != 0) {
		// 自分の番で、次に勝てる状態が1つでもあれば勝てる状態
		for (auto it = ss[0].begin(); it != ss[0].end(); ++it) {
			const std::set<State> prev = blute.getPrev(blute.getAll()[*it]);
			for (auto it2 = prev.begin(); it2 != prev.end(); ++it2) {
				const State s = (*it2);
				const int si = blute.getIndexOfAll(s);
				if (vted[si]) continue;
				vted[si] = true;
				winners[si] = w;
				win[pl].insert(si);
				ss[1].insert(si);
			}
		}
		ss[0].clear();

		// 相手の番で、次の状態がすべて勝てる状態なら勝てる状態
		std::set<int> parent;
		for (auto it = ss[1].begin(); it != ss[1].end(); ++it) {
			const std::set<State> prev = blute.getPrev(blute.getAll()[*it]);
			for (auto it2 = prev.begin(); it2 != prev.end(); ++it2) {
				const State si = blute.getIndexOfAll(*it2);
				if (!vted[si]) parent.insert(si);
			}

		}
		for (auto it = parent.begin(); it != parent.end(); ++it) {
			const int si = (*it);
			const State s = blute.getAll()[si];
			bool f = true;
			const std::set<State> next = blute.getNext(s);
			for (auto it2 = next.begin(); it2 != next.end(); ++it2) {
				f &= (winners[blute.getIndexOfAll(*it2)] == w);
			}
			if (f) {
				vted[si] = true;
				winners[si] = w;
				win[pl].insert(si);
				ss[0].insert(si);
			}
		}
		ss[1].clear();
	}
}

// 確実に勝てる状態のリストを生成する。
void makeWinnersList() {
	makeWinnersList1();
	makeWinnersList(State::first);
	makeWinnersList(State::second);
}

#pragma endregion

#pragma region HTML

// グラフ構造をリンクにした HTML を必勝手を明示して生成する。
void generateGraphHtml() {
	std::vector<bool> vted(blute.getAll().size(), false);
	std::queue<State> q;
	q.push(0);
	vted[blute.getIndexOfAll(0)] = true;
	auto col = [&](int si) { return (winners[si] == 1) ? L"#ff0000" : (winners[si] == 2) ? L"#0000ff" : L"#808080"; };
	while (!q.empty()) {
		State s = q.front(); q.pop();
		saveImage(s, Format(L"out/img/", s.v, L".png"));
		TextWriter writer(Format(L"out/html/", s.v, L".html"));
		const int si = blute.getIndexOfAll(s);
		writer.writeln(L"<html>");
		writer.writeln(L"<head><title>", s.v, L"</title></head>");
		writer.writeln(L"<body>");
		writer.writeln(L"<img src=\"../img/", s.v, L".png\" style=\"border: solid 3px ", col(si), L";\" />");
		writer.writeln(L"<hr />");
		const std::set<State> next = blute.getNext(s);
		for (auto it = next.begin(); it != next.end(); ++it) {
			const State s2 = (*it);
			const int si2 = blute.getIndexOfAll(s2);
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
	State s = 0;

	blute.makeGraph();
	blute.writeAllState("out/all");
	blute.writeGraph("out/graph");
	//blute.readAllState("out/all");
	//blute.readGraph("out/graph");
	makeWinnersList();
	generateGraphHtml();

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
