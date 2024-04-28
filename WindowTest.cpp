#include "windowgl.h"
#include "font.h"

void drawChar(Window& window, Font& font, BYTE c, WORD xOffset, WORD yOffset){
    Glyph& glyph = font.glyphStorage.glyphs[font.asciiToGlyphMapping[c]];
    WORD startIdx = 0;
    for(SWORD i=0; i < glyph.numContours; ++i){
        WORD endIdx = glyph.endOfContours[i];
        for(WORD j=startIdx; j < endIdx; ++j){
            WORD end = j+1;
            drawLine(window, glyph.xCoords[j]+xOffset, glyph.yCoords[j]+yOffset, glyph.xCoords[end]+xOffset, glyph.yCoords[end]+yOffset, 1, RGBA(255, 255, 255));
        }
        drawLine(window, glyph.xCoords[endIdx]+xOffset, glyph.yCoords[endIdx]+yOffset, glyph.xCoords[startIdx]+xOffset, glyph.yCoords[startIdx]+yOffset, 1, RGBA(255, 255, 255));
        startIdx = endIdx+1;
    }
    // for(SWORD i=0; i < glyph.numPoints; ++i){
    //     drawCircle(window, glyph.xCoords[i]+xOffset, glyph.yCoords[i]+yOffset, 4, RGBA(0, 180, 255));
    // }
}

INT WinMain(HINSTANCE hInstance, HINSTANCE hPreviousInst, LPSTR lpszCmdLine, int nCmdShow){
    Window window;
    if(ErrCheck(createWindow(window, hInstance, 1000, 1000, 0, 0, 1, "Fenster"), "Fenster öffnen") != SUCCESS) return -1;
    init();

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
    char text[] = "The quick brown fox jumps over the lazy dog.";
    while(1){
        getMessages(window);
        if(getWindowFlag(window, WINDOW_CLOSE)) break;

        if(getButton(mouse, MOUSE_LMB)){
            if(LmbPressed == false){

            }
            LmbPressed = true;
        }else LmbPressed = false;

        clearWindow(window);

        DWORD xOffset = 20;
        
        for(int i=0; i < strlen(text); ++i){
            drawChar(window, font, text[i], xOffset, 0);
            Glyph& glyph = font.glyphStorage.glyphs[font.asciiToGlyphMapping[text[i]]];
            drawLine(window, glyph.xMin+xOffset, glyph.yMin, glyph.xMax+xOffset, glyph.yMin, 1, RGBA(0, 255, 0));
            drawLine(window, glyph.xMin+xOffset, glyph.yMax, glyph.xMax+xOffset, glyph.yMax, 1, RGBA(255, 0, 0));
            if(font.horMetricsCount > 1) xOffset += font.horMetrics[font.asciiToGlyphMapping[text[i]]].advanceWidth;
            else xOffset += font.horMetrics[0].advanceWidth;
        }
        xOffset = 20;
        for(int i=0; i < strlen(text); ++i){
            drawChar(window, font, text[i], xOffset, (yMax-yMin)/scalingFactor);
            Glyph& glyph = font.glyphStorage.glyphs[font.asciiToGlyphMapping[text[i]]];
            drawLine(window, glyph.xMin+xOffset, glyph.yMin, glyph.xMax+xOffset, glyph.yMin, 1, RGBA(0, 255, 0));
            drawLine(window, glyph.xMin+xOffset, glyph.yMax, glyph.xMax+xOffset, glyph.yMax, 1, RGBA(255, 0, 0));
            if(font.horMetricsCount > 1) xOffset += font.horMetrics[font.asciiToGlyphMapping[text[i]]].advanceWidth;
            else xOffset += font.horMetrics[0].advanceWidth;
        }

        drawWindow(window);
        Sleep(16);
    }
    destroyFont(font);
    destroyImage(image);
    if(ErrCheck(destroyWindow(window), "Fenster schließen") != SUCCESS) return -1;
    return 0;
}
