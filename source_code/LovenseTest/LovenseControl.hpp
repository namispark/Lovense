#pragma once

#include <chrono>
#include <thread>
#include "Lovense.h"

class LovenseControl : public ILovenseSDKNotify
{
public:

	// --- Variables ---
	CLovenseToyManager* manager = NULL; // Stores the pointer to the toy manager
	bool toy_found = false; // Binary flag if the toy is found
	bool toy_connected = false; // Binary flag if the toy is connected
	std::string toy_name; // Toy name
	std::string toy_id; // Toy ID

	// --- Unused Callbacks ---
	virtual	void LovenseDidSearchStart() {}
	virtual void LovenseErrorOutPut(int errorCode, const char* errorMsg) {}
	virtual void LovenseDidSearchEnd() {}
	virtual	void LovenseDidSendCmdStart() {}
	virtual void LovenseSendCmdResult(const char* szToyID, CLovenseToy::CmdType cmd, const char* result, CLovenseToy::Error errorCode) {}
	virtual	void LovenseDidSendCmdEnd() {}

	// --- Search Callback ---
	virtual  void LovenseSearchingToys(lovense_toy_info_t* info) {
		if (info) {
			// Set the toy_found flag
			toy_found = true;
			// Save the toy name
			toy_name = std::string(info->toy_name);
			// Save the toy ID
			toy_id = std::string(info->toy_id);
		}
	}

	// --- Connect Callback ---
	virtual void LovenseToyConnectedStatus(const char* szToyID, bool isConnected) {
		// Set the toy_connected flag
		toy_connected = isConnected;
	}

	// --- Setup System ---
	bool setupSystem() {
		// Get the toy manager
		manager = GetLovenseToyManager();
		// Set the developer token
		if (manager->SetDeveloperToken(developer_token.c_str())) {
			// Register callback
			manager->RegisterEventCallBack(this);
		}
		// Start the toy search
		manager->StartSearchToy();
		// Wait until the toy is found (Maximum: 10 seconds) 
		for (int i = 0; i < 10; i++) {
			// Wait for 1 second
			std::this_thread::sleep_for(std::chrono::milliseconds(1000));
			if (toy_found) {
				// If the toy is found, then stop the toy search
				manager->StopSearchToy();
				break;
			}
		}
		// If the toy is found, then connect with the toy
		if (toy_found) {
			// Wait until the toy is connected (Maximum: 10 seconds) 
			for (int i = 0; i < 10; i++) {
				// Wait for 1 second
				std::this_thread::sleep_for(std::chrono::milliseconds(1000));
				if (toy_connected) {
					// If the toy is connected, then break the for-loop
					break;
				}
				else {
					// Attempt to connect with the toy
					manager->ConnectToToy(toy_id.c_str());
				}
			}
		}
		// If the toy is found and connected, then the system is ready 
		return toy_found && toy_connected;
	}

	// --- Close System ---
	void closeSystem() {
		// Disconnect the toy
		manager->DisConnectedToy(toy_id.c_str());
		// Release the toy manager
		ReleaseLovenseToyManager();
		manager = NULL;
	}

	// --- Vibrate Toy ---
	void vibrateToy(int level) {
		// Ensure that the level is in the interval [0, 20]
		level = std::min(20, std::max(0, level));
		// Send the vibrate command
		manager->SendCommand(toy_id.c_str(), CLovenseToy::CmdType::COMMAND_VIBRATE, level);
	}
};
