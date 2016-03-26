#pragma once

#include "Pad.hpp"

#define MAX_PAD_NUMBER 6

namespace FMOD
{
	class System;
}

class Board
{
public:
	Board(FMOD::System *fmodSystem);
	void updatePadsVoltage(const std::string &buffer);
	void displayPadsVoltage() const;
	void updateGui();
private:
	Pad m_pads[MAX_PAD_NUMBER];
	FMOD::System *m_fmodSystem;
};