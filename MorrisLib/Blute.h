#pragma once

#include "State.hpp"
#include <iostream>
#include <map>
#include <set>
#include <string>
#include <vector>

namespace Morris {

	class Blute {
	public:
		Blute();

		/// <summary>
		/// すべての状態を総当たりして(最小化された状態の)状態遷移グラフを生成する。
		/// </summary>
		void makeGraph();

		/// <summary>
		/// すべての(最小化された)状態のリストを取得する。
		/// </summary>
		const std::vector<State> &getAll() const;

		/// <summary>
		/// ある状態(を最小化した状態)が getAll のどこにあるかを取得する。
		/// </summary>
		const std::size_t getIndexOfAll(State s) const;

		/// <summary>
		/// ある状態から1手で遷移できる(最小化された)状態の一覧を取得する。
		/// </summary>
		const std::set<State> getNext(State s) const;

		/// <summary>
		/// ある状態から1手で遷移できる(最小化された)状態の一覧を取得する。
		/// </summary>
		void getNext(State s, std::set<State> &res) const;

		/// <summary>
		/// ある状態に1手で遷移できる(最小化された)状態の一覧を取得する。
		/// </summary>
		const std::set<State> getPrev(State s) const;

		/// <summary>
		/// ある状態に1手で遷移できる(最小化された)状態の一覧を取得する。
		/// </summary>
		void getPrev(State s, std::set<State> &res) const;

		#pragma region ファイル読み書き

		/// <summary>
		/// すべての状態一覧をファイルから読み込む。
		/// </summary>
		void readAllState(const std::string &path);

		/// <summary>
		/// すべての状態一覧をファイルから読み込む。
		/// </summary>
		void readAllState(std::istream &is);

		/// <summary>
		/// すべての状態一覧をファイルに書き込む。
		/// </summary>
		void writeAllState(const std::string &path);

		/// <summary>
		/// すべての状態一覧をファイルに書き込む。
		/// </summary>
		void writeAllState(std::ostream &os);

		/// <summary>
		/// 状態遷移グラフをファイルから読み込む。
		/// </summary>
		void readGraph(const std::string &path);

		/// <summary>
		/// 状態遷移グラフをファイルから読み込む。
		/// </summary>
		void readGraph(std::istream &is);

		/// <summary>
		/// 状態遷移グラフをファイルに書き込む。
		/// </summary>
		void writeGraph(const std::string &path) const;

		/// <summary>
		/// 状態遷移グラフをファイルに書き込む。
		/// </summary>
		void writeGraph(std::ostream &os) const;

		#pragma endregion

	private:
		Blute(const Blute &obj);
		const Blute &operator =(const Blute &rhs);

		std::vector<State> all;
		std::map<State, int> allr;
		std::vector<std::vector<int>> es, esr;
	};

}
