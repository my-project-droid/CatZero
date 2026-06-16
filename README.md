# 🐱 CatZero OS

An advanced, open-source 2.4GHz, Sub-GHz, and Infrared tactical cybersecurity development platform. CatZero is built explicitly to bridge the gap between low-frequency hardware hacking, passive anti-surveillance intelligence, and aggressive wireless security auditing. 

Unlike consumer multi-tools that restrict capabilities behind rigid, locked down firmware, CatZero is completely standalone, modular, and optimized for discrete field deployment. By combining a classic dual-core processing architecture with a high-contrast 1.18-inch (3cm) OLED display, an external antenna, and a streamlined 1-Button UI Shell, CatZero transforms standard microcontrollers into weaponized environmental reconnaissance units.

---

## ⚡ Technical Architecture: CatZero vs. The Industry

To understand the tactical advantage of CatZero, it must be compared directly against popular consumer and DIY alternatives:

### 1. The Flipper Zero Conundrum
* **The Industry Flaw:** Out of the box, the Flipper Zero has **no native Wi-Fi hardware** and its internal Bluetooth chip is locked down to basic smartphone app syncing. To run a simple 2.4GHz test, users must buy and stack external daughterboards on top of the device. 
* **The CatZero Override:** Powered natively by an **ESP32-U** dual-core 240MHz processor, CatZero handles high-speed wireless traffic natively. Because it utilizes a physical, high-frequency **U.FL coaxial connector**, operators can attach high-gain external antennas (Ducky, Panel, or directional Yagi antennas) to monitor networks, deauthenticate targets, and track signals from blocks away.

### 2. The LILYGO T-Embed Limitation
* **The Industry Flaw:** The T-Embed features a bright color screen and a scroll wheel, but relies entirely on tiny, internal PCB patch antennas. This restricts its effective wireless range to a single room unless you manually break out a soldering iron to modify the traces. 
* **The CatZero Override:** CatZero prioritizes field utility and range over flashy colors. By sacrificing the color LCD for an ink-black **1.18-inch (3cm) Monochrome OLED**, any pixel rendering black is completely powered down. This yields a massive reduction in battery draw (2-15mA max) and guarantees absolute visual discretion in low-light environments with zero backlight bleed.

### 3. The Hack Gears RootKitten Restriction
* **The Industry Flaw:** The RootKitten uses a standard 0.96-inch micro-stamp screen, which heavily limits menu readability and data layouts, forcing software to truncate essential text logs. It treats security testing strictly as a linear list of tools.
* **The CatZero Override:** CatZero leverages its expanded 3cm display canvas to render complex real-time graphics—such as the **Wi-Fi Channel Analyzer's dynamic bar chart** and a live **RSSI Signal Locating Radar**. Additionally, it introduces an active, persistent game loop that tracks field actions and translates them into an immersive progression ecosystem.

---

## ⚔️ Armed Tactical Modules & Feature Breakdown

CatZero organizes its extensive hacking and anti-surveillance suites into four core software layers, seamlessly toggled via animated, low-profile protocol logo wheels:

### 📡 1. Wi-Fi Pentesting & Anti-Surveillance
* **Channel Analyzer:** Visually maps out local 2.4GHz network congestion across all channels (1-13) using a real-time, bouncing bar chart to instantly pinpoint the optimal channel for exploitation.
* **Passive Camera Detector:** Promiscuously sniffs nearby wireless traffic to identify hidden security cameras. By analyzing MAC OUI prefixes and raw frame heuristics, it finger prints and displays devices from **Ring, Blink, Nest, Arlo, Wyze, Reolink, Eufy, Hikvision, Dahua, Axis, and 20+ other brands** without transmitting a single probe packet.
* **Camera Deauther:** Locks onto any fingerprinted security camera discovered during scanning and broadcasts automated deauthentication packets to forcibly kick it off its wireless access point, blinding its feed.
* **Pineapple & Pwnagotchi Detectors:** Passively listens for unique beacon frame profiles, MAC signatures, and handshakes to reveal the physical presence of nearby Hak5 Wi-Fi Pineapples or Pwnagotchi AI sniffing rigs.
* **Pwnagotchi Spam:** A grid-flooding tool that floods local airwaves with hundreds of fake Pwnagotchi identities and randomized beacon profiles to confuse surrounding automated scanners.
* **Drone Detector:** Intercepts and parses raw **Remote ID (ODID)** broadcast packets sent over Wi-Fi and Bluetooth per the ASTM F3411 specification. Displays live drone identification, GPS coordinates, altitude, airspeed, and operator details.
* **Drone Spoofer:** Generates and broadcasts fabricated Open Drone ID packets, simulating randomized drone identities, flight vectors, speeds, and fake operator positions.
* **Flock Detector:** Employs dual-mode Wi-Fi and BLE promiscuous frame capture to spot **Flock Safety surveillance cameras**, detecting them accurately even when they are not actively broadcasting an open SSID.
* **Device Scout:** A passive anti-surveillance engine that continuously tracks surrounding MAC addresses, ranking them by persistence over time to identify hidden GPS trackers or stalker devices following your path.
* **Advanced Networking Frame:** Includes raw **LAP Sniffing** utilities, a streamlined **Kismet** data-logging engine, multi-target **Deauth Flooding**, **Karma Attacks**, and credential-harvesting **Evil Portals**.

### 💙 2. BLE Proximity Suite (Bluetooth Low Energy)
* **BLE Inspector:** Decodes raw BLE advertising packets in real time, breaking down service UUIDs, manufacturer data, TX power parameters, flags, and raw data payloads.
* **nyanBOX & Flipper Scanners:** Monitors proximity channels to instantly alert the operator to the presence of nearby nyanBOX or Flipper Zero devices, detailing their signal strength, firmware version, and level.
* **Axon Detector:** Specialized sniffing routine designed to detect and log nearby **Axon law enforcement equipment**, including body cameras and tasers.
* **Meshtastic & MeshCore Detectors:** Identifies and charts nearby off-grid mesh routing nodes running Meshtastic or MeshCore firmware.
* **Skimmer Detector:** Scans for standard HC-03, HC-05, and HC-06 Bluetooth serial modules, which are overwhelmingly used by criminals inside gas pump and ATM credit card skimming devices.
* **Tracker Detectors (AirTag / SmartTag / Tile):** Sweeps and isolates encrypted proximity beacons to identify nearby Apple AirTags, Samsung SmartTags, or Tile tracking hardware.
* **AirTag & BLE Spoofers:** Clones and rebroadcasts detected tracker signatures. The BLE Spoofer can achieve complete 1:1 replication of a target's MAC address, device name, scan responses, and connectable state.
* **RayBan Detector:** Automatically identifies and logs the presence of nearby Ray-Ban Meta smart glasses.
* **iBeacon Detector:** Decodes Apple iBeacon advertisements used in retail spaces, venues, and transit hubs to actively map and track human foot traffic.
* **Tactical Spam Engines:**
  * **Swift Pair:** Triggers rapid, cascading Windows Swift Pair pairing notifications on nearby computers.
  * **Sour Apple:** Broadcasts specialized Apple proximity packets to trigger rapid AirPods Pro or Apple TV pairing loop animations on nearby iOS devices.
  * **Sour Droid:** Floods nearby Android and Samsung hardware by cycling through hundreds of Google FastPair and Samsung EasySetup device models to test protocol resilience.
  * **Spam All:** Executes an aggressive, cyclical loop running through all spam protocols sequentially (Android ➡️ Samsung ➡️ Windows ➡️ Apple).

### 📻 3. Sub-GHz Radio Subsystem (CC1101)
* **ProtoPirate Automotive Engine:** A powerful 11-protocol vehicle key fob decoder. It allows operators to select, capture, parse, and save radio signatures across **Kia, Chrysler, Jeep, StarLine, Scher-Khan, Subaru, Fiat, Ford, Suzuki, Toyota, and more**.
* **Automated Frequency Shifting:** Short-clicking through the ProtoPirate car list automatically configures your wired CC1101 module to shift between global standard frequencies (**315.00 MHz, 433.92 MHz, and 868.35 MHz**) to match that brand's regional factory specifications.
* **RollJam Attack Framework:** Implements a hardware-level reactive jam-and-capture routine to safely bypass automotive rolling code security measures during authorized testing.
* **Signal Lock & Jamming:** Offers dynamic full-band and intermittent signal jamming alongside a **Signal Lock** utility to clamp the CC1101 receiver onto a moving frequency.
* **Visual Spectrum Analyzer:** Utilizes the 3cm OLED to map out real-time RSSI spectrums, spectrogram grids, and raw square-wave readouts.

### 👁️ 4. Infrared Control Module
* **TV-B-Gone:** Rapidly loops through hundreds of universal power codes to instantly turn off display walls, televisions, and public digital signage.
* **IR Read/Replay:** Intercepts proprietary consumer remote signatures via the IR receiver, decodes the protocol, saves it internally, and replays it through the TX blaster.
* **IR Jammer & Brute-Forcer:** Blasts high-frequency infrared noise to flood local receivers, alongside automated remote code brute-forcing sequences.
* **IR2Keyboard:** A specialized bridging routine that translates incoming infrared remote clicks into direct computer keyboard shortcuts and terminal commands.

---

## 🎮 The 9-Rank Level Progression Engine

CatZero replaces static, dry menu layouts with an integrated RPG progression framework that dynamically tracks and rewards field usage:

```text
       [ Kitten ] ---> [ Scout ] ---> [ Sniffer ] ---> [ Prowler ] 
          (L1-4)         (L5-9)        (L10-19)        (L20-29)
            |
            v
       [ Phreaker ] -> [ Operator ] -> [ Ghost ] ----> [ Specter ] ---> [ Nyan-Lord ]
         (L30-44)       (L45-59)       (L60-74)         (L75-89)          (L90-99)
```
