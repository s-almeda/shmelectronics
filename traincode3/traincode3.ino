// The code shm used on the Arduino UNO inside their "fleet of the future" BART train costume! 
//
// uses the MD_Parola library
//
// MD_MAX72XX library can be found at https://github.com/MajicDesigns/MD_MAX72XX
//

#include <MD_Parola.h>
#include <MD_MAX72xx.h>
#include <SPI.h>
#include <FastLED.h>

#define LED_PIN     A3
#define NUM_LEDS    4

CRGB leds[NUM_LEDS];

// Button
#define BUTTON_PIN  A0  // Button connected to analog pin A0
#define DEBOUNCE_DELAY 50  // 50 milliseconds debounce delay

#define BUTTON_PIN_2 A1  // Second button connected to analog pin A1
bool signOn = false;  // Variable to track whether the sign is on or off


// Define states for LED modes
enum LedMode { WHITE, BLUE, CYAN, MAGENTA, YELLOW, RAINBOW, DIM_WHITE, OFF };
LedMode currentMode = OFF;  // Start with LEDs off

unsigned long lastDebounceTime = 0;  // Timestamp for debouncing
int lastButtonState1 = HIGH;  // Last state of first button
int lastButtonState2 = HIGH;  // Last state of second button

// Define the number of devices in the chain and the hardware interface
#define HARDWARE_TYPE MD_MAX72XX::FC16_HW
#define MAX_DEVICES 4
#define CLK_PIN 13
#define DATA_PIN 11
#define CS_PIN 10  // Chip select for the single sign

// Initialize the Parola object
MD_Parola P = MD_Parola(HARDWARE_TYPE, DATA_PIN, CLK_PIN, CS_PIN, MAX_DEVICES);

#define PAUSE_TIME 7000
const uint8_t SCROLL_SPEED = 1;  // text speed

MD_MAX72XX::fontType_t trainfont[] PROGMEM = 
{
  0, 	// 0
	0, 	// 1
	0, 	// 2
	0, 	// 3
	0, 	// 4
	0, 	// 5
	0, 	// 6
	0, 	// 7
	0, 	// 8
	0, 	// 9
	0, 	// 10
	0, 	// 11
	0, 	// 12
	0, 	// 13
	0, 	// 14
	0, 	// 15
	0, 	// 16
	0, 	// 17
	0, 	// 18
	0, 	// 19
	0, 	// 20
	0, 	// 21
	0, 	// 22
	0, 	// 23
	0, 	// 24
	0, 	// 25
	0, 	// 26
	0, 	// 27
	0, 	// 28
	0, 	// 29
	0, 	// 30
	0, 	// 31
	1, 0, 	// 32
	0, 	// 33
	0, 	// 34
	0, 	// 35
	0, 	// 36
	0, 	// 37
	0, 	// 38
	0, 	// 39
	0, 	// 40
	0, 	// 41
	0, 	// 42
	0, 	// 43
	0, 	// 44
	0, 	// 45
	0, 	// 46
	4, 192, 48, 12, 3, 	// 47
	0, 	// 48
	0, 	// 49
	0, 	// 50
	0, 	// 51
	0, 	// 52
	0, 	// 53
	0, 	// 54
	0, 	// 55
	0, 	// 56
	0, 	// 57
	0, 	// 58
	0, 	// 59
	0, 	// 60
	0, 	// 61
	0, 	// 62
	0, 	// 63
	0, 	// 64
	0, 	// 65
	0, 	// 66
	0, 	// 67
	0, 	// 68
	0, 	// 69
	3, 255, 9, 9, 	// 70
	0, 	// 71
	0, 	// 72
	0, 	// 73
	0, 	// 74
	0, 	// 75
	3, 255, 128, 128, 	// 76 L
	5, 255, 1, 254, 1, 254, 	// 77
	0, 	// 78
	3, 126, 129, 126, 	// 79
	0, 	// 80
	0, 	// 81
	3, 255, 17, 238, 	// 82 ||| R
	3, 70, 137, 114, 	// 83
	0, 	// 84
	0, 	// 85
	0, 	// 86
	0, 	// 87
	0, 	// 88
	0, 	// 89
	0, 	// 90
	4, 255, 255, 255, 0, 	// 91
	8, 0, 0, 0, 0, 0, 0, 0, 0, 	// 92
	0, 	// 93
	0, 	// 94
	8, 0, 0, 0, 0, 0, 0, 0, 0, 	// 95
	0, 	// 96
	3, 232, 168, 248, 	// 97
	3, 255, 136, 240, 	// 98
	4, 112, 136, 136, 0, 	// 99
	3, 112, 136, 255, 	// 100 d
	3, 248, 168, 176, 	// 101 e
	0, 	// 102
	0, 	// 103
	3, 255, 8, 240, 	// 104
	2, 250, 0, 	// 105
	0, 	// 106
	0, 	// 107
	2, 255, 0, 	// 108
	5, 248, 8, 240, 8, 240, 	// 109
	3, 248, 8, 240, 	// 110
	3, 112, 136, 112, 	// 111
	0, 	// 112
	0, 	// 113
	3, 248, 8, 16, 	// 114
	0, 	// 115
	0, 	// 116
	0, 	// 117
	0, 	// 118
	0, 	// 119
	0, 	// 120
	0, 	// 121
	0, 	// 122
	0, 	// 123
	0, 	// 124
	0, 	// 125
	0, 	// 126
	0, 	// 127
	0, 	// 128
	0, 	// 129
	0, 	// 130
	0, 	// 131
	0, 	// 132
	0, 	// 133
	0, 	// 134
	0, 	// 135
	0, 	// 136
	0, 	// 137
	0, 	// 138
	0, 	// 139
	0, 	// 140
	0, 	// 141
	0, 	// 142
	0, 	// 143
	0, 	// 144
	0, 	// 145
	0, 	// 146
	0, 	// 147
	0, 	// 148
	0, 	// 149
	0, 	// 150
	0, 	// 151
	0, 	// 152
	0, 	// 153
	0, 	// 154
	0, 	// 155
	0, 	// 156
	0, 	// 157
	0, 	// 158
	0, 	// 159
	0, 	// 160
	0, 	// 161
	0, 	// 162
	0, 	// 163
	0, 	// 164
	0, 	// 165
	0, 	// 166
	0, 	// 167
	0, 	// 168
	0, 	// 169
	0, 	// 170
	0, 	// 171
	0, 	// 172
	0, 	// 173
	0, 	// 174
	0, 	// 175
	0, 	// 176
	0, 	// 177
	0, 	// 178
	0, 	// 179
	0, 	// 180
	0, 	// 181
	0, 	// 182
	0, 	// 183
	0, 	// 184
	0, 	// 185
	0, 	// 186
	0, 	// 187
	0, 	// 188
	0, 	// 189
	0, 	// 190
	0, 	// 191
	0, 	// 192
	0, 	// 193
	0, 	// 194
	0, 	// 195
	0, 	// 196
	0, 	// 197
	0, 	// 198
	0, 	// 199
	0, 	// 200
	0, 	// 201
	0, 	// 202
	0, 	// 203
	0, 	// 204
	0, 	// 205
	0, 	// 206
	0, 	// 207
	0, 	// 208
	0, 	// 209
	0, 	// 210
	0, 	// 211
	0, 	// 212
	0, 	// 213
	0, 	// 214
	0, 	// 215
	0, 	// 216
	0, 	// 217
	0, 	// 218
	0, 	// 219
	0, 	// 220
	0, 	// 221
	0, 	// 222
	0, 	// 223
	0, 	// 224
	0, 	// 225
	0, 	// 226
	0, 	// 227
	0, 	// 228
	0, 	// 229
	0, 	// 230
	0, 	// 231
	0, 	// 232
	0, 	// 233
	0, 	// 234
	0, 	// 235
	0, 	// 236
	0, 	// 237
	0, 	// 238
	0, 	// 239
	0, 	// 240
	0, 	// 241
	0, 	// 242
	0, 	// 243
	0, 	// 244
	0, 	// 245
	0, 	// 246
	0, 	// 247
	0, 	// 248
	0, 	// 249
	0, 	// 250
	0, 	// 251
	0, 	// 252
	0, 	// 253
	0, 	// 254
	0, 	// 255
};

// Turn on debug statements to the serial output
#define DEBUG 0

#if DEBUG
#define PRINT(s, x) \
  { \
    Serial.print(F(s)); \
    Serial.print(x); \
  }
#define PRINTS(x) Serial.print(F(x))
#define PRINTX(x) Serial.println(x, HEX)
#else
#define PRINT(s, x)
#define PRINTS(x)
#define PRINTX(x)
#endif

const uint8_t Sq[] = { 8, 255, 255, 255, 255, 255, 255, 255, 255 };  // Square

// Global variables
typedef struct
{
  const char *msg;  // message to display
} msgDef_t;

msgDef_t M[] = {
  { "[ Shm  Line" },
  { "[ Richmond" },
  { "[  S F O "},
};
#define MAX_STRINGS (sizeof(M) / sizeof(M[0]))

void setup(void) {
  FastLED.addLeds<WS2812, LED_PIN, GRB>(leds, NUM_LEDS);
  pinMode(BUTTON_PIN, INPUT_PULLUP);  // Enable internal pull-up resistor for button 1
  pinMode(BUTTON_PIN_2, INPUT_PULLUP);  // Enable internal pull-up resistor for button 2

  Serial.begin(57600);

  // Initialize the sign
  P.begin();
  P.setIntensity(0);
  P.setCharSpacing(0);
  P.setFont(trainfont);  // Custom font for the sign
  
  // Set initial text for the sign
  P.displayText(M[0].msg, PA_RIGHT, SCROLL_SPEED, PAUSE_TIME, PA_SCROLL_RIGHT, PA_SCROLL_RIGHT);
  // Apply the flip effects to the zone
  P.setZoneEffect(0, true, PA_FLIP_LR);  // Flip left-right
  P.setZoneEffect(0, true, PA_FLIP_UD);  // Flip upside-down
}

void loop(void) {
  // Handle button press and LED mode toggle
  int reading = digitalRead(BUTTON_PIN);  // Read the button state
  if (reading == LOW) {  // Button pressed (reads low with INPUT_PULLUP)
    if ((millis() - lastDebounceTime) > DEBOUNCE_DELAY) {
      if (lastButtonState1 == HIGH) {  // Button just pressed
        currentMode = static_cast<LedMode>((currentMode + 1) % 7);  // Cycle through modes
        lastDebounceTime = millis();  // Reset debounce timer
      }
    }
    lastButtonState1 = LOW;  // Button is pressed
  } else {
    lastButtonState1 = HIGH;  // Button is released
  }

  // Handle button press for turning the signs on/off
  int reading2 = digitalRead(BUTTON_PIN_2);  // Read the second button state (BUTTON_PIN_2 should be defined)
  static int lastButtonState2 = HIGH;  // State of second button
  if (reading2 == LOW) {  // Button pressed (reads low with INPUT_PULLUP)
    if ((millis() - lastDebounceTime) > DEBOUNCE_DELAY) {
      if (lastButtonState2 == HIGH) {  // Button just pressed
        signOn = !signOn;  // Toggle the Parola signs on/off
        if (!signOn) {
          P.displayClear();  // Clear the first sign
          //P2.displayClear();  // Clear the second sign
        }
        lastDebounceTime = millis();  // Reset debounce timer
      }
    }
    lastButtonState2 = LOW;  // Button is pressed
  } else {
    lastButtonState2 = HIGH;  // Button is released
  }

    // Only animate the sign if signOn is true
  if (signOn) {
    static uint8_t n = 1;
    if (P.displayAnimate()) {
      P.setTextBuffer(M[n].msg);
      P.displayReset();
      n = (n + 1) % MAX_STRINGS;
    }
  }

  // Control LEDs based on the current mode
  switch (currentMode) {
    case WHITE:
      setSolidColor(CRGB::White);
      break;
    case BLUE:
      setSolidColor(CRGB::Blue);
      break;
    case CYAN:
      setSolidColor(CRGB::Cyan);
      break;
    case MAGENTA:
      setSolidColor(CRGB::Magenta);
      break;
    case YELLOW:
      setSolidColor(CRGB::Yellow);
      break;
    case RAINBOW:
      rainbowMode();
      break;
    case OFF:
      setSolidColor(CRGB::Black);  // Turn off LEDs
      break;
    default:
      // Default case: turn off LEDs if the mode is unrecognized
      setSolidColor(CRGB::Black);  
      break;
  }
    // case DIM_WHITE:
    //   CRGB dimWhite = CRGB::White;  // Create a copy of CRGB::White
    //   dimWhite.nscale8(50);  // Scale the brightness to 50%
    //   setSolidColor(dimWhite);  // Set the dimmed color
    //   break;
  //}

  FastLED.show();  // Update the LED state
  delay(40);  // Slight delay for smooth transitions
}

void setSolidColor(CRGB color) {
  for (int i = 0; i < NUM_LEDS; i++) {
    leds[i] = color;
  }
}

void rainbowMode() {
  static uint8_t hue = 0;  // Start at hue 0 (red)
  
  // Set all LEDs to the same hue, which will change over time
  fill_solid(leds, NUM_LEDS, CHSV(hue, 255, 255));  // All LEDs same color

  hue++;  // Increment hue for smooth color transition across the rainbow
}
