#include <Arduino.h>
#include <U8g2lib.h>
#include <OneButton.h>

// 1. HARDWARE CONFIGURATION
#define BUTTON_PIN 13
#define SCREEN_SDA 21
#define SCREEN_SCL 22

// Initialize your 3cm OLED (128x64 I2C)
U8G2_SSD1306_128X64_NONAME_F_HW_I2C u8g2(U8G2_R0, /* reset= */ U8X8_PIN_NONE, SCREEN_SCL, SCREEN_SDA);

// Set up the single button controller
OneButton button(BUTTON_PIN, true); // true = active LOW (pullup resistor style)

// 2. STATE MACHINE FOR MENUS
enum OperatingMode { BOOT_SCREEN, LOGO_CYCLE, MENU_DISPLAY, ACTION_RUNNING };
OperatingMode currentMode = BOOT_SCREEN;

enum ProtocolCategory { WIFI, BLUETOOTH, SUB_GHZ, INFRARED, EXTRAS };
ProtocolCategory activeProtocol = WIFI;

// Menu variables
int currentMenuLine = 0;
int maxMenuLines = 0;
bool insideSubMenu = false;
int currentSubMenuLevel = 0; // 0 = Main Categories, 1 = First Level Sub-Menu, 2 = Nested Sub-Menu

// 3. MENU TEXT ARRAYS (The layouts you requested)
const char* wifiMenu[] = {
  "[ BACK ]", "Connect WiFi", "WiFi AP", "WiFi Atks", "Target Atks", 
  "Karma Attack", "Beacon Spam", "Deauth Flood", "Evil Portal", 
  "Listen TCP", "Client TCP", "TelNet", "SSH", "DPWO-ESP32", 
  "RAW Sniffer", "Scan Hosts", "Host Info", "WireGuard Tunneling", 
  "Brucegotchi", "Responder", "SOCKS4 Proxy", "Wifi Password Recovery", "WiFi Brute Force"
};
const char* beaconSubMenu[] = { "[ BACK ]", "Funny SSID", "Rick Roll", "Random SSID", "Custom SSID" };
const char* targetAtksSubMenu[] = { "[ BACK ]", "SSH Connect", "Station Deauth", "ARP Spoofing", "ARP Poisoning" };

const char* bleMenu[] = {
  "[ BACK ]", "Disconnect BLE", "Media Commands", "BLE Scan", "BadBLE", 
  "BLE Spam Features", "Windows Spam", "Android Spam", "Samsung Spam", 
  "Spam All", "Custom Spam", "iBeacon", "BLE Suite"
};
const char* bleSpamSubMenu[] = { "[ BACK ]", "AirPods Pro", "Apple TV Popup", "SourApple Crash", "AppleJuice" };
const char* bleSuiteSubMenu[] = { 
  "[ BACK ]", "Quick Vuln Scan", "Deep Profile", "FastPair Suite", 
  "HFP Suite", "Audio Suite", "HID Suite", "Memory Corruption", "DoS Attacks" 
};

const char* rfMenu[] = {
  "[ BACK ]", "Scan/Copy", "Record Raw", "Custom SubGhz", "Spectrum", 
  "RSSI Spectrum", "SquareWave Spec", "Spectogram", "Listen", 
  "Bruceforce", "Jammer Intermittent", "Jammer Full", "RF Brute Force", "ProtoPirate", "Config"
};
const char* rfScanSubMenu[] = { "[ BACK ]", "Mode", "Filter", "Replay", "Save", "CC1101 Settings" };
const char* cc1101SubMenu[] = { "[ BACK ]", "Range", "Threshold" };

const char* irMenu[] = {
  "[ BACK ]", "TV-B-Gone", "Custom IR", "IR Read/replay", "IR Jammer", "IR Brute Force", "IR2Keyboard"
};

const char* extrasMenu[] = {
  "[ BACK ]", "Timer", "Games", "Clock", "Calculator"
};
const char* gamesSubMenu[] = { "[ BACK ]", "Snake", "Tetris", "Racing", "Magic 8 Ball", "Space Shooter", "Dino", "Tamagotchi Cat" };

// Global pointer tracking active item context
const char** activeMenuSource;
unsigned long bootTimer = 0;

// 4. DRAWING UTILITIES
void drawCatFace(int x, int y) {
  // Simple geometric ASCII-style cat for your screen
  u8g2.drawTriangle(x, y+10, x+8, y, x+12, y+10);     // Left Ear
  u8g2.drawTriangle(x+24, y+10, x+28, y, x+36, y+10); // Right Ear
  u8g2.drawFrame(x+4, y+10, 28, 16);                 // Face outline
  u8g2.drawBox(x+9, y+15, 3, 3);                      // Left Eye
  u8g2.drawBox(x+24, y+15, 3, 3);                     // Right Eye
  u8g2.drawTriangle(x+16, y+20, x+14, y+18, x+18, y+18); // Nose
}

void drawProtocolLogo(ProtocolCategory proto) {
  u8g2.clearBuffer();
  u8g2.setFont(u8g2_font_open_iconic_all_4x_t);
  
  switch(proto) {
    case WIFI:
      u8g2.drawGlyph(48, 40, 247); // WiFi Icon Code
      u8g2.setFont(u8g2_font_profont12_mf);
      u8g2.drawStr(40, 56, "[ WiFi ]");
      break;
    case BLUETOOTH:
      u8g2.drawGlyph(48, 40, 212); // Bluetooth Icon Code
      u8g2.setFont(u8g2_font_profont12_mf);
      u8g2.drawStr(24, 56, "[ Bluetooth ]");
      break;
    case SUB_GHZ:
      u8g2.drawGlyph(48, 40, 89);  // Radio Tower Icon
      u8g2.setFont(u8g2_font_profont12_mf);
      u8g2.drawStr(32, 56, "[ Sub-GHz ]");
      break;
    case INFRARED:
      u8g2.drawGlyph(48, 40, 119); // Remote Controller Icon
      u8g2.setFont(u8g2_font_profont12_mf);
      u8g2.drawStr(32, 56, "[ Infrared ]");
      break;
    case EXTRAS:
      u8g2.drawGlyph(48, 40, 172); // Game Controller / Extra Gear Icon
      u8g2.setFont(u8g2_font_profont12_mf);
      u8g2.drawStr(40, 56, "[ Extras ]");
      break;
  }
  u8g2.sendBuffer();
}

// 5. BUTTON CLICKS & HOLD HANDLERS
void handleShortClick() {
  if (currentMode == LOGO_CYCLE) {
    // Cycle through main functional options
    activeProtocol = static_cast<ProtocolCategory>((activeProtocol + 1) % 5);
    drawProtocolLogo(activeProtocol);
  } 
  else if (currentMode == MENU_DISPLAY) {
    // Scroll down line by line inside lists
    currentMenuLine++;
    if (currentMenuLine >= maxMenuLines) {
      currentMenuLine = 0; // Wrap back to the top element
    }
    updateMenuScreen();
  }
  else if (currentMode == ACTION_RUNNING) {
    // Short press stops execution and returns up one level
    currentMode = MENU_DISPLAY;
    updateMenuScreen();
  }
}

void handleLongPress() {
  if (currentMode == LOGO_CYCLE) {
    // Lock into targeted sub-system menu layout
    currentMode = MENU_DISPLAY;
    insideSubMenu = false;
    currentSubMenuLevel = 0;
    currentMenuLine = 0;
    loadMenuContext();
  } 
  else if (currentMode == MENU_DISPLAY) {
    // Handle selections inside list states
    String selection = String(activeMenuSource[currentMenuLine]);
    
    if (selection == "[ BACK ]") {
      goBackALevel();
    } else {
      processMenuSelection(selection);
    }
  }
}

// 6. DYNAMIC LIST MANAGEMENT
void loadMenuContext() {
  if (!insideSubMenu) {
    currentSubMenuLevel = 0;
    switch(activeProtocol) {
      case WIFI:      activeMenuSource = wifiMenu;      maxMenuLines = sizeof(wifiMenu)/sizeof(wifiMenu[0]); break;
      case BLUETOOTH: activeMenuSource = bleMenu;       maxMenuLines = sizeof(bleMenu)/sizeof(bleMenu[0]); break;
      case SUB_GHZ:   activeMenuSource = rfMenu;        maxMenuLines = sizeof(rfMenu)/sizeof(rfMenu[0]); break;
      case INFRARED:  activeMenuSource = irMenu;        maxMenuLines = sizeof(irMenu)/sizeof(irMenu[0]); break;
      case EXTRAS:    activeMenuSource = extrasMenu;    maxMenuLines = sizeof(extrasMenu)/sizeof(extrasMenu[0]); break;
    }
  }
  updateMenuScreen();
}

void processMenuSelection(String item) {
  // Nested Folder Routines
  if (item == "Beacon Spam") {
    activeMenuSource = beaconSubMenu; maxMenuLines = sizeof(beaconSubMenu)/sizeof(beaconSubMenu[0]);
    currentMenuLine = 0; insideSubMenu = true; currentSubMenuLevel = 1;
  } else if (item == "Target Atks") {
    activeMenuSource = targetAtksSubMenu; maxMenuLines = sizeof(targetAtksSubMenu)/sizeof(targetAtksSubMenu[0]);
    currentMenuLine = 0; insideSubMenu = true; currentSubMenuLevel = 1;
  } else if (item == "BLE Spam Features") {
    activeMenuSource = bleSpamSubMenu; maxMenuLines = sizeof(bleSpamSubMenu)/sizeof(bleSpamSubMenu[0]);
    currentMenuLine = 0; insideSubMenu = true; currentSubMenuLevel = 1;
  } else if (item == "BLE Suite") {
    activeMenuSource = bleSuiteSubMenu; maxMenuLines = sizeof(bleSuiteSubMenu)/sizeof(bleSuiteSubMenu[0]);
    currentMenuLine = 0; insideSubMenu = true; currentSubMenuLevel = 1;
  } else if (item == "Scan/Copy") {
    activeMenuSource = rfScanSubMenu; maxMenuLines = sizeof(rfScanSubMenu)/sizeof(rfScanSubMenu[0]);
    currentMenuLine = 0; insideSubMenu = true; currentSubMenuLevel = 1;
  } else if (item == "CC1101 Settings") {
    activeMenuSource = cc1101SubMenu; maxMenuLines = sizeof(cc1101SubMenu)/sizeof(cc1101SubMenu[0]);
    currentMenuLine = 0; insideSubMenu = true; currentSubMenuLevel = 2;
  } else if (item == "Games") {
    activeMenuSource = gamesSubMenu; maxMenuLines = sizeof(gamesSubMenu)/sizeof(gamesSubMenu[0]);
    currentMenuLine = 0; insideSubMenu = true; currentSubMenuLevel = 1;
  } else {
    // If it's not a folder, it's an attack! Execute UI Action screen
    currentMode = ACTION_RUNNING;
    u8g2.clearBuffer();
    u8g2.setFont(u8g2_font_profont12_mf);
    u8g2.drawStr(0, 15, "EXECUTING:");
    u8g2.drawStr(0, 35, item.c_str());
    u8g2.drawStr(0, 58, "[Press to Stop]");
    u8g2.sendBuffer();
    
    // TODO: This is where you would place your real CC1101/IR/WiFi functions!
  }
  if (currentMode == MENU_DISPLAY) updateMenuScreen();
}

void goBackALevel() {
  if (currentSubMenuLevel == 2) { // From CC1101 options back to Scan/Copy options
    activeMenuSource = rfScanSubMenu; maxMenuLines = sizeof(rfScanSubMenu)/sizeof(rfScanSubMenu[0]);
    currentMenuLine = 0; currentSubMenuLevel = 1;
  } else if (currentSubMenuLevel == 1) { // Back to top group of protocol items
    insideSubMenu = false;
    loadMenuContext();
  } else { // Back out to main icon selector wheel
    currentMode = LOGO_CYCLE;
    drawProtocolLogo(activeProtocol);
  }
}

void updateMenuScreen() {
  u8g2.clearBuffer();
  u8g2.setFont(u8g2_font_profont10_mf); // Tiny text optimized for small OLED windows
  
  // Calculate rolling visibility viewport frame
  int startEntry = max(0, currentMenuLine - 2);
  int endEntry = min(maxMenuLines, startEntry + 4);
  
  int textY = 12;
  for (int i = startEntry; i < endEntry; i++) {
    if (i == currentMenuLine) {
      u8g2.drawBox(0, textY - 9, 128, 11); // Indicator frame selector bar
      u8g2.setDrawColor(0); // White text inside black selector bar
    } else {
      u8g2.setDrawColor(1);
    }
    u8g2.drawStr(4, textY, activeMenuSource[i]);
    textY += 13;
  }
  u8g2.setDrawColor(1); // Restore pixel defaults
  u8g2.sendBuffer();
}

// 7. CORE SETUP & WORK LOOPS
void setup() {
  u8g2.begin();
  bootTimer = millis();
  
  // Setup button duration assignments
  button.setLongPressIntervalMs(1000); // 1-second long-press to register menu choices
  button.attachClick(handleShortClick);
Use code with caution.
