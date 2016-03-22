#include "DouceurPCH.h"
#include "Pad.hpp"

Pad::Pad()
	: m_type(PadType::EMITTER)
	, m_port(0)
	, m_voltageArrayIndex(0)
{
	memset(m_voltageArray, 0, sizeof(m_voltageArray));
}

void Pad::updateVoltage(float voltage)
{
	m_voltage = voltage;
	m_voltageArray[m_voltageArrayIndex] = voltage;
	m_voltageArrayIndex = (m_voltageArrayIndex + 1) % VoltageArraySize;
}

void Pad::displayVoltage() const
{
	ImGui::PlotLines("Voltage", m_voltageArray, VoltageArraySize, m_voltageArrayIndex, nullptr, 0.f, 5.f);
}

