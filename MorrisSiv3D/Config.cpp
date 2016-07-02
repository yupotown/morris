#include "Config.h"

void Config::write(String file) const
{
	INIWriter ini(file);
	ini.write(L"Window", L"fullscreen", fullscreen);
	ini.write(L"Window", L"resolution", resolution);
	ini.write(L"Grid", L"center", gridCenter);
	ini.write(L"Grid", L"size", gridSize);
	ini.write(L"Grid", L"color", gridCol);
	for (int i = 0; i < 2; ++i) for (int j = 0; j < 3; ++j) {
		ini.write(L"Piece", Format(L"point", i * 3 + j + 1), piecePos[i][j]);
	}
	ini.write(L"Piece", L"radius", pieceRad);
	ini.write(L"Piece", L"color1", pieceCol[0]);
	ini.write(L"Piece", L"color2", pieceCol[1]);
	ini.write(L"Turn", L"font", turnFont.name());
	ini.write(L"Turn", L"fontsize", turnFont.size());
	ini.write(L"Turn", L"point", turnPos);
	ini.write(L"Turn", L"color", turnCol);
	ini.write(L"Reset", L"point", resetPos);
	ini.write(L"Reset", L"font", resetFont.name());
	ini.write(L"Reset", L"fontsize", resetFont.size());
}

void Config::read(String file)
{
	INIReader ini(file);
	fullscreen = ini.getOr<bool>(L"Window.fullscreen", false);
	resolution = ini.getOr<Size>(L"Window.resolution", Size(640, 480));
	gridCenter = ini.getOr<Point>(L"Grid.center", resolution / 2);
	gridSize = ini.getOr<Size>(L"Grid.size", Size(120, 120));
	gridCol = ini.getOr<Color>(L"Grid.color", Color(255, 255, 255, 255));
	for (int i = 0; i < 2; ++i) for (int j = 0; j < 3; ++j) {
		piecePos[i][j] = ini.getOr<Point>(Format(L"Piece.point", i * 3 + j + 1),
			Point(gridCenter.x + (i * 4 - 2) * gridSize.x, gridCenter.y + (j - 1) * gridSize.y));
	}
	pieceRad = ini.getOr<double>(L"Piece.radius", 24);
	pieceCol[0] = ini.getOr<Color>(L"Piece.color1", Color(255, 0, 0, 255));
	pieceCol[1] = ini.getOr<Color>(L"Piece.color2", Color(0, 0, 255, 255));
	pieceColHold[0] = Color(pieceCol[0].r, pieceCol[0].g, pieceCol[0].b, 128);
	pieceColHold[1] = Color(pieceCol[1].r, pieceCol[1].g, pieceCol[1].b, 128);
	turnFont = Font(ini.getOr<int>(L"Turn.fontsize", 20), ini.getOr<String>(L"Turn.font", Typeface::Default.str()));
	turnPos = ini.getOr<Point>(L"Turn.point", Point(10, 10));
	turnCol = ini.getOr<Color>(L"Turn.color", Color(255, 255, 255, 255));
	resetPos = ini.getOr<Point>(L"Reset.point", Point(resolution.x - 160, 10));
	resetFont = Font(ini.getOr<int>(L"Reset.fontsize", 14), ini.getOr<String>(L"Reset.font", Typeface::Default.str()));
}

Point Config::gridPos(int x, int y) const
{
	return{ gridCenter.x + (x - 1) * gridSize.x, gridCenter.y + (y - 1) * gridSize.y };
}
