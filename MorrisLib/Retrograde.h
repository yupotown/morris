#pragma once

#include "Graph.h"

namespace Morris {

	class Retrograde {
	public:
		Retrograde(const Graph &graph);

		/// <summary>
		/// ����
		/// </summary>
		enum Winner {

			/// <summary>
			/// ��������(�����)
			/// </summary>
			draw = 0,

			/// <summary>
			/// ���K��
			/// </summary>
			red = 1,

			/// <summary>
			/// ���K��
			/// </summary>
			blue = 2,

		};

		/// <summary>
		/// ��މ�͂ŕK��������߂�B
		/// </summary>
		void analyze();

		/// <summary>
		/// ��͌���
		/// </summary>
		const Winner &at(int index) const;

		/// <summary>
		/// ��͌���
		/// </summary>
		const Winner &operator [](int index) const;

		/// <summary>
		/// ��͑Ώۂ̃O���t
		/// </summary>
		const Graph &graph() const;

	private:
		Retrograde(const Retrograde &obj);
		const Retrograde &operator =(const Retrograde &rhs);

		const Graph &g;

		std::vector<Winner> winners;
		void innerAnalyze(Winner w, const std::set<int> &already);
	};

}
