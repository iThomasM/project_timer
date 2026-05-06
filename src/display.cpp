#include "display.h"

GyverOLED<SSH1106_128x64> oled;

int screenWidth = 128;
int screenHeight = 64;
int scale = 1;

float calcAo5(float ao5[5]) {
    float slowest = -INFINITY;
    float fastest = INFINITY; 
    int slowestIndex = -1;
    int fastestIndex = -1;
    float mo3[3];

    for (int i = 0; i < 5; i++) {
        if (ao5[i] > slowest) {
            slowest = ao5[i];
            slowestIndex = i;
        }
        if (ao5[i] < fastest) {
            fastest = ao5[i];
            fastestIndex = i;
        }
    }

    int j = 0; 
    for (int i = 0; i < 5; i++) {
        if (i != slowestIndex && i != fastestIndex) {
            mo3[j] = ao5[i];
            j++;
            if (j == 3) break;
        }
    }

    float sum = mo3[0] + mo3[1] + mo3[2];
    return sum / 3;
}

void displayIdle(float time, float ao5[], int ao5len, int currentSelection) {
    float curAo5 = calcAo5(ao5);
    oled.clear();

    String timeStr = (time == INFINITY) ? "DNF" : String(time, 3); //condition ? value_true : value_false
    int textWidth = 5 * scale * timeStr.length();
    int textHeight = 7 * scale;
    int xpos = (screenWidth - textWidth) / 2;
    int ypos = (screenHeight - textHeight) / 2;

    oled.setScale(scale);
    oled.setCursorXY(xpos, ypos);
    oled.invertText(currentSelection == CurrentTime);
    oled.print(timeStr);

    if (ao5len == 5) { 
        oled.setCursorXY(8, 8);
        if (currentSelection == Ao5) oled.invertText(1); 
        else oled.invertText(0); 
        if (curAo5 == INFINITY) oled.print("DNF"); 
        else (oled.print(calcAo5(ao5), 3)); }   
        oled.update();
}

void displayAo5Stage(float ao5[], int ao5len) {
    oled.clear();
    oled.invertText(0);
    oled.setCursorXY(0, 0);
    if (ao5len == 5) {
        for (int i = 0; i < 5; i++) {
            oled.setCursorXY(0, 8 + (i * 8));
            if (ao5[i] == INFINITY) oled.print("DNF");
            else (oled.print(ao5[i], 3));
        }
    } else {
        oled.print("Ao5: N/A");
    }
    oled.update();
}

void displayCurrentTimeStage(float time, int penaltySelection) {

    oled.clear();
    const char* options[] = {"+2", "DNF", "DEL", "OK"};

    // chatgpt code maybe it'll work
    int x = 0;
    for (int i = 0; i < 4; i++) {
        oled.setCursorXY(x, 0);

        if (i == penaltySelection) {
            oled.invertText(1);  
        } else {
            oled.invertText(0);
        }

        oled.print(options[i]);

        x += 30; 
    }

    oled.invertText(0); 
    oled.setCursorXY(0, 20);
    oled.print(time, 3);

    oled.update();
}

void displayStarting() {
    // inefficient copy-pasted code.
    String startstring = "STARTING";
    int textWidth = 5 * scale * startstring.length();
    int textHeight = 7 * scale;
    int xpos = (screenWidth - textWidth) / 2;
    int ypos = (screenHeight - textHeight) / 2;
    oled.clear();
    oled.setCursorXY(xpos, ypos);
    oled.print(startstring);
    oled.update();
}

void displayReady() {
    String rdystring = "READY";
    int textWidth = 5 * scale * rdystring.length();
    int textHeight = 7 * scale;
    int xpos = (screenWidth - textWidth) / 2;
    int ypos = (screenHeight - textHeight) / 2;
    oled.clear();
    oled.setCursorXY(xpos, ypos);
    oled.print(rdystring);
    oled.update();
}

void displayTiming(float time) {
    String timingstring = String(time, 3);
    int textWidth = 5 * scale * timingstring.length();
    int textHeight = 7 * scale;
    int xpos = (screenWidth - textWidth) / 2;
    int ypos = (screenHeight - textHeight) / 2;
    oled.clear();
    oled.setCursorXY(xpos, ypos);
    oled.print(timingstring);
    oled.update();
}

