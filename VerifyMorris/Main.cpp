
# include <Siv3D.hpp>
#include <set>
#include <list>
#include <map>
#include <queue>
#include <vector>
#include "../MorrisLib/State.hpp"

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

#pragma region 勝ち負け

std::map<int, int> winnerMap;

void saveWinner() {
	BinaryWriter writer(L"out/winner");
	for (auto it = winnerMap.begin(); it != winnerMap.end(); ++it) {
		writer.write(it->first);
		writer.write(it->second);
	}
}

void loadWinner() {
	BinaryReader reader(L"out/winner");
	int s, v;
	while (reader.read(s)) {
		reader.read(v);
		winnerMap.insert(std::make_pair(s, v));
	}
}

// 0: なし, 1: 先手(赤)の勝ち, 2: 後手(青)の勝ち
int winner(State s) {
	if (winnerMap.count(s) != 0) return winnerMap[s];
	bool ps[4][3][3] = {};
	for (int y = 0; y < 3; ++y) for (int x = 0; x < 3; ++x) ps[s.vertex({ x, y })][x][y] = true;
	int res = 0;
	for (int i = 1; i <= 2; ++i) {
		if ((ps[i][0][0] && ps[i][1][0] && ps[i][2][0])
			|| (ps[i][0][1] && ps[i][1][1] && ps[i][2][1])
			|| (ps[i][0][2] && ps[i][1][2] && ps[i][2][2])
			|| (ps[i][0][0] && ps[i][0][1] && ps[i][0][2])
			|| (ps[i][1][0] && ps[i][1][1] && ps[i][1][2])
			|| (ps[i][2][0] && ps[i][2][1] && ps[i][2][2])
			|| (ps[i][0][0] && ps[i][1][1] && ps[i][2][2])
			|| (ps[i][2][0] && ps[i][1][1] && ps[i][0][2])) {
			res = i;
		}
	}
	winnerMap.insert(std::make_pair(s, res));
	return res;
}

#pragma endregion

#pragma region 総当たり・グラフ生成

// 全状態(sorted)
std::vector<State> allss;

// 状態 -> allss のインデックス
std::map<State, int> allssr;

// 状態遷移グラフ(allss のインデックス)
std::vector<std::vector<int>> es, esr;

// すべての状態を列挙して状態遷移グラフを生成する。
void bluteState() {
	std::set<State> ss[2];
	std::map<State, std::set<State>> res;
	int ssi = 0;

	res.insert(std::make_pair(0, std::set<State>()));
	ss[ssi].insert(0);

	while (!ss[ssi].empty()) {
		ssi = (ssi + 1) % 2;
		ss[ssi].clear();

		for (auto it = ss[(ssi + 1) % 2].begin(); it != ss[(ssi + 1) % 2].end(); ++it) {
			const State s = (*it);
			const State::Turn t = s.turn(), t2 = t == State::first ? State::second : State::first;
			const State::Phase ph = s.phase();
			const State::Vertex v = t == State::first ? State::red : State::blue;
			const State::LastMove lm = s.lastMove(t);

			res.insert(std::make_pair(s, std::set<State>()));

			if (winner(s) != 0) continue;

			State s2 = s.turn(t2);

			if (ph == State::put) {
				if (t == State::second) {
					int pc[4] = { 0, 0, 0, 0 };
					for (int y = 0; y < 3; ++y) for (int x = 0; x < 3; ++x) ++pc[s.vertex({ x, y })];
					if (pc[State::blue] == 2) s2 = s2.phase(State::move1);
				}
				for (int y = 0; y < 3; ++y) for (int x = 0; x < 3; ++x) if (s2.vertex({ x, y }) == State::blank) {
					const State s3 = s2.vertex({ x, y }, v).minimize();
					res[s].insert(s3); ss[ssi].insert(s3);
				}
			}

			if (ph >= State::move1) {
				if (ph == State::move1) s2 = s2.phase(State::move2);
				else if (ph == State::move2) s2 = s2.phase(State::move3);
				for (int y = 0; y < 3; ++y) for (int x = 0; x < 3; ++x) if (s2.vertex({ x, y }) == v) {
					const State::Point a = { x, y };
					for (int i = 0; i < 16; ++i) {
						const State::Edge e = static_cast<State::Edge>(i);
						if (ph == State::move3 && e == lm) continue;
						const State::EdgeEnds ee = State::edgeEnds(e);
						State::Point b;
						if (ee.a == a) b = ee.b;
						else if (ee.b == a) b = ee.a;
						else continue;
						if (s2.vertex(b) != State::blank) continue;
						const State s3 = s2.lastMove(t, e).vertex(a, State::blank).vertex(b, v).minimize();
						if (res[s].count(s3) != 0) continue;
						res[s].insert(s3); ss[ssi].insert(s3);
					}
				}
			}
		}
	}

	allss.clear();
	allssr.clear();
	int index = 0;
	for (auto it = res.begin(); it != res.end(); ++it) {
		allss.push_back(it->first);
		allssr.insert(std::make_pair(it->first, index));
		++index;
	}

	const int n = allss.size();
	es.assign(n, std::vector<int>());
	esr.assign(n, std::vector<int>());
	for (auto it = res.begin(); it != res.end(); ++it) {
		const State &s = it->first;
		const std::set<State> &s2s = it->second;
		const int si = allssr[s];
		for (auto it2 = s2s.begin(); it2 != s2s.end(); ++it2) {
			const int si2 = allssr[*it2];
			es[si].push_back(si2);
			esr[si2].push_back(si);
		}
	}
}

// 全状態読み込み
void loadAllss() {
	allss.clear();
	allssr.clear();
	BinaryReader reader(L"out/all");
	int index = 0;
	for (int s; reader.read(s); ) {
		allss.push_back(s);
		allssr.insert(std::make_pair(s, index));
		++index;
	}
}

// 全状態保存
void saveAllss() {
	BinaryWriter writer(L"out/all");
	for (auto it = allss.begin(); it != allss.end(); ++it) writer.write(it->v);
}

#pragma endregion

#pragma region グラフ解析

// 先手/後手が既に勝っている状態のリスト
std::vector<int> win1[2];

// 先手/後手が確実に勝てる状態のリスト
std::set<int> win[2];

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
	winners.assign(allss.size(), unknown);
	for (auto it = winnerMap.begin(); it != winnerMap.end(); ++it) {
		const int si = allssr[it->first];
		if (it->second == 1) {
			win1[State::first].push_back(si);
			winners[si] = fwin;
		}
		if (it->second == 2) {
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
	std::vector<bool> vted(allss.size(), false);
	for (std::size_t i = 0; i < win1[pl].size(); ++i) {
		ss[0].insert(win1[pl][i]);
		vted[win1[pl][i]] = true;
	}
	while (ss[0].size() != 0) {
		// 自分の番で、次に勝てる状態が1つでもあれば勝てる状態
		for (auto it = ss[0].begin(); it != ss[0].end(); ++it) for (std::size_t i = 0; i < esr[*it].size(); ++i) {
			const int v = esr[*it][i];
			if (vted[v]) continue;
			vted[v] = true;
			winners[v] = w;
			win[pl].insert(v);
			ss[1].insert(v);
		}
		ss[0].clear();

		// 相手の番で、次の状態がすべて勝てる状態なら勝てる状態
		std::set<int> parent;
		for (auto it = ss[1].begin(); it != ss[1].end(); ++it) for (std::size_t i = 0; i < esr[*it].size(); ++i) {
			const int v = esr[*it][i];
			if (!vted[v]) parent.insert(v);
		}
		for (auto it = parent.begin(); it != parent.end(); ++it) {
			const int v = (*it);
			bool f = true;
			for (std::size_t i = 0; i < es[v].size(); ++i) {
				f &= (winners[es[v][i]] == w);
			}
			if (f) {
				vted[v] = true;
				winners[v] = w;
				win[pl].insert(v);
				ss[0].insert(v);
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
	std::vector<bool> vted(allss.size(), false);
	std::queue<State> q;
	q.push(0);
	vted[allssr[0]] = true;
	auto col = [&](int si) { return (winners[si] == 1) ? L"#ff0000" : (winners[si] == 2) ? L"#0000ff" : L"#808080"; };
	while (!q.empty()) {
		State s = q.front(); q.pop();
		saveImage(s, Format(L"out/img/", s.v, L".png"));
		TextWriter writer(Format(L"out/graph/html/", s.v, L".html"));
		const int si = allssr[s];
		writer.writeln(L"<html>");
		writer.writeln(L"<head><title>", s.v, L"</title></head>");
		writer.writeln(L"<body>");
		writer.writeln(L"<img src=\"../../img/", s.v, L".png\" style=\"border: solid 3px ", col(si), L";\" />");
		writer.writeln(L"<hr />");
		for (std::size_t i = 0; i < es[si].size(); ++i) {
			const int si2 = es[si][i];
			const State s2 = allss[si2];
			writer.write(L"<a href=\"", s2.v, L".html\" title=\"", s2.v, L"\">");
			writer.write(L"<img src=\"../../img/", s2.v, L".png\" style=\"border: solid 3px ", col(si2), L";\" />");
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

	bluteState();
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

	saveWinner();
}
