#include "MorrisException.h"

MorrisException::MorrisException(const MorrisState & state)
	: std::exception("�O�ڕ��ׂ̃v���[�������ɗ�O���������܂����B"),
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
