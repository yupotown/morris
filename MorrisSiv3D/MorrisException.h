#pragma once

#include <exception>
#include <string>
#include "MorrisState.h"

class MorrisException : public std::exception {
public:
	MorrisException(const MorrisState &state);
	MorrisException(const MorrisState &state, const std::string &mes);

	const MorrisState &state() const;

private:
	MorrisState s;
};
