# Python script to check the lock state of an Android device and unlock it if necessary.

## Notes

* This script uses basic key events to unlock the device. More advanced unlock methods (e.g., swipe, PIN input) may be required for some devices.
* The script assumes the device has a standard unlock method that can be triggered by the menu key.
* Test this script on test devices first.
* If you have issues with the device state not being retrieved, double check that adb is working correctly, and that the device is authorized.
* The script now parses the dumpsys output directly in python, which increases reliability.

## Run the Script:** Execute the script with the device serial number as a command-line argument:

    ```bash
    python3 screenUnLock.py <device_serial>
    ```

    Replace `<device_serial>` with the actual serial number of your device.

    **Example:**

    ```bash
    python3 screenUnLock.py ABC123XYZ
    ```



## Script Description

The script performs the following actions:

1.  **Checks Lock State:** Retrieves the screen state of the specified Android device using ADB.
2.  **Unlocks Device (If Locked):** If the device is locked, it attempts to unlock it by sending key events (wake and menu).
3.  **Prints Results:** Displays the current and new screen states, and indicates whether the unlock attempt was successful.

## Code Structure

* `get_screen_state(serial: str) -> Optional[str]`: Retrieves the screen state of the specified device.
* `unlock_device(serial: str) -> None`: Unlocks the specified device.
* `get_screen_state_code(state: Optional[str]) -> int`: Converts the screen state string to an integer code.
* `main() -> None`: Main function that orchestrates the script's execution.

## Error Handling

The script includes error handling to gracefully handle potential issues such as:

* ADB not found.
* Device connection issues.
* Timeout errors.
* General exceptions.




## Prerequisites for development

* **ADB (Android Debug Bridge):** Ensure that ADB is installed and configured on your system. You can download it as part of the Android SDK Platform-Tools.
* **Python 3:** This script is written in Python 3.
* **Android Device:** An Android device with ADB debugging enabled.
* **USB Connection:** A USB cable to connect your Android device to your computer.
* **Authorize ADB:** Authorize your computer to debug your Android device when prompted.

## Installation

1.  **Clone the Repository (Optional):** If you have this code in a repository, clone it:

    ```bash
    git clone <repository_url>
    cd <repository_directory>
    ```

2.  **Save the Script:** Save the Python code as a file named `screenUnLock.py`.

## Usage

1.  **Connect Device:** Connect your Android device to your computer via USB.
2.  **Enable ADB Debugging:** Ensure ADB debugging is enabled on your device.
3.  **Get Device Serial:** Use the following command to get the serial number of your device:

    ```bash
    adb devices
    ```

