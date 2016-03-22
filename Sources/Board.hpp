#pragma once

#include "Pad.hpp"

#define MAX_PAD_NUMBER 6

class Board
{
public:
	Board();
	void updatePadsVoltage(const std::string &buffer);
	void displayPadsVoltage() const;
private:
	Pad m_pads[MAX_PAD_NUMBER];
};