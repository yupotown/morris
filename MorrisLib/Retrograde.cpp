#include "Retrograde.h"

namespace Morris {

	Retrograde::Retrograde(const Graph & graph)
		: g(graph)
	{
	}

	void Retrograde::analyze()
	{
		winners.assign(g.size(), draw);

		// 既に勝っている状態
		std::set<int> alreadyWins[2];
		for (int i = 0; i < g.size(); ++i) {
			State::Vertex w = g[i].winner();
			if (w == State::red) {
				alreadyWins[0].insert(i);
				winners[i] = red;
			}
			else if (w == State::blue) {
				alreadyWins[1].insert(i);
				winners[i] = blue;
			}
		}

		// 後退解析
		innerAnalyze(red, alreadyWins[0]);
		innerAnalyze(blue, alreadyWins[1]);
	}

	const Retrograde::Winner & Retrograde::at(int index) const
	{
		return winners.at(index);
	}

	const Retrograde::Winner & Retrograde::operator[](int index) const
	{
		return winners[index];
	}

	const Graph & Retrograde::graph() const
	{
		return g;
	}

	Retrograde::Retrograde(const Retrograde & obj)
		: g(obj.g)
	{
	}

	const Retrograde &Retrograde::operator=(const Retrograde & rhs)
	{
		return *this;
	}

	void Retrograde::innerAnalyze(Winner w, const std::set<int>& already)
	{
		std::set<int> ss[2]; // ss[0]: 相手の番で確実に勝てる状態, ss[1]: 自分の番で確実に勝てる状態
		std::vector<bool> vted(g.size(), false);
		for (auto it = already.begin(); it != already.end(); ++it) {
			ss[0].insert(*it);
			vted[*it] = true;
		}
		while (ss[0].size() != 0) {

			// 自分の番で、次に勝てる状態が1つでもあれば勝てる状態
			for (auto it = ss[0].begin(); it != ss[0].end(); ++it) {
				const std::set<State> prev = g.getPrev(g[*it]);
				for (auto it2 = prev.begin(); it2 != prev.end(); ++it2) {
					const State s = (*it2);
					const int si = g.getIndexOfAll(s);
					if (vted[si]) continue;
					vted[si] = true;
					winners[si] = w;
					ss[1].insert(si);
				}
			}
			ss[0].clear();

			// 相手の番で、次の状態がすべて勝てる状態なら勝てる状態
			std::set<int> parent;
			for (auto it = ss[1].begin(); it != ss[1].end(); ++it) {
				const std::set<State> prev = g.getPrev(g[*it]);
				for (auto it2 = prev.begin(); it2 != prev.end(); ++it2) {
					const State si = g.getIndexOfAll(*it2);
					if (!vted[si]) parent.insert(si);
				}

			}
			for (auto it = parent.begin(); it != parent.end(); ++it) {
				const int si = (*it);
				const State s = g[si];
				bool f = true;
				const std::set<State> next = g.getNext(s);
				for (auto it2 = next.begin(); it2 != next.end(); ++it2) {
					f &= (winners[g.getIndexOfAll(*it2)] == w);
				}
				if (f) {
					vted[si] = true;
					winners[si] = w;
					ss[0].insert(si);
				}
			}
			ss[1].clear();

		}
	}

}
