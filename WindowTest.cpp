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

    float scalingFactor = font.unitsPerEm/75.f;
    SWORD yMin = font.glyphStorage.glyphs[font.asciiToGlyphMapping[0]].yMin;
    SWORD yMax = font.glyphStorage.glyphs[font.asciiToGlyphMapping[0]].yMax;
    for(WORD i=1; i < 256; ++i){
        SWORD min = font.glyphStorage.glyphs[font.asciiToGlyphMapping[i]].yMin;
        SWORD max = font.glyphStorage.glyphs[font.asciiToGlyphMapping[i]].yMax;
        if(min < yMin) yMin = min;
        if(max > yMax) yMax = max;
    }
    for(WORD i=0; i < font.horMetricsCount; ++i) font.horMetrics[i].advanceWidth /= scalingFactor;
    for(WORD i=0; i < font.glyphStorage.glyphCount; ++i){
        Glyph& glyph = font.glyphStorage.glyphs[i];
        glyph.yMin = (0-glyph.yMin)+yMax;
        glyph.yMax = (0-glyph.yMax)+yMax;
        glyph.yMin /= scalingFactor;
        glyph.yMax /= scalingFactor;
        glyph.xMin /= scalingFactor;
        glyph.xMax /= scalingFactor;
        for(WORD j=0; j < glyph.numPoints; ++j){
            glyph.yCoords[j] = (0-glyph.yCoords[j])+yMax;
            glyph.xCoords[j] /= scalingFactor;
            glyph.yCoords[j] /= scalingFactor;
        }
    }

    bool LmbPressed = false;
    std::string text = "Kann ich jetzt die Buchstaben äÄöÖüÜ zeigen? o:";

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

        drawFontString(font, lines, text.c_str(), 20, 20);
        drawFontString(font, lines, text.c_str(), 20, 20+(yMax-yMin)/scalingFactor);
        drawFontString(font, lines, text.c_str(), 20, 20+(yMax-yMin)/scalingFactor*2);
        drawFontString(font, lines, text.c_str(), 20, 20+(yMax-yMin)/scalingFactor*3);
        drawFontString(font, lines, text.c_str(), 20, 20+(yMax-yMin)/scalingFactor*4);
        drawFontString(font, lines, text.c_str(), 20, 20+(yMax-yMin)/scalingFactor*5);
        drawFontString(font, lines, text.c_str(), 20, 20+(yMax-yMin)/scalingFactor*6);
        drawFontString(font, lines, text.c_str(), 20, 20+(yMax-yMin)/scalingFactor*7);
        drawFontString(font, lines, text.c_str(), 20, 20+(yMax-yMin)/scalingFactor*8);
        drawFontString(font, lines, text.c_str(), 20, 20+(yMax-yMin)/scalingFactor*9);


        renderCircles(window, circles.data(), circles.size());
        renderLines(window, lines.data(), lines.size());
        drawWindow(window);
        circles.clear();
        lines.clear();
        // std::cout << getTimerMillis(timer) << std::endl;
        Sleep(16);
    }
    destroyFont(font);
    destroyImage(image);
    if(ErrCheck(destroyWindow(window), "Fenster schließen") != SUCCESS) return -1;
    return 0;
}
