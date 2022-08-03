#include <cstdlib>
#include <iostream>
#include "Constants.hpp"
#include "LovenseControl.hpp"

using namespace std;

/* --- Wait function ---
The program needs to wait at least one second between each sent command.

Documentation:
https://developer.lovense.com/#step-4-connect-lovense-toys-and-send-commands-2
Quote: "For best results, don't sent toy commands more than once per second."
*/

void waitForSeconds(int seconds) {
	for (int i = 1; i <= seconds; i++) {
		std::this_thread::sleep_for(std::chrono::milliseconds(1000));
		std::cout << "Wait: " << i << " seconds" << std::endl;
	}
}

// --- Main function ---
int main(void) {
	
	// --- Setup System ---
	LovenseControl control;
	bool systemReady = control.setupSystem();
	if (systemReady) { cout << "SYSTEM READY" << endl; }
	else { cout << "SYSTEM NOT INITIALIZED" << endl; }

	// --- Run Test ---
	if (systemReady) {
		cout << "--- RUN TEST ---" << endl;
		waitForSeconds(2);
		cout << "--- VIBRATE ON LEVEL 1 ---" << endl;
		control.vibrateToy(1);
		waitForSeconds(2);
		cout << "--- VIBRATE ON LEVEL 2 ---" << endl;
		control.vibrateToy(2);
		waitForSeconds(2);
		cout << "--- VIBRATE ON LEVEL 3 ---" << endl;
		control.vibrateToy(3);
		waitForSeconds(2);
		cout << "--- VIBRATE ON LEVEL 0 ---" << endl;
		control.vibrateToy(0);
		waitForSeconds(2);
	}

	// --- Close System ---
	control.closeSystem();
	cout << "SYSTEM CLOSED" << endl;

	return EXIT_SUCCESS;
}
