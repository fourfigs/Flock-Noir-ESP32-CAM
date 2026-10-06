<div align="center">
<img src="docs/logo.png" alt="Flock Noir" width="150">

# Flock Noir

**ALPR evidence, Axon alerts and wardriving on the Seeed XIAO ESP32-S3 Sense.**

By Your Pal Kal · Arduino / PlatformIO · [MIT](LICENSE) · Experimental
Port to ESP32-CAM +OV3660 (it said 2640) by Four Fig Newtons

</div>

**[INSTALL WITH THE XIAO WEB FLASHER](https://valleytechsolutions.github.io/Flock-Noir/)** · [0.6.0 release](https://github.com/valleytechsolutions/Flock-Noir/releases/tag/xiao-v0.6.0) · [Parts and wiring](HARDWARE.md) · [Detection details](docs/RADIO.md)

Flock Noir combines OPT101 pulse measurements, an IR-cut-free OV2640 and passive
Flock-You WiFi/OUI/BLE rules. It records **possible ALPR evidence**, including how
each event was detected. The dark green dashboard, VGA preview and editable
per-device sounds remain. No cloud account is needed.

**There is no verified universal ALPR flash rate in this project.** The optical
reference profile is **8–12 Hz, 10–30% duty, 8–35 ms pulses**. A matching light or
vendor OUI cannot prove a camera's identity. The sensors measure light intensity,
not wavelength. Nearby optical and radio sources may be different devices; a
quiet scan does not establish that cameras are absent. See [timing and validation](DETECTION.md).

## Four exclusive scan modes — XIAO 0.6.0

Choosing one of these tabs **starts that mode and stops the previous scanner**.
The active mode and resources appear above every tab. Camera and Settings are
views; opening them leaves the selected scan mode running.

| Scan tab | What runs | Alerts and logs |
|---|---|---|
| **ALPR** | OPT101 at ~1 kHz, camera brightness analysis, ALPR/Flock BLE and promiscuous WiFi/OUI/probe rules | Retro alerts; dedicated `/alpr/alpr_*.csv` with individual or combined detection methods |
| **SCANNER** | General WiFi/BLE device signatures, watchlist, RSSI tracking, Remote ID; optics paused | Editable sounds for ALPR, Axon, Ring, Meta, Flipper, Pineapple, Biscuit and drones; radio JSONL; ALPR radio matches also enter ALPR CSV |
| **PIG DETECTOR** | Axon BLE only; WiFi detection, camera analysis, OPT101, wardrive and recording paused | Axon siren immediately on fresh evidence, then every 10 seconds while matches continue; 5–60 second reminder setting; radio JSONL |
| **WARDRIVE** | WiFi network surveys and BLE advertisements; optical analysis and detection rules paused | No automatic detection tones; separate WiGLE CSV containing `WIFI` and `BLE` rows |

Pig Detector listens continuously for Axon company/service identifiers, advertised
Axon names and public-address Axon OUI hints. Vendor evidence means a **possible
Axon device**, not a guaranteed body camera or recording state. Its requested BLE
receive window is 90 ms per 100 ms with the dashboard, and 100 ms per 100 ms in
field coverage with WiFi off. Actual reception also depends on interference and
advertising behavior. Reminder sounds stop after 3 seconds without a matching
advertisement. Muting still permits logging; **Settings → Axon** changes its tone.

General Scanner retains Colonel Panic's Flock-You approach, including the full
34-prefix union, transmitter/receiver roles, wildcard-probe and strict IE evidence.
Weak shared-vendor hints stay distinct from stronger signatures. Biscuit uses its
name and optional service; the shared Arduino example UUID alone is insufficient.
Pineapple Pager is covered only as a Pineapple-family network-name candidate, with
no unique Pager model fingerprint. ESP32-S3 supports **2.4 GHz WiFi and BLE**;
it cannot survey Bluetooth Classic, 5/6 GHz WiFi or silent radios.

![Pig Detector preview with simulated Axon evidence](docs/screenshot-pig.png)

*UI preview uses simulated evidence, not a field detection.*

## Install and flash (XIAO ESP32-S3 Sense)

1. Open the **[web flasher](https://valleytechsolutions.github.io/Flock-Noir/)** in
   desktop Chrome or Edge. Connect the XIAO with a USB **data** cable and close
   serial monitors using its port.
2. Select **Connect & Install**, choose the XIAO serial port and follow the prompts.
   Leave **Erase device unchecked** when updating to keep saved settings.
   The installer uses split images that leave the NVS settings area intact.
3. If USB discovery fails, hold **BOOT**, tap **RESET**, then release BOOT and
   reconnect. Tap RESET after installation if it remains in the bootloader.
4. Join WiFi **Flock Noir**, password **flocknoir**, and open **http://192.168.4.1**.
   Your phone may report that the hotspot has no internet; stay connected.
5. Select **ALPR**, **Scanner**, **Pig Detector** or **Wardrive**. Saved mode and
   tone settings persist. On upgrades, the previous ALPR/General selection migrates.

This binary is for the **Seeed XIAO ESP32-S3 Sense with OV2640**. A chip-family check
cannot distinguish every S3 board. **Do not flash this XIAO binary onto an
ESP32-CAM.** This fork has a separate `esp32_cam` source-build target with different
pins; see the [ESP32-CAM instructions below](#esp32-cam--ov3660-port).
Raspberry Pi Zero 2 W remains on its separate **0.5.1** release and
[Pi guide](pi/README.md).

### Radio coverage

Coverage is separate from the scan mode. **DASHBOARD ON** keeps the hotspot available;
WiFi detection listens on its channel while a client is connected. Wardrive can
run passive full-channel surveys when no dashboard clients are connected.

**FIELD SCAN · HOTSPOT OFF** gives the radio broader coverage: ALPR uses channels
1/6/11 with 350 ms dwell; Wardrive uses channels 1–11; General uses the selected
channel plan. BLE continues alongside WiFi. Pig Detector instead switches WiFi
off and reserves its scan window for Axon BLE. **Hold BOOT for 1.5 seconds** to
restore the dashboard without changing the chosen scanner. Boot always restores
the hotspot, even if the saved scan mode is Pig Detector or Wardrive.

## Parts and wiring

### ESP32-CAM + OV3660 port

This fork uses an **ESP32-CAM with AI Thinker camera connector wiring** and an
**OV3660** (the camera supplied with this build was advertised as OV2640).
Use the ESP32-CAM pin map below, not the XIAO diagram or D-pin labels.

**Hardware status — October 6, 2026:** the final sensor has not arrived, and the
ATGM336H is still not powering up. GPS operation and the complete sensor setup
remain unverified. The GPS and buzzer connections below describe the firmware
configuration, not a tested complete assembly.

Parts for this port: ESP32-CAM and compatible camera, stable board power supply,
USB programming adapter/base, FAT32 microSD, small **passive** piezo, ~100 Ω
series resistor, hookup wires, and ATGM336H GPS with antenna once its power issue
is resolved.

| Connection | ESP32-CAM pin / setting |
|---|---|
| Camera | Existing ribbon connector; AI Thinker camera pin map |
| ATGM336H TX → ESP RX | **U0R / GPIO3**, **9600 baud** |
| ATGM336H RX | Leave unconnected; receive-only GPS (`GPS_TX_PIN = -1`) |
| ATGM336H GND | Common **GND** with ESP32-CAM |
| ATGM336H VCC | Supply matching the exact module/breakout rating; power-up is unresolved |
| Passive piezo signal / return | **GPIO12 → ~100 Ω → piezo → GND** |
| Onboard microSD | **SPI mode**: CS **GPIO13**, SCK **GPIO14**, MISO **GPIO2**, MOSI **GPIO15** |
| OPT101 analog output | **Not connected / unsupported in this configuration** (`IR_SENSOR_PIN = -1`) |

Disconnect power before changing wiring. GPS power requirements depend on the
exact breakout; do not assume the ESP32-CAM's board supply is also suitable for
GPS VCC. Confirm the module labels, rated input voltage, polarity and voltage at
its VCC/GND terminals before reconnecting the UART. A satellite-fix test must
wait until the power issue is resolved.

**GPIO3 is shared with the programming adapter's TX line.** Disconnect GPS TX
while flashing. For GPS operation, disconnect the adapter's TX connection to
GPIO3 before reconnecting GPS TX, so two transmitters do not drive the same pin.
The firmware reserves GPIO3 for GPS reception and keeps console output on GPIO1;
serial console input commands are unavailable in this configuration. Use the web
dashboard for controls.

**GPIO12 is a boot-strapping pin:** the buzzer circuit must not pull it high at
reset. Use a small passive piezo as shown, not an active buzzer module with a
pull-up. The SD socket uses SPI in this firmware; do not substitute a generic
ESP32-CAM SD_MMC wiring guide.

**OPT101 does not yet have a supported analog connection on this port.**
No free ADC1 input has been confirmed with the camera connected, and
GPIO2 belongs to microSD. Keep the IR photodiode setting off; do not copy the
XIAO OPT101-to-GPIO2 wiring below. Receiving the sensor alone will not enable
this path without a verified interface and corresponding firmware support.

Build and upload the board-specific firmware with PlatformIO:

```sh
python -m platformio run -e esp32_cam
python -m platformio run -e esp32_cam -t upload
```

After flashing, return the board to normal boot, join **Flock Noir** with password
**flocknoir**, and open **http://192.168.4.1**. Check the camera preview, SD status
and buzzer separately. Camera/radio checks can proceed while GPS and the final
sensor are pending. GPS-tagged logging and WiGLE survey output require a working
receiver and a valid fix; missing coordinates are expected until then.

Open **http://192.168.4.1/api/status** in a browser for the system-status page,
which refreshes every two seconds. For raw status data, use
**http://192.168.4.1/api/status?format=json**. An enabled GPS or buzzer setting
does not confirm that the physical module is powered or working.

### XIAO ESP32-S3 Sense reference build

Use the [XIAO bill of materials and assembly guide](HARDWARE.md). For the complete
ALPR build: XIAO S3 **Sense**, compatible **OV2640 without its IR-cut filter**,
**3.3 V OPT101 analog module**, ATGM336H GPS, antenna, FAT32 microSD, a passive piezo,
wires and a USB data cable/power source. No additional bare photodiode or op-amp
is needed when using OPT101.

![OPT101, GPS and buzzer wiring](docs/wiring-opt101.svg)

| Connection | XIAO header / GPIO |
|---|---|
| OPT101 VCC, GND, OUT | **3V3**, **GND**, **D1 / GPIO2** |
| ATGM336H TX → ESP RX | **D7 / GPIO44**, **9600 baud** |
| ATGM336H RX ← ESP TX (optional) | **D6 / GPIO43** |
| Passive piezo signal / return | **D0 / GPIO1** / **GND**; ~100 Ω series resistor |
| Sense camera and microSD | Existing ribbon/board connections; SD CS **GPIO21** |

**D1 is the second left pin** with USB at the top and the component side facing
you. Disconnect power before wiring. Follow the breakout labels; terminal order
varies. Use a 3.3 V-compatible module and never feed 5 V into the ADC. Confirm the
ATGM336H breakout's supply rating before connecting power; the UART pin map here
applies only to the XIAO reference build.

Select **ALPR**, enable **IR photodiode sensor** after connecting OPT101, and cover /
uncover it to check the raw count and waveform. First-install sensor default is
off; updating preserves your saved enable setting. ADC activity alone does not
prove the module is connected. Keep camera and OPT101 aimed at the same area.

## ALPR evidence and CSV

The ALPR tab shows four evidence indicators and their ages. The 3-second
correlation window supports both radio-first and optical-first encounters, with
strong evidence expiring independently of newer weak hints.

- **OPT101:** ~1 kHz ADC, adaptive noise threshold, pulse width/duty, at least four
  consecutive valid intervals, regularity, clipping and sampling-gap checks.
- **Camera:** 640×480 JPEG with fixed exposure/gain, two PSRAM buffers and a separate
  80×60 brightness analysis task. Around 25 fps cannot measure a 20 ms pulse exactly.
  A 10 Hz / 20 ms test light can appear as 5 Hz; such events retain the **observed**
  frequency and `camera_timing=aliased_candidate`, with reduced confidence.
- **Radio:** Flock-You OUI/probe evidence and BLE names/services. OUI-only matches
  remain candidates. Source proximity strengthens evidence but does not bind a
  light source to a specific MAC.

**[ALPR CSV on your device](http://192.168.4.1/api/alpr.csv)** is also available
through the ALPR tab. Its 31 columns include observation/log timestamps, GPS,
`source`, measured `freq_hz` and `duty`, `detection_method` (`ir`, `camera`, `ble`,
`wifi`, or combinations), category, assessment, MAC/RSSI, radio rule/tier,
`ir_timing_match`, `camera_pattern`, `ir_pulse_ms`, `ir_sample_hz`,
`camera_sample_hz`, `camera_timing` and `scan_mode`. Unmeasured radio-only pulse
fields remain empty. Axon/Meta/etc. never enter this ALPR file. Optical events
still log without GPS, with unavailable coordinates and a boot-relative timestamp.

| Data | SD location | Download |
|---|---|---|
| ALPR / optical candidates | `/alpr/alpr_*.csv` | ALPR CSV; `/api/log` remains an alias |
| WiGLE surveys | `/wardrive/wigle_*.csv` | Wardrive → WiGLE CSV |
| Radio detection events | `/radio/events_*.jsonl` | Scanner/Pig Detector → radio event log |
| Optional General captures | `/radio/wifi_*.pcap`, `/radio/ble_*.jsonl` | Scanner → capture downloads |
| Recordings | `/videos/` | Camera → saved recordings |

Wardrive logs both **WiFi APs** and **BLE advertisers**, including unrecognized
vendors. It saves repeat measurements every 15 seconds per protocol/address,
requires a fresh GPS position, UTC, altitude and HDOP, and reports skipped
observations/SD errors. `AccuracyMeters` is explicitly **HDOP × 5 m estimated
accuracy**, not a measurement from the receiver. BLE channel is **0 (unknown)**;
SSID/name quoting is CSV-safe and line breaks become spaces for WiGLE import.
The format follows [WiGLE's published parser](https://github.com/wiglenet/wigle-wifi-wardriving/blob/main/wiglewifiwardriving/src/main/java/net/wigle/wigleandroid/util/NetworkCsv.java).

## Build, test and USB tools

Dependencies are pinned in [platformio.ini](platformio.ini): pioarduino 55.03.39,
Arduino ESP32 3.3.9 and TinyGPSPlus 1.0.3. No new runtime library is needed for 0.6.0.

```sh
python tools/html2header.py
python -m platformio run -e xiao_esp32s3_sense
python -m platformio run -e xiao_esp32s3_sense -t upload
python tools/package_firmware.py
python tools/build_flasher.py
```

PlatformIO uploads split images. A manual merged image at **0x0** includes NVS
padding and can reset settings; use the web flasher or PlatformIO for updates.
Firmware checksums and validation notes are in [binaries/BUILD.md](binaries/BUILD.md).
Tagging `xiao-v*` runs regression/browser/build checks, publishes the board release
and deploys its matching flasher images to GitHub Pages.

USB serial runs at 115200 baud. Commands: `CMD:HEALTH`, `CMD:STATUS`,
`CMD:PROFILE:alpr`, `CMD:PROFILE:general`, `CMD:PROFILE:axon`,
`CMD:PROFILE:wardrive`, `CMD:COVERAGE:dashboard`, `CMD:COVERAGE:field`,
`CMD:DUMP_LIVE`, and `CMD:TEST_SOUND:axon`. A tone preview tests the buzzer;
it does not simulate a detection.

Host tests cover timing, aliasing, mode filtering, stale reminder suppression,
packet parsing, all 34 Flock OUIs, evidence expiry, WiGLE serialization and JPEG
analysis. Browser tests exercise mode changes, failures, mobile layout, sound
settings and preview recovery. Hardware smoke tests are opt-in; field range,
false-positive rates, actual ALPR pulse profiles and Axon beacon coverage require
real target measurements.

## Credits and attributions

Flock Noir builds on the work of the counter-surveillance and wardriving community.
Thanks to:

- **Colonel Panic, OUI Spy** ([github.com/colonelpanichacks/oui-spy](https://github.com/colonelpanichacks/oui-spy),
  [colonelpanic.tech](https://colonelpanic.tech/), [Tindie](https://www.tindie.com/products/colonel_panic/oui-spy/)).
  OUI Spy pioneered approachable ESP32 hardware for passively detecting surveillance
  devices, and was a primary inspiration for the signature-detection approach here.
- **Noflock / Flock-IR-Detection** ([github.com/Noflock/Flock-IR-Detection](https://github.com/Noflock/Flock-IR-Detection)).
  The photodiode circuit and the edge/period IR-detection algorithm in Flock Noir follow this
  project's photodiode pulse-detection research; Flock Noir still requires field validation.
- **justcallmekoko, ESP32 Marauder** ([github.com/justcallmekoko/ESP32Marauder](https://github.com/justcallmekoko/ESP32Marauder)).
  The reference open-source ESP32 wireless research toolkit; its approachable, hackable
  design for wardriving and radio recon shaped how Flock Noir's wireless side is built.
- **Midwest Gadgets (@hamspiced), Piglet wardriver** ([github.com/hamspiced/piglet](https://github.com/hamspiced/piglet),
  [midwestgadgets.org](https://www.midwestgadgets.org/product-page/piglet)). The Wi-Fi
  wardriving side of this project, WiGLE-format logging on the XIAO with a web UI, is
  modeled on Piglet's clean, transparent design.
- **[Seeed Studio](https://www.seeedstudio.com/)** for the XIAO ESP32-S3 Sense platform and
  its [documentation](https://wiki.seeedstudio.com/xiao_esp32s3_getting_started/).
- **[Espressif](https://github.com/espressif/arduino-esp32)** (Arduino-ESP32 core,
  esp32-camera) and Mikal Hart ([TinyGPSPlus](https://github.com/mikalhart/TinyGPSPlus)).
- **Four Fig Newtons** for the **ESP32-CAM + OV3660 port**, adapting Flock Noir beyond the
  original Seeed XIAO ESP32-S3 Sense + OV2640 hardware target.
- **[WiGLE](https://wigle.net)** for the wardriving CSV format and mapping ecosystem.

If you build on this, please keep these attributions and add your own.

---

## License

Released under the [MIT License](LICENSE): free to use, modify, and distribute, with
attribution. See the license file for the full text and the experimental-software notice.

---

## Legal and ethics

- This tool is for lawful security research, education, and personal privacy awareness.
- Wardriving: logging the existence of broadcast Wi-Fi beacons is legal in many places, but
  you are responsible for the laws in your jurisdiction. Do not connect to, probe, or
  interfere with networks you do not own. Never log or transmit personal data.
- Recording: audio and video recording laws vary widely (one-party versus all-party
  consent, public versus private spaces). Know and follow your local rules before recording.
- The IR detector is a research aid, not evidence. Do not use its output to harass, target,
  or make claims about any person, property, or organization.
- Provided as is, without warranty. You assume all risk and responsibility.

---

<div align="center">

Made by Your Pal Kal

</div>

See [ATTRIBUTIONS.md](ATTRIBUTIONS.md) for the full OUI Spy / Unified Blue credits, support links and research provenance.
