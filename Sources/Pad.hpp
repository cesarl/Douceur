#pragma once

enum PadType
{
	EMITTER = 0
	, RECEIVER
	, END
};

namespace FMOD
{
	class System;
	class Sound;
	class Channel;
	class ChannelGroup;
	class DSP;
}

struct DcrInputInterface
{
	FMOD::System *system;

	DcrInputInterface(FMOD::System *system) : system(system) {}
	virtual void updateGui() = 0;
	virtual void play(float voltage) = 0;
};

struct DcrSound : public DcrInputInterface
{
	FMOD::Sound *soundPtr;
	std::string  path;
	unsigned int from;
	unsigned int to;

	FMOD::Channel *channel;

	DcrSound(FMOD::System *system);
	virtual ~DcrSound(){}
	virtual void updateGui();
	virtual void play(float voltage);
};

struct DcrTone : public DcrInputInterface
{
	FMOD::DSP *dsp;
	char name[1024];
	float note;
	int oscillatorType;
	bool editorPlay;
	float voltageMin;
	bool voltageModifier;
	float voltageModifierFrom;
	float voltageModifierTo;

	FMOD::Channel *channel;

	DcrTone(FMOD::System *system);
	virtual ~DcrTone();
	virtual void updateGui();
	virtual void play(float voltage);
};

struct DcrModifier : public DcrInputInterface
{
	FMOD::DSP *dsp[6];
	std::string name;

	FMOD::ChannelGroup *channel;

	DcrModifier(FMOD::System *system);
	virtual ~DcrModifier(){}
	virtual void updateGui();
	virtual void play(float voltage);
};

class Pad
{
public:
	Pad();
	void updateGui();
	void updateVoltage(float voltage);
	void displayVoltage() const;

	inline void setPort(uint8_t port) { m_port = port; }
	inline uint8_t getPort() const { return m_port; }
	inline void setType(PadType type) { m_type = type; }
	inline PadType getType() const { return m_type; }
	inline void setFmodSystem(FMOD::System *fmodSystem) { m_fmodSystem = fmodSystem; }
private:
	static const size_t VoltageArraySize = 1024;
	PadType m_type;
	uint8_t m_port;
	float   m_voltage;
	FMOD::System *m_fmodSystem;

	DcrInputInterface *m_input;
	
	float m_voltageArray[VoltageArraySize];
	size_t m_voltageArrayIndex;
};