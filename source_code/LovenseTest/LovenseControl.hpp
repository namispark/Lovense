#pragma once

#include <chrono>
#include <thread>
#include "Lovense.h"

class LovenseControl : public ILovenseSDKNotify
{
public:

	// --- Variables ---
	CLovenseToyManager* manager = NULL; // Stores pointer to toy manager
	bool toy_found = false; // Binary flag if toy is found
	bool toy_connected = false; // Binary flag if toy is connected
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
			// Set toy_found flag
			toy_found = true;
			// save name
			toy_name = std::string(info->toy_name);
			// save ID
			toy_id = std::string(info->toy_id);
		}
	}

	// --- Connected Callback ---
	virtual void LovenseToyConnectedStatus(const char* szToyID, bool isConnected) {
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
				// If toy is found, then stop toy search
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
					// If the toy is connected, then break the for loop
					break;
				}
				else {
					// Attempt to connect with the toy
					manager->ConnectToToy(toy_id.c_str());
				}
			}
		}
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
		// Send vibrate command
		manager->SendCommand(toy_id.c_str(), CLovenseToy::CmdType::COMMAND_VIBRATE, level);
	}
};
