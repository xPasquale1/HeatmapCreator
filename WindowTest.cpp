#include "windowgl.h"
#include "font.h"

std::vector<LineData> lines;
std::vector<CircleData> circles;
std::vector<CharData> chars;

INT WinMain(HINSTANCE hInstance, HINSTANCE hPreviousInst, LPSTR lpszCmdLine, int nCmdShow){
    Window window;
    if(ErrCheck(createWindow(window, hInstance, 1000, 1000, 0, 0, 1, "Fenster"), "Fenster öffnen") != SUCCESS) return -1;
    if(init() != SUCCESS) return -1;

    Image image;
    if(ErrCheck(loadImage("images/cat.tex", image), "Image laden") != SUCCESS) return -1;

    Font font;
    if(ErrCheck(loadTTF(font, "fonts/OpenSans-Bold.ttf"), "Font laden") != SUCCESS) return -1;

    while(1){
        getMessages(window);
        if(getWindowFlag(window, WINDOW_CLOSE)) break;

        if(getButton(mouse, MOUSE_LMB)){
            font.pixelSize += 1;
        }
        if(getButton(mouse, MOUSE_RMB)){
            font.pixelSize -= 1;
        }

        clearWindow(window);

        drawFontString(window, font, chars, "The quick brown fox jumps over the lazy dog.", 10, 10);
        drawFontString(window, font, chars, "The quick brown fox jumps over the lazy dog.", 10, 10+font.pixelSize);
        drawFontString(window, font, chars, "The quick brown fox jumps over the lazy dog.", 10, 10+font.pixelSize*2);
        drawFontString(window, font, chars, "The quick brown fox jumps over the lazy dog.", 10, 10+font.pixelSize*3);
        drawFontString(window, font, chars, "The quick brown fox jumps over the lazy dog.", 10, 10+font.pixelSize*4);
        drawFontString(window, font, chars, "The quick brown fox jumps over the lazy dog.", 10, 10+font.pixelSize*5);
        drawFontString(window, font, chars, "The quick brown fox jumps over the lazy dog.", 10, 10+font.pixelSize*6);
        drawFontString(window, font, chars, "The quick brown fox jumps over the lazy dog.", 10, 10+font.pixelSize*7);
        drawFontString(window, font, chars, "The quick brown fox jumps over the lazy dog.", 10, 10+font.pixelSize*8);

        renderCircles(window, circles.data(), circles.size());
        renderLines(window, lines.data(), lines.size());
        renderFontChars(window, font, chars.data(), chars.size());
        drawWindow(window);
        circles.clear();
        lines.clear();
        chars.clear();
        Sleep(16);
    }
    destroyFont(font);
    destroyImage(image);
    if(ErrCheck(destroyWindow(window), "Fenster schließen") != SUCCESS) return -1;
    return 0;
}
