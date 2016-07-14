#pragma once

#include "State.hpp"
#include <iostream>
#include <map>
#include <set>
#include <string>
#include <vector>

namespace Morris {

	class Graph {
	public:
		Graph();

		/// <summary>
		/// ���ׂĂ̏�Ԃ𑍓����肵��(�ŏ������ꂽ��Ԃ�)��ԑJ�ڃO���t�𐶐�����B
		/// </summary>
		void makeGraph();

		/// <summary>
		/// ���ׂĂ�(�ŏ������ꂽ)��Ԃ̃��X�g���擾����B
		/// </summary>
		const std::vector<State> &getAll() const;

		/// <summary>
		/// ������(���ŏ����������)�� getAll �̂ǂ��ɂ��邩���擾����B
		/// </summary>
		const std::size_t getIndexOfAll(State s) const;

		/// <summary>
		/// �����Ԃ���1��őJ�ڂł���(�ŏ������ꂽ)��Ԃ̈ꗗ���擾����B
		/// </summary>
		const std::set<State> getNext(State s) const;

		/// <summary>
		/// �����Ԃ���1��őJ�ڂł���(�ŏ������ꂽ)��Ԃ̈ꗗ���擾����B
		/// </summary>
		void getNext(State s, std::set<State> &res) const;

		/// <summary>
		/// �����Ԃ�1��őJ�ڂł���(�ŏ������ꂽ)��Ԃ̈ꗗ���擾����B
		/// </summary>
		const std::set<State> getPrev(State s) const;

		/// <summary>
		/// �����Ԃ�1��őJ�ڂł���(�ŏ������ꂽ)��Ԃ̈ꗗ���擾����B
		/// </summary>
		void getPrev(State s, std::set<State> &res) const;

		#pragma region �t�@�C���ǂݏ���

		/// <summary>
		/// ���ׂĂ̏�Ԉꗗ���t�@�C������ǂݍ��ށB
		/// </summary>
		void readAllState(const std::string &path);

		/// <summary>
		/// ���ׂĂ̏�Ԉꗗ���t�@�C������ǂݍ��ށB
		/// </summary>
		void readAllState(std::istream &is);

		/// <summary>
		/// ���ׂĂ̏�Ԉꗗ���t�@�C���ɏ������ށB
		/// </summary>
		void writeAllState(const std::string &path);

		/// <summary>
		/// ���ׂĂ̏�Ԉꗗ���t�@�C���ɏ������ށB
		/// </summary>
		void writeAllState(std::ostream &os);

		/// <summary>
		/// ��ԑJ�ڃO���t���t�@�C������ǂݍ��ށB
		/// </summary>
		void readGraph(const std::string &path);

		/// <summary>
		/// ��ԑJ�ڃO���t���t�@�C������ǂݍ��ށB
		/// </summary>
		void readGraph(std::istream &is);

		/// <summary>
		/// ��ԑJ�ڃO���t���t�@�C���ɏ������ށB
		/// </summary>
		void writeGraph(const std::string &path) const;

		/// <summary>
		/// ��ԑJ�ڃO���t���t�@�C���ɏ������ށB
		/// </summary>
		void writeGraph(std::ostream &os) const;

		#pragma endregion

	private:
		Graph(const Graph &obj);
		const Graph &operator =(const Graph &rhs);

		std::vector<State> all;
		std::map<State, int> allr;
		std::vector<std::vector<int>> es, esr;
	};

}
