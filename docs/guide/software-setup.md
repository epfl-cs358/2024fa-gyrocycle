---
next:
    text: "You've done it!"
    link: '/guide/congrats'
---

# Software Setup

To balance itself, the bicycle needs software to be the orchestrator of the balancing and tell each component what it should do.

The NodeMCU ESP32 is the central component. It hosts and runs the software and is wired to other components to control them.

Following is a tutorial on how to setup the IDE, the software, along with other components to configure such as the ODrive and RemoteXY.

[[toc]]

## Setting up Arduino IDE

We used [Arduino IDE 2](https://docs.arduino.cc/software/ide/) for the software part of GyroCycle.

If you do not have it installed already, download and install the software on your machine from [this link](https://www.arduino.cc/en/software).

Because we used an ESP32 and not an Arduino, some additional steps are required, described below.

### Installing the ESP32 board

By default, Arduino IDE only supports Arduino devices. To compile and upload code for ESP32, you will have to add an `Additional boards manager`.

1. In Arduino IDE, go to `File` > `Preferences`

1. In the preferences window that opens up, at the bottom, there is an `Additional boards manager URL`. Paste the following URL into it:

    ```
    https://raw.githubusercontent.com/espressif/arduino-esp32/gh-pages/package_esp32_index.json
    ```

    If you already have other URLs, you can separate them with commas.

1. Go to `Tools` > `Board` > `Boards Manager...`

1. Search for `ESP32` and install the `esp32` by `Espressif Systems` version `3.X`

1. Wait for the installation to be done.

1. In the top drop-down device selector, you should see new boards appearing. Select `DOIT ESP32 DEVKIT V1` for GyroCycle.

::: tip FOR WINDOWS USERS
In addition to the above, Windows users need to download the `CP210x Windows VCP Driver` in order to detect the microcontroller. You can download it [here](https://www.silabs.com/developer-tools/usb-to-uart-bridge-vcp-drivers?tab=downloads).
:::

### Installing libraries

GyroCycle uses several libraries in its code.

| Library | Author | Version used | Comment |
|-|-|-|-|
| [`Adafruit MPU6050`](https://docs.arduino.cc/libraries/adafruit-mpu6050/) | Adafruit | `2.2.6` | For communication with the gyroscope sensor. **Installing this library will also install others that it depends on.** |
| [`ESP32Servo`](https://docs.arduino.cc/libraries/esp32servo/) | Kevin Harrington, John K. Bennett | `3.0.6` | To communicate with the Servo from the ESP32. |
| [`ODriveArduino`](https://docs.arduino.cc/libraries/odrivearduino/) | ODrive Robotics Inc. | `0.10.1` | To communicate with the ODrive from the ESP32. Even though we do not use an Arduino, the library contains useful functions and constants. |
| [`RemoteXY`](https://docs.arduino.cc/libraries/remotexy/) | Evgeny Shemanuev, RemoteXY | `3.1.14` | We use [RemoteXY](https://remotexy.com/) to remotelly control the bicycle from a phone. |

## Configuring the ODrive

GyroCycle uses an ODrive motor driver for its flywheel. That driver can be extensively configured. For example, you can provide it with a maximum speed for safety.

Configuring the ODrive is performed through the [`odrivetool`](https://docs.odriverobotics.com/v/0.5.6/getting-started.html#start-odrivetool) CLI.

::: warning
When wiring your ODrive to your computer, don't forget to use the USB isolator.
:::

### Installing `odrivetool`

1. Make sure [`matplotlib`](https://matplotlib.org/) is installed

    ```
    pip install matplotlib
    ```

1. Install [`odrivetool`](https://docs.odriverobotics.com/v/0.5.6/getting-started.html#start-odrivetool)

    ```
    pip install --upgrade odrive
    ```

1. Make sure the libraries installed with `pip` are in your PATH (you should be able to run `odrivetool` as a command in your terminal once installed)

1. If you are a Windows user, you need to install an additional driver. Skip this step if you are not using Windows.

    1. Download the driver installer [here](https://github.com/pbatard/libwdi/wiki/Install#user-content-Latest_official_release) (`Latest official release` section).

    1. Make sure your ODrive device is connected to your PC via USB

    1. Run the installer

    1. In the installer, under `Options`, make sure `List All Devices` is checked

    1. Find the ODrive in the list. Select the `Native Interface`

    1. Aside of `WinUSB`, click on the bottom arrow until you reach `libusb`

    1. Click on `Replace Driver`

    1. Wait for the process to finish

    1. Unplug and replug your ODrive if `odrivetool` was already running

1. Open a terminal

    ::: tip
    If you are using Windows, open the terminal as an administrator.
    :::

1. Run `odrivetool`

1. After a while, it should print `Connected to ODrive ...`, meaning it has recongized the connected ODrive device

Now that you successfully installed `odrivetool`, you can use it to configure your ODrive as documented below.

### ODrive Configuration

We provide a Python script that you can run once `odrivetool` is installed and set up.

Clone the repository or download the configuration file and run it as a Python script. Wait for it to succeed.

```
git clone https://github.com/epfl-cs358/2024fa-gyrocycle.git gyrocycle
cd gyrocycle/code
python3 odrv_config.py
```

::: info
You might see some error messages when the ODrive is asked to reboot. This is expected, and you should see right below:

```
Reboot command failed - this is normal
```
:::

## Upload the code to the ESP32

You have everything set up and wired, it is now time to put software on this bicycle.

1. If not already done, clone the GitHub repository to get the code

    ```
    git clone https://github.com/epfl-cs358/2024fa-gyrocycle.git gyrocycle
    ```

1. In the repository, open the `/code/gyrocycle/gyrocycle.ino` file in Arduino IDE

1. Connect the ESP32 board to your PC

    ::: warning
    Don't forget the USB isolator.
    :::

1. Make sure to select `DOIT ESP32 DEVKIT V1` as board type in Arduino IDE

1. In the top left corner of Arduino IDE, click the right arrow button (`Upload`)

1. Wait for the compiling to be done, then for the uploading to be done

    ::: tip
    During the hours we spent uploading code to the bicycle, we noticed that some of our team members had to hold the reset button of the ESP32 pressed when the compiling stopped and the uploading started, for the microcontroller to be in download mode.

    We can't fully explain why, but it worked. If you have trouble uploading code to the ESP32, you might want to give that a try.
    :::

Once the upload is successful (no errors displayed in red in Arduino IDE's console), your microcontroller is ready to control the whole bicycle.

## Use the software

It is finally time to turn this bad boy on!

1. Start the bicycle with the On-Off switch

1. Plug your laptop to the ESP32 (don't forget the USB isolator)

Once plugged and started, you should see some data coming via serial.

```
MPU6050 found.
Configuring the sensor...
Sensor configured.
Accelerometer range set to: +-2G
Gyro range set to: +- 250 deg/s
Filter bandwidth set to: 260 Hz
[...]
Setup complete.
Starting in 3...
2...
1...
```

Once the `1...` has been printed, you have entered what we called the **configuration mode**. It is designed for you to be able to change many configuration options and values without re-compiling and re-uploading the code (helped us a lot while testing).

You can use the `help` command (send `help` via serial) to get a list of available commands with their description. The main ones are

- **`config <Kp> <Ki> <Kd> <max_fw_speed> <max_fw_torque> <odrive_max_speed>`**

    Defines the PID controller constants `Kp`, `Ki` and `Kd` to the provided values. Also defines the maximum speed at which the flywheel can rotate, and the maximum torque we can ask from the motor. Those are safety limits.

::: tip TODO
List other commands and document how to use the final code. There is no point in documenting how to use the code before it's finished.
:::

## Set up RemoteXY

You can use your mobile phone as a basic remote controller for the bicycle. You'll have to download the [RemoteXY](https://remotexy.com/en/download/) app on your phone.

Once you installed the app, make sure your bicycle is on, open the app and scan the following QR code:

<img src="/images/remotexy-qr.svg" width="300" />

It will tell your app all it needs to know to communicate with the ESP32.

That's pretty much it for setting up RemoteXY. Use the `ON/OFF` switch to enable/disable balancing. If disabled, the bicycle will stay still, otherwise it will balance itself using the flywheel.

The other sliders are for the Servo and the propulsion motor. Use them to steer the bike and make it go forward/backward.

::: tip TODO
Tell the reader whether the bike is expected to balance while in motion, once the bicycle is done. There's no point in documenting what might change.
:::

You now have a fully working reproduction of what we did as a semester project!