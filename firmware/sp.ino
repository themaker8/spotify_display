#include <Arduino.h>
#include <ArduinoJson.h>
#include <Adafruit_GFX.h>
#include <Adafruit_ILI9341.h>
#include <WiFi.h>
#include <SpotifyEsp32.h>
#include <SPI.h>

#define TFT_CS   1
#define TFT_RST  2
#define TFT_DC   3
#define TFT_SCLK 4
#define TFT_MOSI 5

#define BTN_PREV 6
#define BTN_PLAY 7
#define BTN_NEXT 8

char* SSID            = "YOUR WIFI SSID";
const char* PASSWORD  = "YOUR WIFI PASSWORD";
const char* CLIENT_ID     = "YOUR CLIENT ID";
const char* CLIENT_SECRET = "YOUR CLIENT SECRET";

String lastArtist;
String lastTrackname;
bool   lastIsPlaying = true;

Adafruit_ILI9341 tft = Adafruit_ILI9341(TFT_CS, TFT_DC, TFT_MOSI, TFT_SCLK, TFT_RST);
Spotify sp(CLIENT_ID, CLIENT_SECRET);

// ─── Progress Bar ─────────────────────────────────────
// Screen is 320×240 in landscape
void drawProgressBar(long progress, long duration) {
  if (duration == 0) return;

  const int barX = 10;
  const int barY = 200;   // near bottom of 240px tall screen
  const int barW = 300;   // wide bar on the bigger screen
  const int barH = 12;

  tft.drawRect(barX, barY, barW, barH, ILI9341_WHITE);

  int filled = (int)((float)progress / duration * barW);
  tft.fillRect(barX,          barY, filled,        barH, ILI9341_GREEN);
  tft.fillRect(barX + filled, barY, barW - filled, barH, ILI9341_BLACK);
}

// ─── Time Remaining ───────────────────────────────────
void drawTimeRemaining(long progress, long duration) {
  long remaining = (duration - progress) / 1000;
  int mins = remaining / 60;
  int secs = remaining % 60;

  char timeStr[8];
  sprintf(timeStr, "-%d:%02d", mins, secs);

  tft.fillRect(220, 178, 90, 18, ILI9341_BLACK);
  tft.setTextSize(1);
  tft.setTextColor(ILI9341_WHITE, ILI9341_BLACK);
  tft.setCursor(222, 180);
  tft.write(timeStr);
}

// ─── Play/Pause Indicator ─────────────────────────────
void drawPlayState(bool isPlaying) {
  tft.fillRect(10, 178, 80, 18, ILI9341_BLACK);
  tft.setTextSize(1);
  tft.setCursor(10, 180);
  if (isPlaying) {
    tft.setTextColor(ILI9341_GREEN, ILI9341_BLACK);
    tft.write("PLAYING");
  } else {
    tft.setTextColor(ILI9341_RED, ILI9341_BLACK);
    tft.write("PAUSED");
  }
}

// ─── Boot Splash ──────────────────────────────────────
void showSplash() {
  tft.fillScreen(ILI9341_BLACK);

  // Big Spotify-green title
  tft.setTextColor(ILI9341_GREEN);
  tft.setTextSize(3);
  tft.setCursor(60, 60);
  tft.write("Spotify");
  tft.setCursor(65, 100);
  tft.write("Display");

  // Your name underneath
  tft.setTextSize(2);
  tft.setTextColor(ILI9341_WHITE);
  tft.setCursor(90, 155);
  tft.write("by YOUR NAME");   // ← put your name

  delay(2500);
  tft.fillScreen(ILI9341_BLACK);
}

// ─── Setup ────────────────────────────────────────────
void setup() {
  Serial.begin(115200);

  pinMode(BTN_PREV, INPUT_PULLUP);
  pinMode(BTN_PLAY, INPUT_PULLUP);
  pinMode(BTN_NEXT, INPUT_PULLUP);

  tft.begin();
  tft.setRotation(1);        // landscape: 320 wide × 240 tall
  tft.fillScreen(ILI9341_BLACK);
  Serial.println("TFT Initialized!");

  showSplash();

  // WiFi connecting screen
  tft.setTextColor(ILI9341_WHITE);
  tft.setTextSize(2);
  tft.setCursor(40, 100);
  tft.write("Connecting WiFi");

  WiFi.begin(SSID, PASSWORD);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.print(".");
  }
  Serial.println("\nConnected!");

  tft.fillScreen(ILI9341_BLACK);
  tft.setTextSize(1);
  tft.setTextColor(ILI9341_GREEN);
  tft.setCursor(10, 10);
  tft.write(WiFi.localIP().toString().c_str());

  sp.begin();
  while (!sp.is_auth()) {
    sp.handle_client();
  }
  Serial.println("Authenticated");

  tft.fillScreen(ILI9341_BLACK);
}

// ─── Loop ─────────────────────────────────────────────
void loop() {

  // ── Buttons ──
  if (digitalRead(BTN_PREV) == LOW) {
    sp.previous();
    delay(300);
  }
  if (digitalRead(BTN_PLAY) == LOW) {
    sp.start_resume_playback();
    delay(300);
  }
  if (digitalRead(BTN_NEXT) == LOW) {
    sp.skip();
    delay(300);
  }

  // ── Fetch ──
  String currentArtist    = sp.current_artist_names();
  String currentTrackname = sp.current_track_name();
  long   progress         = sp.current_track_progress_ms();
  long   duration         = sp.current_track_duration_ms();
  bool   isPlaying        = sp.is_playing();

  // ── Artist changed → redraw everything ──
  if (lastArtist != currentArtist &&
      currentArtist != "Something went wrong" &&
      !currentArtist.isEmpty()) {

    tft.fillScreen(ILI9341_BLACK);
    lastArtist = currentArtist;

    tft.setTextSize(2);
    tft.setTextColor(ILI9341_CYAN, ILI9341_BLACK);
    tft.setCursor(10, 20);
    tft.write(lastArtist.c_str());

    // Rewrite track after screen clear
    tft.setTextSize(2);
    tft.setTextColor(ILI9341_WHITE, ILI9341_BLACK);
    tft.setCursor(10, 80);
    tft.write(lastTrackname.c_str());

    Serial.println("Artist: " + lastArtist);
  }

  // ── Track changed ──
  if (lastTrackname != currentTrackname &&
      currentTrackname != "Something went wrong" &&
      currentTrackname != "null") {

    lastTrackname = currentTrackname;

    tft.fillRect(0, 70, 320, 50, ILI9341_BLACK);
    tft.setTextSize(2);
    tft.setTextColor(ILI9341_WHITE, ILI9341_BLACK);
    tft.setCursor(10, 80);
    tft.write(lastTrackname.c_str());

    Serial.println("Track: " + lastTrackname);
  }

  // ── Play state changed ──
  if (isPlaying != lastIsPlaying) {
    lastIsPlaying = isPlaying;
    drawPlayState(isPlaying);
  }

  // ── Always update progress ──
  drawProgressBar(progress, duration);
  drawTimeRemaining(progress, duration);

  delay(2000);
}