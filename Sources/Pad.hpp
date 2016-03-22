#pragma once

enum PadType
{
	EMITTER = 0
	, RECEIVER
	, END
};

class Pad
{
public:
	Pad();
	void updateVoltage(float voltage);
	void displayVoltage() const;

	inline void setPort(uint8_t port) { m_port = port; }
	inline uint8_t getPort() const { return m_port; }
	inline void setType(PadType type) { m_type = type; }
	inline PadType getType() const { return m_type; }
private:
	static const size_t VoltageArraySize = 1024;
	PadType m_type;
	uint8_t m_port;
	float   m_voltage;
	
	float m_voltageArray[VoltageArraySize];
	size_t m_voltageArrayIndex;
};