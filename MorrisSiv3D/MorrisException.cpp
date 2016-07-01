#include "MorrisException.h"

MorrisException::MorrisException(const MorrisState & state)
	: std::exception("三目並べのプレー処理中に例外が発生しました。"),
	s(state)
{
}

MorrisException::MorrisException(const MorrisState & state, const std::string & mes)
	: std::exception(mes.c_str()),
	s(state)
{
}

const MorrisState & MorrisException::state() const
{
	return s;
}
