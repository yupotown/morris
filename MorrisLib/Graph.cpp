#include "Graph.h"

#include <fstream>

#define REP_VTX(x, y) for (int y = 0; y < 3; ++y) for (int x = 0; x < 3; ++x)

namespace Morris {

	Morris::Graph::Graph()
	{
	}

	void Graph::makeGraph()
	{
		std::set<State> ss[2];
		std::map<State, std::set<State>> res;
		int ssi = 0;

		res.insert(std::make_pair(State(0), std::set<State>()));
		ss[ssi].insert(State(0));

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

				if (s.winner() != State::blank) continue;

				State s2 = s.turn(t2);

				if (ph == State::put) {
					if (t == State::second) {
						int pc[4] = { 0, 0, 0, 0 };
						REP_VTX(x, y) ++pc[s.vertex({ x, y })];
						if (pc[State::blue] == 2) s2 = s2.phase(State::move1);
					}
					REP_VTX(x, y) if (s2.vertex({ x, y }) == State::blank) {
						const State s3 = s2.vertex({ x, y }, v).minimize();
						res[s].insert(s3); ss[ssi].insert(s3);
					}
				}

				if (ph >= State::move1) {
					if (ph == State::move1) s2 = s2.phase(State::move2);
					else if (ph == State::move2) s2 = s2.phase(State::move3);
					REP_VTX(x, y) if (s2.vertex({ x, y }) == v) {
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

		all.clear();
		allr.clear();
		int index = 0;
		for (auto it = res.begin(); it != res.end(); ++it) {
			all.push_back(it->first);
			allr.insert(std::make_pair(it->first, index));
			++index;
		}

		const std::size_t n = all.size();
		es.assign(n, std::vector<int>());
		esr.assign(n, std::vector<int>());
		for (auto it = res.begin(); it != res.end(); ++it) {
			const State &s = it->first;
			const std::set<State> &s2s = it->second;
			const int si = allr[s];
			for (auto it2 = s2s.begin(); it2 != s2s.end(); ++it2) {
				const int si2 = allr[*it2];
				es[si].push_back(si2);
				esr[si2].push_back(si);
			}
		}
	}

	const std::vector<State>& Graph::getAll() const
	{
		return all;
	}

	std::size_t Graph::size() const
	{
		return all.size();
	}

	const State & Graph::at(int index) const
	{
		return all.at(index);
	}

	State & Graph::at(int index)
	{
		return all.at(index);
	}

	const State & Graph::operator[](int index) const
	{
		return all[index];
	}

	State & Graph::operator[](int index)
	{
		return all[index];
	}

	const int Graph::getIndexOfAll(State s) const
	{
		return static_cast<int>(allr.at(s));
	}

	const std::set<State> Graph::getNext(State s) const
	{
		std::set<State> res;
		getNext(s, res);
		return res;
	}

	void Graph::getNext(State s, std::set<State>& res) const
	{
		const int si = allr.at(s.minimize());
		for (std::size_t i = 0; i < es[si].size(); ++i) {
			res.insert(all[es[si][i]]);
		}
	}

	const std::set<State> Graph::getPrev(State s) const
	{
		std::set<State> res;
		getPrev(s, res);
		return res;
	}

	void Graph::getPrev(State s, std::set<State>& res) const
	{
		const int si = allr.at(s.minimize());
		for (std::size_t i = 0; i < esr[si].size(); ++i) {
			res.insert(all[esr[si][i]]);
		}
	}

	Morris::Graph::Graph(const Graph &)
	{
	}

	const Graph & Morris::Graph::operator=(const Graph &)
	{
		return *this;
	}

	void Graph::readAllState(const std::string & path)
	{
		readAllState(std::ifstream(path, std::ios::binary));
	}

	void Graph::readAllState(std::istream & is)
	{
		all.clear();
		allr.clear();

		State s;
		int index = 0;
		while (true) {
			is.read(reinterpret_cast<char *>(&s.v), sizeof(int));
			if (is.eof()) break;
			all.push_back(s);
			allr.insert(std::make_pair(s, index));
			++index;
		}
	}

	void Graph::writeAllState(const std::string & path)
	{
		writeAllState(std::ofstream(path, std::ios::binary));
	}

	void Graph::writeAllState(std::ostream & os)
	{
		for (std::size_t i = 0; i < all.size(); ++i) {
			os.write(reinterpret_cast<const char *>(&all[i].v), sizeof(int));
		}
	}

	void Graph::readGraph(const std::string & path)
	{
		readGraph(std::ifstream(path, std::ios::binary));
	}

	void Graph::readGraph(std::istream & is)
	{
		const std::size_t n = all.size();
		es.assign(n, std::vector<int>());
		esr.assign(n, std::vector<int>());
		int cnt;
		State s1, s2;
		for (std::size_t i = 0; i < n; ++i) {
			is.read(reinterpret_cast<char *>(&s1.v), sizeof(int));
			const int si = allr.at(s1);
			is.read(reinterpret_cast<char *>(&cnt), sizeof(int));
			for (int j = 0; j < cnt; ++j) {
				is.read(reinterpret_cast<char *>(&s2.v), sizeof(int));
				const int si2 = allr.at(s2);
				es[si].push_back(si2);
				esr[si2].push_back(si);
			}
		}
	}

	void Graph::writeGraph(const std::string & path) const
	{
		writeGraph(std::ofstream(path, std::ios::binary));
	}

	void Graph::writeGraph(std::ostream & os) const
	{
		const std::size_t n = all.size();
		for (std::size_t i = 0; i < n; ++i) {
			const State s = all[i];
			os.write(reinterpret_cast<const char *>(&s.v), sizeof(int));
			const std::size_t cnt = es[i].size();
			os.write(reinterpret_cast<const char *>(&cnt), sizeof(int));
			for (std::size_t j = 0; j < cnt; ++j) {
				const State s2 = all[es[i][j]];
				os.write(reinterpret_cast<const char *>(&s2.v), sizeof(int));
			}
		}
	}

}
