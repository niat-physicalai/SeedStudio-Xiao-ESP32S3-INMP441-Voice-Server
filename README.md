# SeedStudio XIAO ESP32S3 + INMP441 Voice Server

A simple LAN-based voice recording server using the **Seeed Studio XIAO ESP32S3** and **INMP441 I2S MEMS Microphone**. Recorded audio is stored on the ESP32S3 and can be downloaded through a web browser connected to the same network.

---

## Hardware Connections

Connect the INMP441 microphone to the XIAO ESP32S3 as shown below:

| INMP441 Pin | XIAO ESP32S3 Pin  |
| ----------- | ----------------- |
| SD          | D0                |
| VDD         | 3V3               |
| GND         | GND               |
| L/R         | GND *(Mono Mode)* |
| WS          | D2                |
| SCK         | D1                |

---

## How It Works

1. The INMP441 captures audio using the I2S interface.
2. The ESP32S3 stores recordings in LittleFS.
3. A web server running on the ESP32S3 provides access to recordings over the local network.
4. Users can start recordings and download saved WAV files directly from a web browser.

---

## Setup Instructions

### 1. Connect the Hardware

Wire the INMP441 microphone to the XIAO ESP32S3 according to the table above.

### 2. Configure Wi-Fi

Open the source code and update the Wi-Fi credentials:

```cpp
const char* ssid = "YOUR_WIFI_NAME";
const char* password = "YOUR_WIFI_PASSWORD";
```

> **Note:** The ESP32S3 and the device used to access recordings (laptop, phone, tablet, etc.) must be connected to the same network. A mobile hotspot can also be used.

### 3. Configure Arduino IDE

In Arduino IDE:

**Tools → USB CDC On Boot → Enabled**

### 4. Upload the Firmware

Compile and upload the sketch to the XIAO ESP32S3.

### 5. Verify Startup

Open the Serial Monitor after the board reboots.

A successful startup will display messages similar to:

```text
1: LittleFS.begin(true)
2: audioBuffer allocation
3: setupI2S();
4: WiFi.begin
5: Initialisation complete......

10.10.51.188
```

The last line is the IP address assigned to the ESP32S3.

---

## Accessing the Voice Server

1. Open a web browser on a device connected to the same Wi-Fi network.
2. Enter the displayed IP address in the address bar.

Example:

```text
http://10.10.51.188
```

3. The web interface will display:

   * Project title
   * Record button
   * Links to download up to three WAV recordings

---

## Recording Audio

After the first boot, no recordings will be available.

1. Click the **Record** button.
2. Audio will be captured and saved automatically.
3. Recordings are stored sequentially as:

   * Recording 1
   * Recording 2
   * Recording 3

When all slots are used, new recordings will overwrite older recordings according to the firmware logic.

---

## Downloading Recordings

Click any available recording link from the web page to download the corresponding WAV file.

The downloaded file can be played using any standard audio player on Windows, Linux, macOS, Android, or iOS.

---

## Features

* ESP32S3-based standalone voice recorder
* INMP441 digital I2S microphone support
* WAV file generation
* LittleFS storage
* Browser-based recording interface
* LAN access without external servers
* Supports Wi-Fi and mobile hotspot networks

---

## Troubleshooting

### No IP Address Appears

* Verify Wi-Fi credentials.
* Ensure the router is within range.
* Check that the ESP32S3 successfully connects to the network.

### No Audio in Recordings

* Verify all microphone connections.
* Confirm that the **L/R** pin is connected to **GND**.
* Check that the I2S pin configuration matches the wiring.

### Web Page Does Not Open

* Ensure both devices are connected to the same network.
* Verify the IP address shown in the Serial Monitor.
* Try refreshing the browser or restarting the ESP32S3.

---

## Example Output

```text
1: LittleFS.begin(true)
2: audioBuffer allocation
3: setupI2S();
4: WiFi.begin
5: Initialisation complete......

10.10.51.188
```

Open the displayed IP address in a browser to access the voice server interface.
