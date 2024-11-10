#include <M5StickCPlus2.h>
#include "tally.h"

enum Page {
    PAGE_TALLY = 0,
    PAGE_MOISTURE,
    PAGE_COUNT
};

Page currentPage = PAGE_TALLY;
TallyPage tallyPage;

void switchPage() {
    currentPage = static_cast<Page>((currentPage + 1) % PAGE_COUNT);
    M5.Lcd.fillScreen(BLACK);
    
    switch (currentPage) {
        case PAGE_TALLY:
            tallyPage.updateDisplay();
            break;
    }
}

void setup() {
    M5.begin();
    M5.Lcd.setRotation(0);
    M5.Lcd.fillScreen(BLACK);
    
    tallyPage.init();
}

void loop() {
    M5.update();
    
    // if (M5.BtnPWR.wasPressed()) {
    //     switchPage();
    // }
    
    switch (currentPage) {
        case PAGE_TALLY:
            tallyPage.update();
            break;
    }
    
    delay(50);
}