# GPU Stress Test

This test evaluates GPU performance and stability under stress. It collects data on power consumption, temperature, frequency, and memory usage to identify potential issues or limitations.

## Table of Contents

*   [Functionality](#functionality)
*   [Build Steps](#build-steps)
*   [Package Creation Steps](#package-creation-steps)
*   [Test Execution Prerequisites](#test-execution-prerequisites)
*   [Test Execution Steps](#test-execution-steps)
*   [Test Usage](#test-usage)

## Functionality <a name="functionality"></a>

This test stresses the GPU using the "stress-ng" tool. It achieves stress by:

*   Applying stress to GPU cores/clusters using stress-ng with various load profiles and durations.
*   Monitoring and logging critical parameters simultaneously: power, voltage, temperature, and frequency.
*   Recording GPU utilization using the mpstat utility.
*   Recording memory usage and capturing any errors or exceptions.
*   Generating comprehensive log files for analysis and reporting.

## Build Steps <a name="build-steps"></a>

### Prerequisites

*   Install Android NDK
*   Set the PATH environment variable for NDK

### Test Case Build Steps

1.  Clone the test case:

    ```bash
    git clone [https://gchips-internal.googlesource.com/pdte-slt/test_content](https://gchips-internal.googlesource.com/pdte-slt/test_content)
    ```

2.  Go to the test case source directory and build:

    ```bash
    cd test_content/subsystem/gpu && make
    ```

### Dependent Package Build Steps (stress-ng and mpstat)

1.  cd deps
2.  make

## Package Creation Steps <a name="package-creation-steps"></a>

Follow CPU process to generate msptats and stressng binaries

## Test Execution Prerequisites <a name="test-execution-prerequisites"></a>

1.  Connect the test device to your PC via USB cable.

2.  Ensure the device ID using the following command:

    ```bash
    adb devices
    ```

3.  Ensure ADB root permissions are granted:

    ```bash
    adb -s <devices_id> root
    ```

4.  Push the `gpu001` folder to `/data/local/tmp/`:

    ```bash
    adb -s <devices_id> push gpu001 /data/local/tmp/
    ```

5.  Change the mode of the `gpu001` directory binary to executable:

    ```bash
    adb -s <devices_id> shell "chmod +x /data/local/tmp/gpu001/*"
    ```

6.  Ensure the Device Under Test (DUT) is in an idle state before starting the test. This may involve rebooting the DUT.

## Test Execution Steps using adb shell <a name="test-execution-steps"></a>

Execute the test case via ADB:

```bash
adb -s <devices_id> shell /data/local/tmp/gpu001/gpu_001
```

## Usages <a name="test-usage"></a>
*./gpu_001 -h [-d log_level] [-c cores] [-m stressor_mode] [-t duration] [config_file]*

**Options:**

*   `-h`: Print usage function
*   `-d level<0 ~ 3>`: Used to set the log level, e.g.:
    *   `0`: Set the log level ERROR
    *   `1`: Set the log level WARN
    *   `2`: Set the log level INFO
    *   `3`: Set the log level DEBUG
*   `-l load`: Enter the load limit, e.g.: `70`
*   `-t duration`: Enter the duration in seconds, e.g.: `30`
*   `-m stressor_mode`: Enter the load mode to stress GPU, e.g.:
    *   `blank`: Capture the parameter without load
    *   `CPU`: Generate maximum CPU load
    *   `shader`: Generate shader load
    *   `tiles`: Generate load tiles
    *   `stress`: Generate load stress
    *   `throttle`: Generate varying cyclic load
*   `[config_file]`: Provide configuration file. If not provided, the default configuration file will be used.

**Examples:**


Testing: 

adb shell settings put global verifier_verify_adb_installs 0
adb install android-arm64-v8a-debug.apk

#### To reinstall and replace the older one 

root@slt-pdte-lab01-41:# adb install -t android-arm64-v8a-debug.apk 
if some problem in install,  do adb uninstall org.gpu.glload.debug



adb root
adb shell
setenforce 0

if screen is locked run script screenUnLock.py from host machine as mentioned at screenUnLock.md


rm -rf /storage/emulated/0/Android/data/org.gpu.glload.debug/files/

mkdir /data/local/tmp/gpu001
cd /data/local/tmp/gpu001

cp gpu_001.conf gpu_001 mpstats and stress-ng android-arm64-v8a-debug.apk from  https://drive.google.com/corp/drive/folders/157-KFbyKxe_FISI3pRg_wfckl111uhDQ?resourcekey=0-VShyb_IoLH86lxC4ngh46w to /data/local/tmp/gpu001  with adb push

download rename  gpu_001.conf as per your board 

chmod +x ./gpu_001 
chmod +x ./mpstats
chmod +x ./stress-ng 
chmod +x ./android-arm64-v8a-debug.apk

test  ./mpstat -P ALL 1 -intv 200 

To install/reinstall APK do ./gpu_001 -m install 


Example: ./gpu_001
Example: ./gpu_001 -t 20 -m tiles
Example: ./gpu_001 -m stress
Example: ./gpu_001 -m shader -t 120
Example: ./gpu_001 -t 180 -m throttle
Example: ./gpu_001 -m cpu
Example: ./gpu_001 -m blank
Example: ./gpu_001 -m pow
Example: ./gpu_001 -m install  
Example: ./gpu_001 -m usage
Example: ./gpu_001 -m idle


Example: ./gpu_001 -d 3
