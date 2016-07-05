
# include <Siv3D.hpp>
#include <set>
#include <list>
#include <map>
#include <queue>
#include <vector>

// * マス (x, y) を数値 (y * 3 + x) に対応させる
// 状態		| フェーズ[2] | ターン[1] | 青の直前移動[4] | 赤の直前移動[4] | 盤面[18] |
// フェーズ	{ 置く, 動かす(1ターン目), 動かす(2ターン目), 動かす(3ターン目以降) }
// ターン	{ 赤, 青 }
// 直前移動	{ 01, 03, 04, 12, 14, 24, 25, 34, 36, 45, 46, 47, 48, 58, 67, 78 }
// 盤面		| マス8[2] | ... | マス0[2] |
// マス		{ なし, 赤, 青, (未使用) }

#pragma region コマ

enum Piece {
	blank = 0,
	red = 1,
	blue = 2,
};

Piece piece(int s, int x, int y) {
	const int shift = (y * 3 + x) * 2;
	return static_cast<Piece>((s >> shift) & 3);
}

int setPiece(int s, int x, int y, Piece p) {
	const int shift = (y * 3 + x) * 2;
	s &= ~(3 << shift);
	s |= (static_cast<int>(p) << shift);
	return s;
}

#pragma endregion

#pragma region ターン

enum Turn {
	first = 0,
	second = 1,
};

Turn turn(int s) {
	return static_cast<Turn>((s >> 26) & 1);
}

int setTurn(int s, Turn t) {
	s &= ~(1 << 26);
	s |= (static_cast<int>(t) << 26);
	return s;
}

#pragma endregion

#pragma region フェーズ

enum Phase {
	put = 0,
	move1 = 1,
	move2 = 2,
	move = 3,
};

Phase phase(int s) {
	return static_cast<Phase>((s >> 27) & 3);
}

int setPhase(int s, Phase ph) {
	s &= ~(3 << 27);
	s |= (static_cast<int>(ph) << 27);
	return s;
}

#pragma endregion

#pragma region 直前移動

typedef int LastMove;
typedef Turn Player;

struct Move {
	int x1, y1, x2, y2;
};

Move lmTbl[16] = {
	{ 0, 0, 1, 0 },
	{ 0, 0, 0, 1 },
	{ 0, 0, 1, 1 },
	{ 1, 0, 2, 0 },
	{ 1, 0, 1, 1 },
	{ 2, 0, 1, 1 },
	{ 2, 0, 2, 1 },
	{ 0, 1, 1, 1 },
	{ 0, 1, 0, 2 },
	{ 1, 1, 2, 1 },
	{ 1, 1, 0, 2 },
	{ 1, 1, 1, 2 },
	{ 1, 1, 2, 2 },
	{ 2, 1, 2, 2 },
	{ 0, 2, 1, 2 },
	{ 1, 2, 2, 2 },
};

LastMove lastmove(int s, Player pl) {
	const int shift = 18 + static_cast<int>(pl) * 4;
	return (s >> shift) & 15;
}

int setLastmove(int s, Player pl, LastMove lm) {
	const int shift = 18 + static_cast<int>(pl) * 4;
	s &= ~(15 << shift);
	s |= (lm << shift);
	return s;
}

#pragma endregion

bool isValid(int s) {
	const Phase ph = phase(s);
	const Turn t = turn(s);
	const LastMove lmf = lastmove(s, first), lms = lastmove(s, second);

	// 最初の移動前の直前移動が 0 でない
	if (ph < move2 && lmf != 0) return false;
	if (ph < move && lms != 0) return false;

	// 直前移動の両側にそのコマがあるか、どちらにもない
	if (ph >= move2) {
		const Move &m = lmTbl[lmf];
		const Piece a = piece(s, m.x1, m.y1), b = piece(s, m.x2, m.y2);
		if (a != red && b != red) return false;
		if (a == red && b == red) return false;
	}
	if (ph >= move) {
		const Move &m = lmTbl[lms];
		const Piece a = piece(s, m.x1, m.y1), b = piece(s, m.x2, m.y2);
		if (a != blue && b != blue) return false;
		if (a == blue && b == blue) return false;
	}

	int pc[4] = { 0, 0, 0, 0 };
	for (int y = 0; y < 3; ++y) for (int x = 0; x < 3; ++x) {
		++pc[piece(s, x, y)];
	}

	// 未使用の値
	if (pc[3] != 0) return false;

	// コマが4つ以上
	if (pc[red] >= 4 || pc[blue] >= 4) return false;

	if (ph == put) {
		if (t == first) {
			// 置くフェーズで、赤の番に赤のコマと青のコマの個数が異なる
			if (pc[red] != pc[blue]) return false;
		}
		else {
			// 置くフェーズで、青の番に赤のコマの個数が青のコマの個数 + 1 になっていない
			if (pc[red] != pc[blue] + 1) return false;
		}
	}
	else {
		// 動かすフェーズでコマが3つずつになっていない
		if (pc[red] != 3 || pc[blue] != 3) return false;
	}

	return true;
}

#pragma region 画像

bool saveAsImage(int s, String path) {
	Image img(51, 57, { 255, 255, 255 });
	const Phase ph = phase(s);
	const LastMove lmf = lastmove(s, first), lms = lastmove(s, second);
	const Color col[4] = { { 128, 128, 128 },{ 255, 0, 0 },{ 0, 0, 255 },{ 0, 0, 0 } };
	Rect(0, 0, img.width, 5).overwrite(img, (turn(s) == first) ? col[1] : col[2]);
	Circle c[3][3];
	for (int y = 0; y < 3; ++y) for (int x = 0; x < 3; ++x) c[x][y] = { { 5 + x * 20, 11 + y * 20}, 6 };
	int gf = -1, gs = -1;
	if (ph >= move2) {
		gf = lmf;
		const Move &m = lmTbl[gf];
		const Piece a = piece(s, m.x1, m.y1), b = piece(s, m.x2, m.y2);
	}
	if (ph >= move) {
		gs = lms;
		const Move &m = lmTbl[gs];
		const Piece a = piece(s, m.x1, m.y1), b = piece(s, m.x2, m.y2);
	}
	for (int i = 0; i < 16; ++i) {
		const Move m = lmTbl[i];
		const Vec2 a = c[m.x1][m.y1].center, b = c[m.x2][m.y2].center;
		const Vec2 u = { a.y - b.y, b.x - a.x };
		const Vec2 t = (1 / Sqrt(u.x * u.x + u.y * u.y)) * u;
		Quad(a + t, b + t, b - t, a - t).overwrite(img, (i == gf) ? col[1] : (i == gs) ? col[2] : col[0]);
	}
	int pc[4] = { 0, 0, 0, 0 };
	for (int y = 0; y < 3; ++y) for (int x = 0; x < 3; ++x) {
		Piece p = piece(s, x, y);
		++pc[p];
		c[x][y].overwrite(img, col[p]);
	}
	img.save(path);
	return true;
}

#pragma endregion

#pragma region 回転と反転

#define rotrev(f, fx, fy, tbl) \
	int f(int s) { \
		int s2 = s; \
		for (int y = 0; y < 3; ++y) for (int x = 0; x < 3; ++x) s2 = setPiece(s2, fx, fy, piece(s, x, y)); \
		const Phase ph = phase(s2); \
		if (ph >= move2) s2 = setLastmove(s2, first, tbl[lastmove(s2, first)]); \
		if (ph >= move) s2 = setLastmove(s2, second, tbl[lastmove(s2, second)]); \
		return s2; \
	}

#define combineTbl(t2, t1) \
	{ \
		t2[t1[0]], t2[t1[1]], t2[t1[2]], t2[t1[3]], \
		t2[t1[4]], t2[t1[5]], t2[t1[6]], t2[t1[7]], \
		t2[t1[8]], t2[t1[9]], t2[t1[10]], t2[t1[11]], \
		t2[t1[12]], t2[t1[13]], t2[t1[14]], t2[t1[15]], \
	}

// 何もしない
int rot0(int s) { return s; }

// 時計回りに90度回転
constexpr LastMove lm90Tbl[16] = { 6, 3, 5, 13, 9, 12, 15, 4, 0, 11, 2, 7, 10, 14, 1, 8 };
rotrev(rot90, 2 - y, x, lm90Tbl)

// 180度回転
constexpr LastMove lm180Tbl[16] = combineTbl(lm90Tbl, lm90Tbl);
rotrev(rot180, 2 - x, 2 - y, lm180Tbl)

// 270度回転
constexpr LastMove lm270Tbl[16] = combineTbl(lm180Tbl, lm90Tbl);
rotrev(rot270, y, 2 - x, lm270Tbl)

// 上下反転
constexpr LastMove lmDefTbl[16] = { 14, 8, 10, 15, 11, 12, 13, 7, 1, 9, 2, 4, 5, 6, 0, 3 };
rotrev(revDef, x, 2 - y, lmDefTbl)

// aei 反転
constexpr LastMove lmAeiTbl[16] = combineTbl(lm90Tbl, lmDefTbl);
rotrev(revAei, y, x, lmAeiTbl)

// 左右反転
constexpr LastMove lmBehTbl[16] = combineTbl(lm90Tbl, lmAeiTbl);
rotrev(revBeh, 2 - x, y, lmBehTbl)

// ceg 反転
constexpr LastMove lmCegTbl[16] = combineTbl(lm90Tbl, lmBehTbl);
rotrev(revCeg, 2 - y, 2 - x, lmCegTbl)

int(*allRotrev[8])(int) = { rot0, rot90, rot180, rot270, revDef, revAei, revBeh, revCeg };

// 回転や反転で得られる状態のうち表現が最小のものを返す。
int minimize(int s) {
	int res = s;
	for (int i = 1; i < 8; ++i) {
		int temp = allRotrev[i](s);
		if (temp < res) res = temp;
	}
	return res;
}

#pragma endregion

#pragma region HTML

template <typename Iterator>
void writeHtml(Iterator begin, Iterator end, String path, String title, std::size_t count) {
	TextWriter writer(path);
	writer.writeln(L"<html>");
	writer.writeln(L"<head><title>", title, L"</title></head>");
	writer.writeln(L"<body>");
	writer.writeln(count, L"<br />");
	for (auto it = begin; it != end; ++it) {
		writer.writeln(L"<a href=\"../img/", (*it), L".png\" title=\"", (*it), L"\"><img src=\"../img/", (*it), L".png\" /></a>");
		if (!FileSystem::Exists(Format(L"out/img/", (*it), L".png"))) {
			saveAsImage((*it), Format(L"out/img/", (*it), L".png"));
		}
	}
	writer.writeln(L"</body>");
	writer.writeln(L"</html>");
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

// 0: なし, 1: 赤の勝ち, 2: 青の勝ち, 3: 両方並んでる
int winner(int s) {
	if (winnerMap.count(s) != 0) return winnerMap[s];
	bool ps[4][3][3] = {};
	for (int y = 0; y < 3; ++y) for (int x = 0; x < 3; ++x) ps[piece(s, x, y)][x][y] = true;
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
			res += i;
		}
	}
	winnerMap.insert(std::make_pair(s, res));
	return res;
}

#pragma endregion

#pragma region 総当たり

// すべての状態の画像を生成する。
void generateAllImage(int n = (1 << 29)) {
	for (int i = 0; i < n; ++i) if (isValid(i)) {
		saveAsImage(i, Format(L"out/img/", i, L".png"));
	}
}

// 置くフェーズの状態をすべて生成する。
void blutePut() {
	std::set<int> ss[7];
	ss[0].insert(0);
	{
		BinaryWriter writer(L"out/blute/put0");
		for (auto it = ss[0].begin(); it != ss[0].end(); ++it) writer.write(*it);
	}
	writeHtml(ss[0].begin(), ss[0].end(), L"out/blute/put0.html", L"put0", ss[0].size());
	for (int i = 1; i <= 6; ++i) {
		const Turn t = (i % 2 == 0) ? first : second;
		const Piece next = (t == first) ? blue : red;
		for (auto it = ss[i - 1].begin(); it != ss[i - 1].end(); ++it) {
			int s = (*it);
			if (winner(s) != 0) continue;
			s = setTurn(s, t);
			s = setPhase(s, (i < 6) ? put : move1);
			for (int y = 0; y < 3; ++y) for (int x = 0; x < 3; ++x) {
				if (piece(s, x, y) != blank) continue;
				int s2 = setPiece(s, x, y, next);
				s2 = minimize(s2);
				ss[i].insert(s2);
			}
		}
		BinaryWriter writer(Format(L"out/blute/put", i));
		for (auto it = ss[i].begin(); it != ss[i].end(); ++it) writer.write(*it);
		writeHtml(ss[i].begin(), ss[i].end(), Format(L"out/blute/put", i, L".html"), Format(L"put", i), ss[i].size());
	}
}

// 動かすフェーズでありえる状態を列挙する。
void bluteMove() {
	std::set<int> res, ss[2];
	int ssi = 0;
	BinaryReader reader(L"out/blute/put6");
	for (int s; reader.read(s); ) {
		res.insert(s);
		if (winner(s) == 0) ss[ssi].insert(s);
	}
	while (ss[ssi].size() != 0) {
		ssi = (ssi + 1) % 2;
		ss[ssi].clear();
		for (auto it = ss[(ssi + 1) % 2].begin(); it != ss[(ssi + 1) % 2].end(); ++it) {
			int s = (*it);
			const Turn t = turn(s);
			const Phase ph = phase(s);
			const Piece tp = static_cast<Piece>(static_cast<int>(t) + 1);
			const LastMove lm = lastmove(s, t);
			for (int y = 0; y < 3; ++y) for (int x = 0; x < 3; ++x) {
				if (piece(s, x, y) != tp) continue;
				for (int i = 0; i < 16; ++i) {
					if (ph == move && i == lm) continue;
					const Move &m = lmTbl[i];
					int x2, y2;
					if (m.x1 == x && m.y1 == y) {
						x2 = m.x2;
						y2 = m.y2;
					}
					else if (m.x2 == x && m.y2 == y) {
						x2 = m.x1;
						y2 = m.y1;
					}
					else {
						continue;
					}
					if (piece(s, x2, y2) != blank) continue;
					// (x, y) のコマを (x2, y2) に動かす
					int s2 = s;
					s2 = setPiece(s2, x, y, blank);
					s2 = setPiece(s2, x2, y2, tp);
					s2 = setTurn(s2, (t == first) ? second : first);
					s2 = setLastmove(s2, t, i);
					if (ph == move1) s2 = setPhase(s2, move2);
					else if (ph == move2) s2 = setPhase(s2, move);
					s2 = minimize(s2);
					if (res.count(s2) == 0) {
						res.insert(s2);
						if (winner(s2) == 0) ss[ssi].insert(s2);
					}
				}
			}
		}
	}
	BinaryWriter writer(L"out/blute/move");
	for (auto it = res.begin(); it != res.end(); ++it) writer.write(*it);
	writeHtml(res.begin(), res.end(), L"out/blute/move.html", L"move", res.size());
}

// 全状態(sorted)
std::vector<int> allss;

// 状態 -> allss のインデックス
std::map<int, int> allssr;

// blutePut と bluteMove の結果から、可能な全状態のリストを作って保存する
void makeAllPutMove() {
	allss.clear();
	allssr.clear();
	std::set<int> ss;
	for (int i = 0; i <= 6; ++i) {
		BinaryReader reader(Format(L"out/blute/put", i));
		for (int s; reader.read(s); ss.insert(s));
	}
	BinaryReader reader(L"out/blute/move");
	for (int s; reader.read(s); ss.insert(s));
	BinaryWriter writer(L"out/all");
	int index = 0;
	for (auto it = ss.begin(); it != ss.end(); ++it) {
		writer.write(*it);
		allss.push_back(*it);
		allssr.insert(std::make_pair((*it), index));
		++index;
	}
	writeHtml(ss.begin(), ss.end(), L"out/debug/all.html", L"all", ss.size());
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
	for (auto it = allss.begin(); it != allss.end(); ++it) writer.write(*it);
}

#pragma endregion

#pragma region グラフ

// 状態遷移グラフ(allss のインデックス)
std::vector<std::vector<int>> es, esr;

// allss を読み込んでグラフを生成する。
void makeGraph() {
	loadWinner();
	loadAllss();

	const int n = allss.size();
	std::vector<bool> vted(n, false);
	es.assign(n, std::vector<int>());
	esr.assign(n, std::vector<int>());

	std::queue<int> q;
	q.push(allssr[0]);
	vted[allssr[0]] = true;

	while (!q.empty()) {
		const int si = q.front(); q.pop();
		const int s = allss[si];
		const Turn t = turn(s), t2 = (t == first) ? second : first;
		const Piece tp = static_cast<Piece>(t + 1);
		const Phase ph = phase(s);
		const LastMove lm = lastmove(s, t), lm2 = lastmove(s, t2);

		if (winner(s) != 0) continue;

		if (ph >= move1) {
			// 動かすフェーズ
			for (int y = 0; y < 3; ++y) for (int x = 0; x < 3; ++x) {
				if (piece(s, x, y) != tp) continue;
				for (int i = 0; i < 16; ++i) {
					if (ph == move && i == lm) continue;
					const Move &m = lmTbl[i];
					int x2, y2;
					if (m.x1 == x && m.y1 == y) {
						x2 = m.x2; y2 = m.y2;
					}
					else if (m.x2 == x && m.y2 == y) {
						x2 = m.x1; y2 = m.y1;
					}
					else {
						continue;
					}
					if (piece(s, x2, y2) != blank) continue;
					int s2 = setPiece(s, x, y, blank);
					s2 = setPiece(s2, x2, y2, tp);
					s2 = setTurn(s2, t2);
					s2 = setLastmove(s2, t, i);
					if (ph == move1) s2 = setPhase(s2, move2);
					else if (ph == move2) s2 = setPhase(s2, move);
					s2 = minimize(s2);
					const int si2 = allssr[s2];
					if (vted[si2]) continue;
					es[si].push_back(si2);
					esr[si2].push_back(si);
					q.push(si2);
					vted[si2] = true;
				}
			}
		}
		else {
			// 置くフェーズ
			int pc[4] = { 0, 0, 0, 0 };
			for (int y = 0; y < 3; ++y) for (int x = 0; x < 3; ++x) {
				++pc[piece(s, x, y)];
			}
			for (int y = 0; y < 3; ++y) for (int x = 0; x < 3; ++x) {
				if (piece(s, x, y) != blank) continue;
				int s2 = setPiece(s, x, y, tp);
				s2 = setTurn(s2, t2);
				if (t == second && pc[blue] == 2) s2 = setPhase(s2, move1);
				s2 = minimize(s2);
				const int si2 = allssr[s2];
				if (vted[si2]) continue;
				es[si].push_back(si2);
				esr[si2].push_back(si);
				q.push(si2);
				vted[si2] = true;
			}
		}
	}

	TextWriter writer(L"out/debug/es.txt");
	for (int i = 0; i < n; ++i) {
		writer.writeln(allss[i], L" {");
		for (std::size_t j = 0; j < es[i].size(); ++j) {
			writer.writeln(L"\t", allss[es[i][j]], L",");
		}
		writer.writeln(L"}");
	}
}

// グラフ構造をリンクにした HTML を生成する。
void generateGraphHtml(int s) {
	if (FileSystem::Exists(Format(L"out/graph/html/", s, L".html"))) return;
	TextWriter writer(Format(L"out/graph/html/", s, L".html"));
	writer.writeln(L"<html>");
	writer.writeln(L"<head><title>", s, L"</title></head>");
	writer.writeln(L"<body>");
	writer.writeln(L"<img src=\"../../img/", s, L".png\" />");
	writer.writeln(L"<hr />");
	const int si = allssr[s];
	for (std::size_t i = 0; i < es[si].size(); ++i) {
		const int si2 = es[si][i];
		const int s2 = allss[si2];
		writer.write(L"<a href=\"", s2, L".html\" title=\"", s2, L"\">");
		writer.write(L"<img src=\"../../img/", s2, L".png\" />");
		writer.writeln(L"</a>");
	}
	writer.writeln(L"</body>");
	writer.writeln(L"</html>");
	writer.close();
	for (std::size_t i = 0; i < es[si].size(); ++i) {
		generateGraphHtml(allss[es[si][i]]);
	}
}

#pragma endregion

void Main()
{
	int s = 0;

	makeGraph();
	generateGraphHtml(0);

	#pragma region GUI 準備

	const Color colPiece[4] = {
		{ 255, 255, 255, 255 },
		{ 255, 0, 0, 255 },
		{ 0, 0, 255, 255 },
		{ 0, 0, 0, 0 }
	};
	const Color colTurn[2] = { colPiece[red], colPiece[blue] };

	// スロット
	std::set<int> slot[10];
	Key keys[10];
	String slotPaths[10], slotHtmlPaths[10];
	for (int i = 0; i < 10; ++i) {
		keys[i] = Key(static_cast<wchar>(L'0' + i));
		slotPaths[i] = Format(L"out/slot/", i);
		slotHtmlPaths[i] = Format(L"out/slot/", i, L".html");
	}
	Font font(10);

	// マス
	Circle c[3][3];
	for (int y = 0; y < 3; ++y) for (int x = 0; x < 3; ++x) c[x][y] = { { 50 + x * 100, 50 + y * 100 }, 20 };

	// グリッド
	Quad grid[16];
	for (int i = 0; i < 16; ++i) {
		Move m = lmTbl[i];
		Vec2 a = c[m.x1][m.y1].center, b = c[m.x2][m.y2].center;
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
		// スロット
		for (int i = 0; i < 10; ++i) {
			if (keys[i].clicked) {
				if (slot[i].count(s) != 0) {
					slot[i].erase(s);
				}
				else {
					bool exists = false;
					for (auto it = slot[i].begin(); !exists && it != slot[i].end(); ++it) {
						for (int j = 0; j < 8; ++j) if (slot[i].count(allRotrev[j](s)) != 0) {
							exists = true;
							break;
						}
					}
					if (!exists) slot[i].insert(s);
				}
			}
			font(Format(slot[i].size())).draw({ i * 50, 0 });
		}

		// 保存
		if ((Input::KeyControl + Input::KeyS).clicked) {
			saveWinner();
			for (int i = 0; i < 10; ++i) {
				BinaryWriter writer(slotPaths[i]);
				writeHtml(slot[i].begin(), slot[i].end(), slotHtmlPaths[i], Format(i), static_cast<int>(slot[i].size()));
				for (auto it = slot[i].begin(); it != slot[i].end(); ++it) {
					writer.write(*it);
				}
			}
		}

		// 読み込み
		if ((Input::KeyControl + Input::KeyO).clicked) {
			loadWinner();
			int temp;
			for (int i = 0; i < 10; ++i) {
				slot[i].clear();
				BinaryReader reader(slotPaths[i]);
				while (reader.read<int>(temp)) slot[i].insert(temp);
			}
		}

		// グリッド
		int gf = -1, gs = -1;
		if (phase(s) >= move2) gf = lastmove(s, first);
		if (phase(s) >= move) gs = lastmove(s, second);
		for (int i = 0; i < 16; ++i) {
			grid[i].draw((i == gf) ? gridCol[1] : (i == gs) ? gridCol[2] : gridCol[0]);
		}

		// コマ
		bool pc = false;
		for (int y = 0; y < 3; ++y) for (int x = 0; x < 3; ++x) {
			Piece p = piece(s, x, y);
			c[x][y].draw(colPiece[p]);
			if (c[x][y].leftClicked) {
				pc = true;
				s = setPiece(s, x, y, (p == red) ? blank : red);
			}
			if (c[x][y].rightClicked) {
				pc = true;
				s = setPiece(s, x, y, (p == blue) ? blank : blue);
			}
		}

		// グリッド(操作)
		if (!pc && phase(s) >= move2) for (int i = 0; i < 16; ++i) {
			if (grid[i].leftClicked) s = setLastmove(s, first, i);
		}
		if (!pc && phase(s) >= move) for (int i = 0; i < 16; ++i) {
			if (grid[i].rightClicked) s = setLastmove(s, second, i);
		}

		// ターン
		if (gui.radioButton(L"turn").hasChanged) {
			s = setTurn(s, static_cast<Turn>(gui.radioButton(L"turn").checkedItem.value()));
		}

		// フェーズ
		if (gui.radioButton(L"phase").hasChanged) {
			Phase ph = static_cast<Phase>(gui.radioButton(L"phase").checkedItem.value());
			s = setPhase(s, ph);
			if (ph < move2) s = setLastmove(s, first, 0);
			if (ph < move) s = setLastmove(s, second, 0);
		}
	}

	saveWinner();
}
