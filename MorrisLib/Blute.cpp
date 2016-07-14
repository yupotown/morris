#include "Blute.h"

#include <fstream>

#define REP_VTX(x, y) for (int y = 0; y < 3; ++y) for (int x = 0; x < 3; ++x)

namespace Morris {

	Morris::Blute::Blute()
	{
	}

	void Blute::makeGraph()
	{
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

				if (s.winner() != State::blank) continue;

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

		all.clear();
		allr.clear();
		int index = 0;
		for (auto it = res.begin(); it != res.end(); ++it) {
			all.push_back(it->first);
			allr.insert(std::make_pair(it->first, index));
			++index;
		}

		const int n = all.size();
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

	const std::vector<State>& Blute::getAll() const
	{
		return all;
	}

	const std::size_t Blute::getIndexOfAll(State s) const
	{
		return allr.at(s);
	}

	const std::set<State> Blute::getNext(State s) const
	{
		std::set<State> res;
		getNext(s, res);
		return res;
	}

	void Blute::getNext(State s, std::set<State>& res) const
	{
		const int si = allr.at(s.minimize());
		for (std::size_t i = 0; i < es[si].size(); ++i) {
			res.insert(all[es[si][i]]);
		}
	}

	const std::set<State> Blute::getPrev(State s) const
	{
		std::set<State> res;
		getPrev(s, res);
		return res;
	}

	void Blute::getPrev(State s, std::set<State>& res) const
	{
		const int si = allr.at(s.minimize());
		for (std::size_t i = 0; i < esr[si].size(); ++i) {
			res.insert(all[esr[si][i]]);
		}
	}

	Morris::Blute::Blute(const Blute &)
	{
	}

	const Blute & Morris::Blute::operator=(const Blute &)
	{
		return *this;
	}

	void Blute::readAllState(const std::string & path)
	{
		readAllState(std::ifstream(path));
	}

	void Blute::readAllState(std::istream & is)
	{
		all.clear();
		allr.clear();

		int buf;
		int index = 0;
		while (true) {
			is.read(reinterpret_cast<char *>(&buf), sizeof(int));
			if (is.eof()) break;
			all.push_back(buf);
			allr.insert(std::make_pair(buf, index));
			++index;
		}
	}

	void Blute::writeAllState(const std::string & path)
	{
		writeAllState(std::ofstream(path));
	}

	void Blute::writeAllState(std::ostream & os)
	{
		for (std::size_t i = 0; i < all.size(); ++i) {
			os.write(reinterpret_cast<const char *>(&all[i].v), sizeof(int));
		}
	}

	void Blute::readGraph(const std::string & path)
	{
		readGraph(std::ifstream(path));
	}

	void Blute::readGraph(std::istream & is)
	{
		const int n = all.size();
		es.assign(n, std::vector<int>());
		esr.assign(n, std::vector<int>());
		int cnt, buf;
		for (int i = 0; i < n; ++i) {
			is.read(reinterpret_cast<char *>(&cnt), sizeof(int));
			for (int j = 0; j < cnt; ++j) {
				is.read(reinterpret_cast<char *>(&buf), sizeof(int));
				es[i].push_back(buf);
				esr[buf].push_back(i);
			}
		}
	}

	void Blute::writeGraph(const std::string & path) const
	{
		writeGraph(std::ofstream(path));
	}

	void Blute::writeGraph(std::ostream & os) const
	{
		const int n = all.size();
		for (int i = 0; i < n; ++i) {
			os.write(reinterpret_cast<const char *>(&i), sizeof(int));
			for (std::size_t j = 0; j < es[i].size(); ++j) {
				os.write(reinterpret_cast<const char *>(&es[i][j]), sizeof(int));
			}
		}
	}

}
