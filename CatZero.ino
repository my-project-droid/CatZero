#include <Arduino.h>
#include <U8g2lib.h>
#include <OneButton.h>

// 1. HARDWARE CONFIGURATION
#define BUTTON_PIN 13
#define SCREEN_SDA 21
#define SCREEN_SCL 22

U8G2_SSD1306_128X64_NONAME_F_HW_I2C u8g2(U8G2_R0, /* reset= */ U8X8_PIN_NONE, SCREEN_SCL, SCREEN_SDA);
OneButton button(BUTTON_PIN, true);

// 2. STATE MACHINE
enum OperatingMode { BOOT_SCREEN, LOGO_CYCLE, MENU_DISPLAY, ACTION_RUNNING, BRUCEGOTCHI_VIEW, PROTOPIRATE_VIEW };
OperatingMode currentMode = BOOT_SCREEN;

enum ProtocolCategory { WIFI, BLUETOOTH, SUB_GHZ, INFRARED, EXTRAS };
ProtocolCategory activeProtocol = WIFI;

// Menu variables
int currentMenuLine = 0;
int maxMenuLines = 0;
bool insideSubMenu = false;
int currentSubMenuLevel = 0;

// 3. BRUCEGOTCHI GAME ENGINE STATE
struct CatPet {
  int level = 1;
  int xp = 0;
  int nextLevelXp = 100;
  int hunger = 50; // 0 to 100
  int happiness = 50;
  String mood = "Happy";
};
CatPet myCat;

// 4. PROTOPIRATE DETECTOR STATE
const char* carBrands[] = { "Kia", "Chrysler", "Jeep", "StarLine", "Scher-Khan", "Subaru", "Fiat", "Ford", "Suzuki", "Toyota" };
int selectedBrandIndex = 0;
float selectedRfFreq = 433.92;
bool isSniffingCar = false;

// 5. MENU ARRAYS
const char* wifiMenu[] = { "[ BACK ]", "Connect WiFi", "WiFi AP", "WiFi Atks", "Target Atks", "Karma Attack", "Beacon Spam", "Deauth Flood", "Evil Portal" };
const char* bleMenu[] = { "[ BACK ]", "Disconnect BLE", "Media Commands", "BLE Scan", "BadBLE", "BLE Spam Features", "Windows Spam", "Android Spam", "Samsung Spam", "Spam All", "Custom Spam", "iBeacon", "BLE Suite" };
const char* rfMenu[] = { "[ BACK ]", "Scan/Copy", "Record Raw", "Custom SubGhz", "Spectrum", "RSSI Spectrum", "Listen", "Bruceforce", "Jammer Intermittent", "Jammer Full", "RF Brute Force", "ProtoPirate", "Config" };
const char* irMenu[] = { "[ BACK ]", "TV-B-Gone", "Custom IR", "IR Read/replay", "IR Jammer", "IR Brute Force", "IR2Keyboard" };
const char* extrasMenu[] = { "[ BACK ]", "Timer", "Games", "Clock", "Calculator" };
const char* gamesSubMenu[] = { "[ BACK ]", "Snake", "Tetris", "Racing", "Magic 8 Ball", "Space Shooter", "Dino", "Tamagotchi Cat" };

const char** activeMenuSource;
unsigned long bootTimer = 0;

// 6. VISUAL RENDERING ASSETS
void drawCatFace(int x, int y, String emotion) {
  // Ears
  u8g2.drawTriangle(x, y+10, x+8, y, x+12, y+10);     
  u8g2.drawTriangle(x+24, y+10, x+28, y, x+36, y+10); 
  u8g2.drawFrame(x+4, y+10, 28, 16); // Face
  
  if (emotion == "Anxious") {
    // Slanted nervous eyes
    u8g2.drawLine(x+8, y+14, x+11, y+16);
    u8g2.drawLine(x+27, y+14, x+24, y+16);
  } else if (emotion == "Sleeping") {
    // Closed lines for sleeping
    u8g2.drawLine(x+8, y+15, x+12, y+15);
    u8g2.drawLine(x+24, y+15, x+28, y+15);
  } else {
    // Normal Eyes
    u8g2.drawBox(x+9, y+15, 3, 3);                      
    u8g2.drawBox(x+24, y+15, 3, 3);                     
  }
  // Nose & Whiskers
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

// 7. THE TWO NEW CORE INTERFACES
void renderBrucegotchiScreen() {
  u8g2.clearBuffer();
  // Animate mood state based on pet core values
  if (myCat.hunger > 70) myCat.mood = "Anxious";
  else if (myCat.happiness < 30) myCat.mood = "Sleeping";
  else myCat.mood = "Happy";

  drawCatFace(5, 10, myCat.mood);
  
  u8g2.setFont(u8g2_font_profont10_mf);
  u8g2.setCursor(48, 12);  u8g2.print("LVL: " + String(myCat.level));
  u8g2.setCursor(48, 24);  u8g2.print("XP: " + String(myCat.xp) + "/" + String(myCat.nextLevelXp));
  u8g2.setCursor(48, 36);  u8g2.print("HNG: " + String(myCat.hunger) + "%");
  u8g2.setCursor(48, 48);  u8g2.print("HAP: " + String(myCat.happiness) + "%");
  
  u8g2.drawFrame(48, 52, 75, 7);
  int xpBarWidth = map(myCat.xp, 0, myCat.nextLevelXp, 0, 73);
  u8g2.drawBox(49, 53, xpBarWidth, 5);

  u8g2.setFont(u8g2_font_profont9_mf);
  u8g2.drawStr(0, 62, "[Hold to Exit] [Click to Feed]");
  u8g2.sendBuffer();
}

void renderProtoPirateScreen() {
  u8g2.clearBuffer();
  u8g2.setFont(u8g2_font_profont12_mf);
  u8g2.drawStr(0, 12, "🏴‍☠️ PROTOPIRATE");
  
  u8g2.setFont(u8g2_font_profont10_mf);
  u8g2.drawStr(0, 26, "Target Brand:");
  u8g2.drawStr(10, 38, carBrands[selectedBrandIndex]);
  
  u8g2.setCursor(0, 50);
  u8g2.print("Freq: " + String(selectedRfFreq, 2) + " MHz");
  
  if (isSniffingCar) {
    u8g2.drawBox(90, 2, 35, 12);
    u8g2.setDrawColor(0);
    u8g2.drawStr(93, 11, "SNIFF");
    u8g2.setDrawColor(1);
    u8g2.drawStr(0, 62, ">> Listening for rolling code...");
  } else {
    u8g2.drawStr(0, 62, "[Click: Brand] [Hold: Sniff/Exit]");
  }
  u8g2.sendBuffer();
}

// 8. NAVIGATION AND LOGIC CONTROLLERS
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
  else if (currentMode == ACTION_RUNNING) {
    // Reward action execution with points
    myCat.xp += 15;
    myCat.hunger += 5;
    if (myCat.xp >= myCat.nextLevelXp) {
      myCat.level++;
      myCat.xp = 0;
    }
    currentMode = MENU_DISPLAY;
    updateMenuScreen();
  }
  else if (currentMode == BRUCEGOTCHI_VIEW) {
    // Interaction: Feed cat reduces hunger, yields happiness boosts
    if (myCat.hunger > 10) myCat.hunger -= 15;
    if (myCat.happiness < 95) myCat.happiness += 10;
    renderBrucegotchiScreen();
  }
  else if (currentMode == PROTOPIRATE_VIEW && !isSniffingCar) {
    // Step through the 10 available car brand configurations
    selectedBrandIndex = (selectedBrandIndex + 1) % 10;
    
    // Auto-switch standard regional channel spaces
    if (selectedBrandIndex % 3 == 0) selectedRfFreq = 315.00;
    else if (selectedBrandIndex % 3 == 1) selectedRfFreq = 433.92;
    else selectedRfFreq = 868.35;
    
    renderProtoPirateScreen();
  }
}

void handleLongPress() {
  if (currentMode == LOGO_CYCLE) {
    currentMode = MENU_DISPLAY;
    insideSubMenu = false;
    currentSubMenuLevel = 0;
    currentMenuLine = 0;
    loadMenuContext();
  } 
  else if (currentMode == MENU_DISPLAY) {
    String selection = String(activeMenuSource[currentMenuLine]);
    if (selection == "[ BACK ]") {
      goBackALevel();
    } else if (selection == "Tamagotchi Cat" || selection == "Brucegotchi") {
      currentMode = BRUCEGOTCHI_VIEW;
      renderBrucegotchiScreen();
    } else if (selection == "ProtoPirate") {
      currentMode = PROTOPIRATE_VIEW;
      isSniffingCar = false;
      renderProtoPirateScreen();
    } else {
      processMenuSelection(selection);
    }
  }
  else if (currentMode == BRUCEGOTCHI_VIEW) {
    currentMode = MENU_DISPLAY;
    updateMenuScreen();
  }
  else if (currentMode == PROTOPIRATE_VIEW) {
    if (!isSniffingCar) {
      isSniffingCar = true; // Lock radio module loop onto decoded channel profile
      renderProtoPirateScreen();
    } else {
      currentMode = MENU_DISPLAY; // Secondary hold exits back upwards to RF configuration lists
      updateMenuScreen();
    }
  }
}

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
  if (item == "Games") {
    activeMenuSource = gamesSubMenu; maxMenuLines = sizeof(gamesSubMenu)/sizeof(gamesSubMenu[0]);
    currentMenuLine = 0; insideSubMenu = true; currentSubMenuLevel = 1;
  } else {
    currentMode = ACTION_RUNNING;
    u8g2.clearBuffer();
    u8g2.setFont(u8g2_font_profont12_mf);
    u8g2.drawStr(0, 15, "RUNNING TEST:");
    u8g2.drawStr(0, 35, item.c_str());
    u8g2.drawStr(0, 58, "[Click to Stop]");
    u8g2.sendBuffer();
  }
  if (currentMode == MENU_DISPLAY) updateMenuScreen();
}

void goBackALevel() {
  if (currentSubMenuLevel == 1) {
    insideSubMenu = false;
    loadMenuContext();
  } else {
    currentMode = LOGO_CYCLE;
    drawProtocolLogo(activeProtocol);
  }
}

void updateMenuScreen() {
  u8g2.clearBuffer();
  u8g2.setFont(u8g2_font_profont10_mf);
  int startEntry = max(0, currentMenuLine - 2);
  int endEntry = min(maxMenuLines, startEntry + 4);
  int textY = 12;
  
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
Use code with caution.
