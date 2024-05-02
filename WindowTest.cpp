#include "windowgl.h"
#include "font.h"

std::vector<LineData> lines;
std::vector<CircleData> circles;

INT WinMain(HINSTANCE hInstance, HINSTANCE hPreviousInst, LPSTR lpszCmdLine, int nCmdShow){
    Window window;
    if(ErrCheck(createWindow(window, hInstance, 1000, 1000, 0, 0, 1, "Fenster"), "Fenster öffnen") != SUCCESS) return -1;
    if(init() != SUCCESS) return -1;

    Image image;
    if(ErrCheck(loadImage("images/cat.tex", image), "Image laden") != SUCCESS) return -1;

    Font font;
    if(ErrCheck(loadTTF(font, "fonts/OpenSans-Bold.ttf"), "Font laden") != SUCCESS) return -1;

    bool LmbPressed = false;
    std::string text = "The quick brown fox jumps over the lazy dog.";
    std::string ftText = "FT: 0ms";

    Timer timer;
    while(1){
        getMessages(window);
        if(getWindowFlag(window, WINDOW_CLOSE)) break;

        if(getButton(mouse, MOUSE_LMB)){
            if(LmbPressed == false){
            }
            LmbPressed = true;
        }else LmbPressed = false;

        resetTimer(timer);
        clearWindow(window);

        drawFontString(font, lines, text.c_str(), 20, 20+font.verticalSpacing);
        drawFontString(font, lines, text.c_str(), 20, 20+font.verticalSpacing*2);
        drawFontString(font, lines, text.c_str(), 20, 20+font.verticalSpacing*3);
        drawFontString(font, lines, text.c_str(), 20, 20+font.verticalSpacing*4);
        drawFontString(font, lines, text.c_str(), 20, 20+font.verticalSpacing*5);
        drawFontString(font, lines, text.c_str(), 20, 20+font.verticalSpacing*6);
        drawFontString(font, lines, text.c_str(), 20, 20+font.verticalSpacing*7);
        drawFontString(font, lines, text.c_str(), 20, 20+font.verticalSpacing*8);
        drawFontString(font, lines, text.c_str(), 20, 20+font.verticalSpacing*9);
        drawFontString(font, lines, text.c_str(), 20, 20+font.verticalSpacing*10);
        drawFontString(font, lines, text.c_str(), 20, 20+font.verticalSpacing*11);

        drawFontString(font, lines, ftText.c_str(), 10, 10);
        renderCircles(window, circles.data(), circles.size());
        renderLines(window, lines.data(), lines.size());
        drawWindow(window);
        ftText = "Frametime: " + floatToString(getTimerMillis(timer), 2) + "ms";
        circles.clear();
        lines.clear();
        Sleep(16);
    }
    destroyFont(font);
    destroyImage(image);
    if(ErrCheck(destroyWindow(window), "Fenster schließen") != SUCCESS) return -1;
    return 0;
}
