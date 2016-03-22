#pragma once

#include "Pad.hpp"

#define MAX_PAD_NUMBER 6

namespace SoLoud
{
	class Soloud;
}

class Board
{
public:
	Board(SoLoud::Soloud *soloud);
	void updatePadsVoltage(const std::string &buffer);
	void displayPadsVoltage() const;
private:
	Pad m_pads[MAX_PAD_NUMBER];
	SoLoud::Soloud *m_soloud;
};