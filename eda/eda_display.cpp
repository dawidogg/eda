/* ESPRESSIF DISTANT ALARM                                                      *
 * by Denis Davidoglu, 2023                                                     *
 *                                                                              *
 * Module for controlling TFT display. The heaviest of all, since I included    *
 * simple animations and an algorithm for word wrapping and aligning to the     *
 * middle of screen.                                                            */

#include "include/eda_display.h"

static const int BLACK = 0x0000;
static const int WHITE = 0xFFFF;
static const int PINK  = 0xE81F;
static const int MOVE_DELAY = 20;
static const int LINE_THICKNESS = 2;
static const int LINE_NUMBER = 40;

enum Direction {LEFT = -1, RIGHT = 1, NONE = 0};
static enum Direction macroDirection[64];

static struct Line {
    int x, y, length;
    uint16_t color;
    enum Direction direction;
} lines[LINE_NUMBER*2];

static TaskHandle_t trigger = NULL;
static Adafruit_ST7735 tft(OLED_CS, OLED_RS, OLED_RESET);
static GFXcanvas1 messageCanvas(160, 128);
static String notification;
static int duration;
static bool draw_lines;

static void _loop(void*);
static void lineSetup();
static void lineMove(Line*, enum Direction);
static inline int xWrap(int);
static void messageCanvasSetup(String);

///////////////////////////////////////////////////////////////////////////////

void eda_display_init() {
    tft.initR(INITR_BLACKTAB);
    tft.setRotation(3);
    messageCanvas.setFont(&eda_font);
    messageCanvas.setTextSize(2);
    messageCanvas.setTextColor(WHITE);
    messageCanvas.setTextWrap(false);

    for (int i = 0; i < 64; i++)
        macroDirection[i] = (i%2)? LEFT : RIGHT;

    xTaskCreate(_loop, "DISPLAY", 4096, NULL, 1, &trigger);
}

void eda_display_off() {
    analogWrite(OLED_LIGHT, 0);
}

void eda_display_clear() {
    tft.fillScreen(ST77XX_BLACK);
}

void eda_display_print(String s, bool _draw_lines) {
    notification = s;
    draw_lines = _draw_lines;
    xTaskNotifyGive(trigger);
}

static void _loop(void *params) { while (1) {
    ulTaskNotifyTake(pdTRUE, portMAX_DELAY);
    eda_stopFlag = false;
    String toPrint = eda_font_encoder_process(notification);
    
    if (draw_lines) lineSetup();
    messageCanvasSetup(toPrint);
    tft.drawBitmap(0, 0, messageCanvas.getBuffer(), 
    messageCanvas.width(), messageCanvas.height(), WHITE, BLACK);

    for (int i = 1; i < 256; i += 1) {
        analogWrite(OLED_LIGHT, i);
        vTaskDelay(1/portTICK_PERIOD_MS);
    }

    while (!eda_stopFlag) {
        for (int i = 0; draw_lines && i < LINE_NUMBER; i++)
            lineMove(&lines[i], lines[i].direction);
        vTaskDelay(MOVE_DELAY/portTICK_PERIOD_MS);
    }
    eda_display_off();
    eda_display_clear();
}}

static void lineSetup() {
    for (int i = 0; i < LINE_NUMBER; i++) {
        int temp_y = esp_random()%64;
        lines[i].x = esp_random()%160;
        lines[i].y = 2*temp_y;
        lines[i].length = (esp_random()%15+4);
        lines[i].color = PINK;
        lines[i].direction = macroDirection[temp_y];
    }
}

static void lineMove(Line *line, enum Direction dir) {
    for (int i = 0; i < LINE_THICKNESS; i++) {
        if (dir == LEFT) {
            if (messageCanvas.getPixel(xWrap(line->x-1), line->y+i) == BLACK)
                tft.drawPixel(xWrap(line->x-1), line->y+i, line->color);
            if (messageCanvas.getPixel(xWrap(line->x+line->length), line->y+i) == BLACK)
                tft.drawPixel(xWrap(line->x+line->length), line->y+i, BLACK);
        }
        if (dir == RIGHT) {
            if (messageCanvas.getPixel(xWrap(line->x+line->length+1), line->y+i) == BLACK)
                tft.drawPixel(xWrap(line->x+line->length+1), line->y+i, line->color);
            if (messageCanvas.getPixel(xWrap(line->x), line->y+i) == BLACK)
                tft.drawPixel(xWrap(line->x), line->y+i, BLACK);
        }
    }
    line->x += dir;
    if (line->x < -320 || line->x > 320) line->x %= 160;
}

static inline int xWrap(int x) {
    x %= 160;
    if (x < 0) x += 160;
    x %= 160;
    return x;
}

static void messageCanvasSetup(String encodedNotification) {
    encodedNotification += " ";
    int partCount = 0;
    int lastSpace = -1, spaceLeft = 10;
    String parts[5];

    messageCanvas.fillScreen(BLACK);

    for (int i = 0; i < encodedNotification.length(); i++) {
        if (encodedNotification.charAt(i) == ' ') {
            Serial.print("Space detected at ");
            Serial.println(i);
            int currentWordLength = i - lastSpace - 1;
            if (spaceLeft >= currentWordLength) {
                Serial.print("There is space to fit '");
                Serial.print(spaceLeft);
                Serial.println("' in current row");
                spaceLeft -= currentWordLength + 1; // one for space
                parts[partCount] += encodedNotification.substring(lastSpace+1, i);
                parts[partCount] += " ";
                lastSpace = i;
            } else {
                Serial.print("There is NO space to fit '");
                Serial.print(spaceLeft);
                Serial.println("' in current row");
                
                if (currentWordLength <= 10) {
                    i = i-1;
                } else {
                    parts[partCount] += encodedNotification.substring(lastSpace+1, lastSpace+spaceLeft+1);
                    parts[partCount] += " ";
                    lastSpace = lastSpace+spaceLeft;
                    i = lastSpace;
                }
                spaceLeft = 0;
            }
            if (spaceLeft <= 0) {
                spaceLeft = 10;
                partCount++;
            }
        }
    }

    while (partCount > 0 && parts[partCount].length() == 0) partCount--;
    Serial.print("Part count: ");
    Serial.println(partCount);
    Serial.println("Parts:");

    for (int i = 0; i <= partCount; i++) {
        Serial.println(parts[i]);
        int x = 79 - 8*(parts[i].length()-1); 
        int y = (125-(partCount+1)*25)/2 + 25*(i);
        messageCanvas.setCursor(x, y);
        messageCanvas.print(parts[i]);
    }
}

void eda_display_introLines() {
    tft.drawLine(80, 112, 80, 0, PINK);
}

static void drawPie(int x_origin, int y_origin, int x_start, int y_start, 
                    double theta, int radius, uint16_t color) {
    double phi = acos((double)(x_start-x_origin)/(double)radius);
    Serial.println(String(x_start) + " " + String(radius) + " " + String(phi)); 
    int x_final = x_origin + (double)radius*cos(phi+theta);
    int y_final = y_origin - (double)radius*sin(phi+theta);
    tft.fillTriangle(x_origin, y_origin, x_start, y_start, x_final, y_final, color);
}

/* static int introAnimationDelays(double theta) {
    double slowdown_start = 1.85, slowdown_finish = 2.05;
    int normal_delay = 30, peak_delay = 600;
    double slope =  (double)(peak_delay - normal_delay) / 
                    ((slowdown_start+slowdown_finish)/2 - slowdown_start);

    if (theta < slowdown_start) 
        return normal_delay;
    if (theta < (slowdown_start + slowdown_finish)/2) 
        return slope*theta - slope*slowdown_start + normal_delay;
    if (theta < slowdown_finish) 
        return -slope*theta + slope*slowdown_finish + normal_delay;
    return normal_delay;
} */

void eda_display_introAnimation() {
    double late_theta;        
    int radius = 123, x, y;
    for (double theta = PI/6; theta <= PI; theta += 0.05) {
        late_theta = theta - PI/10;
        if (theta < 5*PI/6 - 0.1) {
            x = 80+radius*cos(theta);
            y = 128-radius*sin(theta);
            drawPie(80, 128, x, y, 0.1, radius, PINK);
        }
        x = 80+(radius+5)*cos(late_theta);
        y = 128-(radius+5)*sin(late_theta);        
        drawPie(80, 128, x, y, 0.1, radius+5, BLACK);
        vTaskDelay(30/portTICK_PERIOD_MS);
    }
}
