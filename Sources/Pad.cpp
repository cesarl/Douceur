#include "DouceurPCH.h"
#include "Pad.hpp"
#include "fmod.hpp"

DcrSound::DcrSound(FMOD::System *system)
	: DcrInputInterface(system)
{}

void DcrSound::updateGui()
{}

void DcrSound::play(float voltage)
{}

//////////////////////////////////////////////////////////////////////////

DcrTone::DcrTone(FMOD::System *system)
	: DcrInputInterface(system)
	, dsp(nullptr)
	, note(440.f)
	, oscillatorType(0)
	, channel(nullptr)
	, editorPlay(false)
	, voltageMin(0.5f)
	, voltageModifier(true)
	, voltageModifierFrom(30000.f)
	, voltageModifierTo(100000.f)
{
	strcpy(name, "no_name_tone");
}

DcrTone::~DcrTone()
{
	play(0.f);
}

void DcrTone::updateGui()
{
	ImGui::InputText("name", name, 1024);
	if (ImGui::SliderFloat("note", &note, 0.f, 11000.f))
	{
		if (dsp)
		{
			dsp->setParameterFloat(FMOD_DSP_OSCILLATOR_RATE, note);
		}
	}
	if (ImGui::Combo("Oscillator type", &oscillatorType, "Sine\0Square\0Saw\0Triangle\0Noise\0\0"))
	{
		if (dsp)
		{
			dsp->setParameterInt(FMOD_DSP_OSCILLATOR_TYPE, oscillatorType);
		}
	}
	ImGui::Checkbox("Voltage modifier", &voltageModifier);
	if (voltageModifier)
	{
		bool modified = false;
		modified |= ImGui::SliderFloat("from", &voltageModifierFrom, 10000.f, 100000.f);
		modified |= ImGui::SliderFloat("to", &voltageModifierTo, 10000.f, 100000.f);
		if (modified && voltageModifierTo <= voltageModifierFrom)
		{
			voltageModifierTo = voltageModifierFrom + 1;
		}
	}

	static bool hack = true;
	ImGui::Checkbox("HACK", &hack);
	if (hack)
	{
		ImGui::Checkbox("play test", &editorPlay);
		if (editorPlay)
		{
			play(5.f);
		}
		else
		{
			play(0.f);
		}
	}
}

void DcrTone::play(float voltage)
{
	if (voltage < voltageMin)
	{
		if (channel)
		{
			channel->stop();
		}
		channel = nullptr;
		dsp = nullptr;
		return;
	}

	if (!dsp)
	{
		system->createDSPByType(FMOD_DSP_TYPE_OSCILLATOR, &dsp);
		dsp->setParameterInt(FMOD_DSP_OSCILLATOR_TYPE, oscillatorType);
		dsp->setParameterFloat(FMOD_DSP_OSCILLATOR_RATE, note);
		system->playDSP(dsp, 0, false, &channel);
	}
	if (voltageModifier)
	{
		float freq = ((voltage / 2.5f) * (voltageModifierTo - voltageModifierFrom)) + voltageModifierFrom;
		channel->setFrequency(freq);
		std::cout << voltage << " " << freq << std::endl;
	}
}

//////////////////////////////////////////////////////////////////////////

DcrModifier::DcrModifier(FMOD::System *system)
	: DcrInputInterface(system)
	, name("no_named_modifier")
	, channel(nullptr)
{
	for (auto &e : dsp)
	{
		e = nullptr;
	}
	system->getMasterChannelGroup(&channel);
}

void DcrModifier::updateGui()
{}

void DcrModifier::play(float voltage)
{}

//////////////////////////////////////////////////////////////////////////
Pad::Pad()
	: m_type(PadType::EMITTER)
	, m_port(0)
	, m_voltageArrayIndex(0)
	, m_fmodSystem(nullptr)
	, m_input(nullptr)
{
	memset(m_voltageArray, 0, sizeof(m_voltageArray));
}

void Pad::updateGui()
{
	ImGui::PushID(this);
	if (m_input)
	{
		m_input->updateGui();
		if (ImGui::Button("Destroy"))
		{
			delete m_input;
			m_input = nullptr;
		}
	}
	else
	{
		if (ImGui::Button("Create"))
		{
			m_input = new DcrTone(m_fmodSystem);
		}
	}
	ImGui::PopID();
}

void Pad::updateVoltage(float voltage)
{
	voltage = m_type == EMITTER ? 5.0f - voltage : voltage;
	m_voltage = voltage;
	m_voltageArray[m_voltageArrayIndex] = voltage;
	m_voltageArrayIndex = (m_voltageArrayIndex + 1) % VoltageArraySize;
	if (m_input)
	{
		m_input->play(voltage);
	}
}

void Pad::displayVoltage() const
{
	ImGui::PlotLines("Voltage", m_voltageArray, VoltageArraySize, m_voltageArrayIndex, nullptr, 0.f, 5.f);
}

