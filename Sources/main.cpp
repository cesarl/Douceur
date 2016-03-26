#include "DouceurPCH.h"

#include <stdlib.h>
#include <tchar.h>
#include <d3d11.h>

#include "DX11.hpp"
#include "ImGuiDX11.hpp"

#include "Serial.hpp"
#include "Board.hpp"

#include "fmod.hpp"

//#include "soloud.h"
//#include "soloud_wav.h"

int main(int ac, const char **av)
{
	// Create application window
	WNDCLASSEX wc = { sizeof(WNDCLASSEX), CS_CLASSDC, WndProc, 0L, 0L, GetModuleHandle(NULL), NULL, LoadCursor(NULL, IDC_ARROW), NULL, NULL, _T("Douceur"), NULL };
	RegisterClassEx(&wc);
	HWND hwnd = CreateWindow(_T("Douceur"), _T("Douceur"), WS_OVERLAPPEDWINDOW, 100, 100, 1280, 800, NULL, NULL, wc.hInstance, NULL);

	// Initialize Direct3D
	if (CreateDeviceD3D(hwnd) < 0)
	{
		CleanupDeviceD3D();
		UnregisterClass(_T("Douceur"), wc.hInstance);
		return 1;
	}

	// Show the window
	ShowWindow(hwnd, SW_SHOWDEFAULT);
	UpdateWindow(hwnd);

	// Setup ImGui binding
	ImGui_ImplDX11_Init(hwnd, g_pd3dDevice, g_pd3dDeviceContext);

	// Load Fonts
	// (there is a default font, this is only if you want to change it. see extra_fonts/README.txt for more details)
	//ImGuiIO& io = ImGui::GetIO();
	//io.Fonts->AddFontDefault();
	//io.Fonts->AddFontFromFileTTF("../../extra_fonts/Cousine-Regular.ttf", 15.0f);
	//io.Fonts->AddFontFromFileTTF("../../extra_fonts/DroidSans.ttf", 16.0f);
	//io.Fonts->AddFontFromFileTTF("../../extra_fonts/ProggyClean.ttf", 13.0f);
	//io.Fonts->AddFontFromFileTTF("../../extra_fonts/ProggyTiny.ttf", 10.0f);
	//io.Fonts->AddFontFromFileTTF("c:\\Windows\\Fonts\\ArialUni.ttf", 18.0f, NULL, io.Fonts->GetGlyphRangesJapanese());

	bool show_test_window = true;
	ImVec4 clear_col = ImColor(114, 144, 154);
	// Main loop
	MSG msg;
	ZeroMemory(&msg, sizeof(msg));

	std::string arduinoMsg;
	std::string arduinoDisplayMsg;
	bool messageEnded = true;

	FMOD::System *fmodSystem;
	FMOD::System_Create(&fmodSystem);

	FMOD::Sound        *sound = 0;
	FMOD::Channel      *channel = 0;
	FMOD::ChannelGroup *mastergroup = 0;

	fmodSystem->init(128, FMOD_INIT_NORMAL, 0);

	fmodSystem->getMasterChannelGroup(&mastergroup);

	fmodSystem->createSound("../DouceurExternal/audio-samples/Yamaha-1.wav", FMOD_LOOP_NORMAL, 0, &sound);

	unsigned int soundLength = 0;
	sound->getLength(&soundLength, FMOD_TIMEUNIT_MS);

	Serial arduino("COM6");
	Board  board(fmodSystem);

	while (msg.message != WM_QUIT)
	{
		fmodSystem->update();

		if (PeekMessage(&msg, NULL, 0U, 0U, PM_REMOVE))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
			continue;
		}
		ImGui_ImplDX11_NewFrame();

		static bool first = true;
		static int from = 0;
		static int to = 400;
		static float freq = 1.f;
		static float pan = 0.f;
		static float pitch = 0.f;
		if (!first)
		{
			fmodSystem->playSound(sound, mastergroup, false, &channel);
			first = true;
			to = soundLength;
			channel->getFrequency(&freq);
			channel->getPitch(&pitch);
		}

		{
			ImGui::SetNextWindowPos(ImVec2(10, 10));
			ImGui::Begin("FPS", nullptr, ImVec2(0, 0), 0.3f, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoSavedSettings);
			ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
			ImGui::End();
		}

		if (false)
		{
			ImGui::Begin("SoloudTest");

			bool changed = ImGui::SliderInt("from", (int*)&from, 0, soundLength - 1);
			changed |= ImGui::SliderInt("to", (int*)&to, 1, soundLength);
			changed |= ImGui::SliderFloat("freq", &freq, 0, 500000.f);
			changed |= ImGui::SliderFloat("pan", &pan, -1.f, 1.f);
			changed |= ImGui::SliderFloat("pitch", &pitch, 0.f, 10.f);
			if (changed)
			{
				if (from >= to)
				{
					from = to - 1;
				}
				channel->setLoopPoints(from, FMOD_TIMEUNIT_MS, to, FMOD_TIMEUNIT_MS);
				channel->setFrequency(freq);
				channel->setPan(pan);
			}
			ImGui::End();
		}

		if (false)
		{
			ImGui::SetNextWindowPos(ImVec2(650, 20), ImGuiSetCond_FirstUseEver);     // Normally user code doesn't need/want to call it because positions are saved in .ini file anyway. Here we just want to make the demo initial state a bit more friendly!
			ImGui::ShowTestWindow(nullptr);
		}

		board.updateGui();

		//Arduino test
		{
			if (arduino.IsConnected())
			{
				if (ImGui::Begin("Arduino test"))
				{
					char buffer[2048];
					if (messageEnded)
					{
						arduino.WriteData("A", 1);
						messageEnded = false;
					}
					while (!messageEnded)
					{
						int count = arduino.ReadData(buffer, 2047);
						if (count)
						{
							buffer[count] = '\0';
							if (buffer[count - 1] == '@')
							{
								buffer[count - 1] = '\0';
								messageEnded = true;
							}
							arduinoMsg += buffer;
						}
						else
						{
							break;
						}
					}
					if (messageEnded)
					{
						board.updatePadsVoltage(arduinoMsg);

						arduinoDisplayMsg = arduinoMsg;
						arduinoMsg.clear();
					}
					ImGui::TextWrapped(arduinoDisplayMsg.c_str());
					ImGui::Text("Message size : %i", arduinoDisplayMsg.size());
				}
				ImGui::End();
				board.displayPadsVoltage();
			}
		}

		// Rendering
		g_pd3dDeviceContext->ClearRenderTargetView(g_mainRenderTargetView, (float*)&clear_col);
		ImGui::Render();
		g_pSwapChain->Present(0, 0);
	}

	ImGui_ImplDX11_Shutdown();
	CleanupDeviceD3D();
	UnregisterClass(_T("Douceur"), wc.hInstance);

	return EXIT_SUCCESS;
}