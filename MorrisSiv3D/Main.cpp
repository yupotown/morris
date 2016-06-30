
# include <Siv3D.hpp>

#include "MorrisState.h"

void Main()
{
	MorrisState s;

	// 描画位置
	Point center = Window::Size() / 2;
	Size grid = { 100, 100 };
	Point posF[3], posS[3];
	for (int i = 0; i < 3; ++i) {
		posF[i] = { center.x - grid.x * 2, center.y + (i - 1) * grid.y };
		posS[i] = { center.x + grid.x * 2, center.y + (i - 1) * grid.y };
	}
	double rf = 20, rs = 20;
	Color
		colF = { 255, 0, 0, 255 },
		colS = { 0, 0, 255, 255 },
		colLine = { 255, 255, 255, 255 };

	while (System::Update())
	{
		// 描画
		const Point lt = center - grid, rb = center + grid;
		Line(lt.x, lt.y, rb.x, lt.y).draw(colLine);
		Line(lt.x, lt.y, lt.x, rb.y).draw(colLine);
		Line(rb.x, lt.y, rb.x, rb.y).draw(colLine);
		Line(lt.x, rb.y, rb.x, rb.y).draw(colLine);
		Line(lt.x, lt.y, rb.x, rb.y).draw(colLine);
		Line(rb.x, lt.y, lt.x, rb.y).draw(colLine);
		for (int i = 0; i < 3; ++i) {
			if (s.fp[i].x < 0 || s.fp[i].x >= 3 || s.fp[i].y < 0 || s.fp[i].y >= 3) {
				Circle(posF[i], rf).draw(colF);
			}
			else {
				Circle({ lt.x + s.fp[i].x * grid.x, lt.y + s.fp[i].y * grid.y }, rf).draw(colF);
			}
			if (s.sp[i].x < 0 || s.sp[i].x >= 3 || s.sp[i].y < 0 || s.sp[i].y >= 3) {
				Circle(posS[i], rs).draw(colS);
			}
			else {
				Circle({ lt.x + s.sp[i].x * grid.x, lt.y + s.sp[i].y * grid.y }, rs).draw(colS);
			}
		}
	}
}

