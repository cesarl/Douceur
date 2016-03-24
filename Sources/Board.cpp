#include "DouceurPCH.h"
#include "Board.hpp"
#include "fmod.hpp"

Board::Board(FMOD::System *fmodSystem)
{
	assert(fmodSystem != nullptr);
	m_fmodSystem = fmodSystem;
	for (uint8_t i = 0; i < MAX_PAD_NUMBER; ++i)
	{
		m_pads[i].setPort(i);
		m_pads[i].setFmodSystem(fmodSystem);
	}
}

void Board::updatePadsVoltage(const std::string &buffer)
{
	char numberStr[8];
	int  port = 0;
	int  cursor = 0;
	const char *str = buffer.c_str();

	for (int i = 0; i < buffer.size(); ++i)
	{
		if (str[i] == ' ')
		{
			// treat it
			int value = atoi(numberStr);
			m_pads[port].updateVoltage(value * (5.f / 1024.f));

			++port;
			cursor = 0;
			memset(numberStr, 0, sizeof(numberStr));
			if (port >= MAX_PAD_NUMBER)
				break;
			continue;
		}
		numberStr[cursor++] = str[i];
	}
}

void Board::displayPadsVoltage() const
{
	if (ImGui::Begin("Pads voltage"))
	{
		for (int i = 0; i < MAX_PAD_NUMBER; ++i)
		{
			m_pads[i].displayVoltage();
		}
	}
	ImGui::End();
}
