"""
Author: Chaithanya M  
LDAP: mchaithanyaaa@google.com
Reviewed 1: Arvind Umrao aumrao@google.com

Requirements:
1.  Check the lock state of a specified Android device using ADB.
2.  If the device is locked (ON_LOCKED or OFF_LOCKED), attempt to unlock it by sending key events (wake and menu).
3.  Report the current and resulting screen states, indicating success or failure of the unlock attempt.

Input: Device serial number (as a command-line argument).
Output: Console output detailing the device's lock state and unlock results.


python3 screenunLock.py 49041FDLK00859


"""

import subprocess
import sys
import time
from typing import Optional


def get_screen_state(serial: str) -> Optional[str]:
    """
    Retrieves the screen state of a specific Android device using ADB.

    Args:
        serial: The serial number of the Android device.

    Returns:
        The screen state string (e.g., "ON_LOCKED", "ON_UNLOCKED"), or None on error.
    """
    try:
        process = subprocess.Popen(
            ["adb", "-s", serial, "shell", "dumpsys", "nfc"],
            stdout=subprocess.PIPE,
            stderr=subprocess.PIPE,
            text=True,
        )
        stdout, stderr = process.communicate(timeout=10)
        if process.returncode == 0:
            for line in stdout.splitlines():
                if "mScreenState=" in line:
                    return line.split("=")[1].strip()
        else:
            print(f"Error getting screen state for {serial}: {stderr}")
            return None
    except subprocess.TimeoutExpired:
        print(f"Timeout while getting screen state for {serial}.")
        return None
    except FileNotFoundError:
        print("ADB not found. Ensure ADB is in your PATH.")
        return None
    except Exception as exc:
        print(f"An unexpected error occurred for {serial}: {exc}")
        return None


def unlock_device(serial: str) -> None:
    """
    Unlocks a specific Android device using ADB.

    Args:
        serial: The serial number of the Android device.
    """
    try:
        subprocess.run(["adb", "-s", serial, "shell", "input", "keyevent", "26"], check=True)
        time.sleep(0.1)
        subprocess.run(["adb", "-s", serial, "shell", "input", "keyevent", "82"], check=True)
        time.sleep(0.1)
        print(f"Device {serial} unlock attempt initiated.")
    except subprocess.CalledProcessError as exc:
        print(f"Error unlocking device {serial}: {exc}")
    except FileNotFoundError:
        print("ADB not found. Ensure ADB is in your PATH.")
    except Exception as exc:
        print(f"An unexpected error occurred for {serial}: {exc}")


def get_screen_state_code(state: Optional[str]) -> int:
    """
    Converts screen state string to integer code.

    Args:
        state: The screen state string.

    Returns:
        The integer code representing the screen state, or -1 for failure.
    """
    if state == "ON_UNLOCKED":
        return 2
    if state == "ON_LOCKED":
        return 3
    if state == "OFF_LOCKED":
        return 4
    return -1  # FAILURE


def main() -> None:
    """
    Main function to process the device and unlock it if needed.
    """
    if len(sys.argv) != 2:
        print("Usage: python script.py <device_serial>")
        sys.exit(1)

    serial = sys.argv[1]
    print(f"Processing device: {serial}")

    state = get_screen_state(serial)
    if state is None:
        return

    state_code = get_screen_state_code(state)
    print(f"  Current screen state: {state} (Code: {state_code})")

    if state_code in (3, 4):  # ON_LOCKED or OFF_LOCKED
        print(f"  Device {serial} is locked. Attempting to unlock...")
        unlock_device(serial)
        time.sleep(3)

        new_state = get_screen_state(serial)
        if new_state is not None:
            new_state_code = get_screen_state_code(new_state)
            print(f"  New screen state: {new_state} (Code: {new_state_code})")
            if new_state_code == 2:
                print(f"  Device {serial} unlocked successfully.")
            else:
                print(f"  Device {serial} unlock attempt failed or device is still locked.")
        else:
            print(f"  Failed to get updated device state for {serial} after unlock attempt.")
    else:
        print(f"  Device {serial} is already unlocked or in an unknown state.")


if __name__ == "__main__":
    main()
