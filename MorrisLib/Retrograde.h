#pragma once

#include "Graph.h"

namespace Morris {

	class Retrograde {
	public:
		Retrograde(const Graph &graph);

		/// <summary>
		/// 勝者
		/// </summary>
		enum Winner {

			/// <summary>
			/// 引き分け(千日手)
			/// </summary>
			draw = 0,

			/// <summary>
			/// 先手必勝
			/// </summary>
			red = 1,

			/// <summary>
			/// 後手必勝
			/// </summary>
			blue = 2,

		};

		/// <summary>
		/// 後退解析で必勝手を求める。
		/// </summary>
		void analyze();

		/// <summary>
		/// 解析結果
		/// </summary>
		const Winner &at(int index) const;

		/// <summary>
		/// 解析結果
		/// </summary>
		const Winner &operator [](int index) const;

		/// <summary>
		/// 解析対象のグラフ
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
