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
    if(ErrCheck(loadTTF(font, "fonts/OpenSans-Bold.ttf", 400), "Font laden") != SUCCESS) return -1;

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

        drawFontCharOutline(font, lines, 'B', 20, 20);

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
