#include <Arduino.h>
#include <U8g2lib.h>
#include <OneButton.h>

// 1. HARDWARE PINS
#define BUTTON_PIN 13
#define SCREEN_SDA 21
#define SCREEN_SCL 22

U8G2_SSD1306_128X64_NONAME_F_HW_I2C u8g2(U8G2_R0, /* reset= */ U8X8_PIN_NONE, SCREEN_SCL, SCREEN_SDA);
OneButton button(BUTTON_PIN, true);

// 2. ADVANCED PROGRESSION ENGINE (9 Ranks)
struct ProgressionSystem {
  int level = 1;
  int xp = 0;
  int nextLevelXp = 100;
  int rankIndex = 0;
  unsigned long sessionStartTime = 0;
  
  const char* rankNames[9] = {
    "Kitten", "Scout", "Sniffer", "Prowler", 
    "Phreaker", "Operator", "Ghost", "Specter", "Nyan-Lord"
  };
};
ProgressionSystem catOS;

// 3. STATE MACHINE EXTENSIONS
enum OperatingMode { BOOT_SCREEN, LOGO_CYCLE, MENU_DISPLAY, ACTION_RUNNING, BRUCEGOTCHI_VIEW, PROTOPIRATE_VIEW, ANALYZER_VIEW, LOCATE_VIEW };
OperatingMode currentMode = BOOT_SCREEN;

enum ProtocolCategory { WIFI, BLUETOOTH, SUB_GHZ, INFRARED, EXTRAS };
ProtocolCategory activeProtocol = WIFI;

int currentMenuLine = 0;
int maxMenuLines = 0;
bool insideSubMenu = false;
int currentSubMenuLevel = 0;

// 4. THE COMPREHENSIVE TACTICAL MENU ARRAYS
const char* wifiMenu[] = { 
  "[ BACK ]", "Channel Analyzer", "Camera Detector", "Camera Deauther", 
  "Pineapple Detector", "Pwnagotchi Detector", "Pwnagotchi Spam", 
  "Drone Detector", "Flock Detector", "Device Scout", "LAP Sniffing", "Kismet" 
};

const char* bleMenu[] = { 
  "[ BACK ]", "BLE Inspector", "nyanBOX Detector", "Flipper Scanner", 
  "Axon Detector", "Meshtastic Detector", "MeshCore Detector", "Skimmer Detector", 
  "AirTag Detector", "AirTag Spoofer", "SmartTag Detector", "Tile Detector", 
  "RayBan Detector", "iBeacon Detector", "BLE Spammer", "Swift Pair", 
  "Sour Apple", "Sour Droid", "BLE Spoofer" 
};

const char* rfMenu[] = { 
  "[ BACK ]", "Scan/Copy", "Record Raw", "Custom SubGhz", "Spectrum", 
  "RSSI Spectrum", "RollJam Attack", "Signal Lock", "Listen", 
  "Bruceforce", "Jammer Intermittent", "Jammer Full", "RF Brute Force", "ProtoPirate" 
};

const char* irMenu[] = { 
  "[ BACK ]", "TV-B-Gone", "Custom IR", "IR Read/replay", "IR Jammer", "IR Brute Force", "IR2Keyboard" 
};

const char* extrasMenu[] = { 
  "[ BACK ]", "Device Networking", "Brucegotchi", "Timer", "Games", "Clock", "Calculator" 
};

const char** activeMenuSource;
unsigned long bootTimer = 0;

// Target tracking variables for locator tools
String targetItemName = "";
int targetRssiSignal = -75;

// 5. XP & PROGRESSION TRACKING CONTROLLER
void awardXP(int amount, String sourceActivity) {
  catOS.xp += amount;
  
  // Handle Level Up Logic up to Level 99
  if (catOS.xp >= catOS.nextLevelXp && catOS.level < 99) {
    catOS.level++;
    catOS.xp = 0;
    catOS.nextLevelXp = catOS.level * 120; // Escalating difficulty curve
    
    // Progress through the 9 specialized ranks based on milestone milestones
    if (catOS.level >= 90) catOS.rankIndex = 8;
    else if (catOS.level >= 75) catOS.rankIndex = 7;
    else if (catOS.level >= 60) catOS.rankIndex = 6;
    else if (catOS.level >= 45) catOS.rankIndex = 5;
    else if (catOS.level >= 30) catOS.rankIndex = 4;
    else if (catOS.level >= 20) catOS.rankIndex = 3;
    else if (catOS.level >= 10) catOS.rankIndex = 2;
    else if (catOS.level >= 5)  catOS.rankIndex = 1;
    else catOS.rankIndex = 0;
    
    // Place your Preferences.h EEPROM persistence save calls here!
  }
}

// 6. SPECIALIZED VIEW DRAWING FUNCTIONS
void drawChannelAnalyzer() {
  u8g2.clearBuffer();
  u8g2.setFont(u8g2_font_profont10_mf);
  u8g2.drawStr(0, 10, "CH ANALYZER (WiFi)");
  
  // Draw simulated real-time bar charts for channels 1 through 13
  for (int i = 0; i < 13; i++) {
    int x = 4 + (i * 9);
    int barHeight = random(5, 45); // Will be tied to real Wi-Fi scan results later
    u8g2.drawBox(x, 55 - barHeight, 6, barHeight);
    
    // Draw numbers for key channels
    if (i == 0 || i == 5 || i == 10) {
      u8g2.setCursor(x, 64);
      u8g2.print(String(i+1));
    }
  }
  u8g2.sendBuffer();
}

void drawSignalLocatingRadar() {
  u8g2.clearBuffer();
  u8g2.setFont(u8g2_font_profont12_mf);
  u8g2.drawStr(0, 12, "🎯 LOCATE MODE");
  u8g2.setFont(u8g2_font_profont10_mf);
  u8g2.drawStr(0, 26, targetItemName.c_str());
  
  // Fluctuating RSSI signal simulation
  targetRssiSignal = targetRssiSignal + random(-3, 4);
  if (targetRssiSignal > -30) targetRssiSignal = -30;
  if (targetRssiSignal < -95) targetRssiSignal = -95;
  
  u8g2.setCursor(0, 42);
  u8g2.print("RSSI: " + String(targetRssiSignal) + " dBm");
  
  // Graphical signal progress bar
  int fillWidth = map(targetRssiSignal, -95, -30, 0, 124);
  u8g2.drawFrame(0, 46, 128, 8);
  u8g2.drawBox(2, 48, fillWidth, 4);
  
  u8g2.drawStr(0, 62, "[Click to Stop Sniffing]");
  u8g2.sendBuffer();
}

void drawCatFace(int x, int y, String emotion) {
  u8g2.drawTriangle(x, y+10, x+8, y, x+12, y+10);     
  u8g2.drawTriangle(x+24, y+10, x+28, y, x+36, y+10); 
  u8g2.drawFrame(x+4, y+10, 28, 16); 
  u8g2.drawBox(x+9, y+15, 3, 3);                      
  u8g2.drawBox(x+24, y+15, 3, 3);                     
  u8g2.drawTriangle(x+16, y+20, x+14, y+18, x+18, y+18); 
  u8g2.drawLine(x, y+18, x+4, y+18);
  u8g2.drawLine(x+32, y+18, x+36, y+18);
}

void drawProtocolLogo(ProtocolCategory proto) {
  u8g2.clearBuffer();
  u8g2.setFont(u8g2_font_open_iconic_all_4x_t);
  switch(proto) {
    case WIFI: u8g2.drawGlyph(48, 40, 247); u8g2.setFont(u8g2_font_profont12_mf); u8g2.drawStr(40, 56, "[ WiFi ]"); break;
    case BLUETOOTH: u8g2.drawGlyph(48, 40, 212); u8g2.setFont(u8g2_font_profont12_mf); u8g2.drawStr(24, 56, "[ Bluetooth ]"); break;
    case SUB_GHZ: u8g2.drawGlyph(48, 40, 89);  u8g2.setFont(u8g2_font_profont12_mf); u8g2.drawStr(32, 56, "[ Sub-GHz ]"); break;
    case INFRARED: u8g2.drawGlyph(48, 40, 119); u8g2.setFont(u8g2_font_profont12_mf); u8g2.drawStr(32, 56, "[ Infrared ]"); break;
    case EXTRAS: u8g2.drawGlyph(48, 40, 172); u8g2.setFont(u8g2_font_profont12_mf); u8g2.drawStr(40, 56, "[ Extras ]"); break;
  }
  u8g2.sendBuffer();
}

// 7. INPUT NAVIGATION INTERRUPT ENGINE
void handleShortClick() {
  if (currentMode == LOGO_CYCLE) {
    activeProtocol = static_cast<ProtocolCategory>((activeProtocol + 1) % 5);
    drawProtocolLogo(activeProtocol);
  } 
  else if (currentMode == MENU_DISPLAY) {
    currentMenuLine++;
    if (currentMenuLine >= maxMenuLines) currentMenuLine = 0;
    updateMenuScreen();
  }
  else if (currentMode == ACTION_RUNNING || currentMode == ANALYZER_VIEW || currentMode == LOCATE_VIEW) {
    // Session length calculator bonus tracking
    unsigned long sessionDuration = (millis() - catOS.sessionStartTime) / 1000;
    int bonusXp = min(20, (int)sessionDuration / 5); // Gain 1 extra XP per 5 seconds active
    
    awardXP(10 + bonusXp, "Attack Stop");
    currentMode = MENU_DISPLAY;
    updateMenuScreen();
  }
}

void handleLongPress() {
  if (currentMode == LOGO_CYCLE) {
    currentMode = MENU_DISPLAY;
    currentMenuLine = 0;
    loadMenuContext();
  } 
  else if (currentMode == MENU_DISPLAY) {
    String selection = String(activeMenuSource[currentMenuLine]);
    
    if (selection == "[ BACK ]") {
      currentMode = LOGO_CYCLE;
      drawProtocolLogo(activeProtocol);
    } 
    else if (selection == "Channel Analyzer") {
      currentMode = ANALYZER_VIEW;
      catOS.sessionStartTime = millis();
    }
    else if (selection.endsWith("Detector") || selection.endsWith("Scanner") || selection == "Device Scout") {
      // Divert tracking modes into radar view tools
      currentMode = LOCATE_VIEW;
      targetItemName = selection;
      targetRssiSignal = -80;
      catOS.sessionStartTime = millis();
    }
    else {
      // Standard utility execution view
      currentMode = ACTION_RUNNING;
      catOS.sessionStartTime = millis();
      u8g2.clearBuffer();
      u8g2.setFont(u8g2_font_profont12_mf);
      u8g2.drawStr(0, 15, "TACTICAL OP:");
      u8g2.drawStr(0, 35, selection.c_str());
      u8g2.drawStr(0, 58, "[Click to Terminate]");
      u8g2.sendBuffer();
    }
  }
}

void loadMenuContext() {
  switch(activeProtocol) {
    case WIFI:      activeMenuSource = wifiMenu;      maxMenuLines = sizeof(wifiMenu)/sizeof(wifiMenu[0]); break;
    case BLUETOOTH: activeMenuSource = bleMenu;       maxMenuLines = sizeof(bleMenu)/sizeof(bleMenu[0]); break;
    case SUB_GHZ:   activeMenuSource = rfMenu;        maxMenuLines = sizeof(rfMenu)/sizeof(rfMenu[0]); break;
    case INFRARED:  activeMenuSource = irMenu;        maxMenuLines = sizeof(irMenu)/sizeof(irMenu[0]); break;
    case EXTRAS:    activeMenuSource = extrasMenu;    maxMenuLines = sizeof(extrasMenu)/sizeof(extrasMenu[0]); break;
  }
  updateMenuScreen();
}

void updateMenuScreen() {
  u8g2.clearBuffer();
  
  // DRAW HEADLINES: Displays Level, Progress Metrics, and Rank
  u8g2.setFont(u8g2_font_profont9_mf);
  u8g2.setCursor(0, 8);
  u8g2.print("L" + String(catOS.level) + " [" + String(catOS.rankNames[catOS.rankIndex]) + "]");
  u8g2.drawFrame(78, 2, 50, 6);
  int xpProgress = map(catOS.xp, 0, catOS.nextLevelXp, 0, 48);
  u8g2.drawBox(79, 3, xpProgress, 4);

  // SCROLLING MENU VIEWPORT CONTAINER
  u8g2.setFont(u8g2_font_profont10_mf);
  int startEntry = max(0, currentMenuLine - 1);
  int endEntry = min(maxMenuLines, startEntry + 4);
  int textY = 23;
  
  for (int i = startEntry; i < endEntry; i++) {
    if (i == currentMenuLine) {
      u8g2.drawBox(0, textY - 9, 128, 11);
      u8g2.setDrawColor(0);
    } else {
      u8g2.setDrawColor(1);
    }
    u8g2.drawStr(4, textY, activeMenuSource[i]);
    textY += 13;
  }
  u8g2.setDrawColor(1);
  u8g2.sendBuffer();
}

void setup() {
  u8g2.begin();
  bootTimer = millis();
  button.setLongPressIntervalMs(1000);
  button.attachClick(handleShortClick);
  button.attachLongPressStop(handleLongPress);
}

void loop() {
  button.tick();
  
  // Real-time animation updates for tracking components
  if (currentMode == ANALYZER_VIEW && millis() % 200 == 0) {
    drawChannelAnalyzer();
  }
