/* ESPRESSIF DISTANT ALARM                                                      *
 * by Denis Davidoglu, 2023                                                     *
 *                                                                              *
 * Module for mapping regular characters to a 127-symbol set. Adafruit_GFX      *
 * library supports custom fonts with only 127 glyphs. To include Turkish and   *
 * Russian letters, I deleted lowercase English letters and some other symbols. */

#include "include/eda_font_encoder.h"

static const int encoderTable_LENGTH = 78;
static const char* encoderTable[][2] = {
    // {"i", "}"}, // covered
    {"ç", "%"},
    {"ü", "&"},
    {"а", "["},
    {"б", "\\"},
    {"в", "]"},
    {"г", "^"},    
    {"д", "_"},
    {"е", "`"},
    {"ж", "a"},
    {"з", "b"},
    {"и", "c"},
    {"й", "d"},
    {"к", "e"},
    {"л", "f"},
    {"м", "g"},
    {"н", "h"},
    {"о", "i"},
    {"п", "j"},
    {"р", "k"},
    {"с", "l"},
    {"т", "m"},
    {"у", "n"},
    {"ф", "o"},
    {"х", "p"},
    {"ц", "q"},
    {"ч", "r"},
    {"ш", "s"},
    {"щ", "t"},
    {"ъ", "u"},
    {"ы", "v"},
    {"ь", "w"},
    {"э", "x"},
    {"ю", "y"},
    {"я", "z"},
    {"ğ", "{"}, 
    {"ş", "|"},
    {"ö", "~"},
    {"Ç", "%"},
    {"Ü", "&"},
    {"А", "["},
    {"Б", "\\"},
    {"В", "]"},
    {"Г", "^"},
    {"Д", "_"},
    {"Е", "`"},
    {"Ж", "a"},
    {"З", "b"},
    {"И", "c"},
    {"Й", "d"},
    {"К", "e"},
    {"Л", "f"},
    {"М", "g"},
    {"Н", "h"},
    {"О", "i"},
    {"П", "j"},
    {"Р", "k"},
    {"С", "l"},
    {"Т", "m"},
    {"У", "n"},
    {"Ф", "o"},
    {"Х", "p"},
    {"Ц", "q"},
    {"Ч", "r"},
    {"Ш", "s"},
    {"Щ", "t"},
    {"Ъ", "u"},
    {"Ы", "v"},
    {"Ь", "w"},
    {"Э", "x"},
    {"Ю", "y"},
    {"Я", "z"},
    {"Ğ", "{"},
    {"Ş", "|"},
    {"İ", "}"},
    {"Ö", "~"},
    {"ı", "I"},
    {"ё", "`"},
    {"Ё", "`"}
};

String eda_font_encoder_process(String s) {
    char buff[256];
    s.toCharArray(buff, 256);
    String encoded;
    
    int i = 0;
    while (buff[i] != '\0') {
        char lb = buff[i];
        if (lb <= 127) { // ascii character
            if (buff[i] == 'i')
                encoded += '}';
            else if (buff[i] >= 'a' && buff[i] <= 'z')
                encoded += (char)(buff[i]-'a'+'A');
            else
                encoded += buff[i]; 
            i++;
        } else { // non-ascii character
            for (int j = 0; j < encoderTable_LENGTH; j++)
                if (encoderTable[j][0][0] == buff[i] && 
                encoderTable[j][0][1] == buff[i+1]) {
                    encoded += encoderTable[j][1][0];
                    break;
                }
            i += 2;
        }
    }
    return encoded;
}
