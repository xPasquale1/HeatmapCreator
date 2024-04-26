#include "windowgl.h"
#include "font.h"

void drawChar(Window& window, Font& font, BYTE c, WORD xOffset, WORD yOffset){
    Glyph& glyph = font.glyphStorage.glyphs[font.asciiToGlyphMapping[c]];
    WORD startIdx = 0;
    for(SWORD i=0; i < glyph.numContours; ++i){
        WORD endIdx = glyph.endOfContours[i];
        for(WORD j=startIdx; j < endIdx; ++j){
            WORD end = j+1;
            drawLine(window, glyph.xCoords[j]+xOffset, glyph.yCoords[j]+yOffset, glyph.xCoords[end]+xOffset, glyph.yCoords[end]+yOffset, 2, RGBA(255, 255, 255));
        }
        drawLine(window, glyph.xCoords[endIdx]+xOffset, glyph.yCoords[endIdx]+yOffset, glyph.xCoords[startIdx]+xOffset, glyph.yCoords[startIdx]+yOffset, 2, RGBA(255, 255, 255));
        startIdx = endIdx+1;
    }
    for(SWORD i=0; i < glyph.numPoints; ++i){
        drawCircle(window, glyph.xCoords[i]+xOffset, glyph.yCoords[i]+yOffset, 4, RGBA(0, 180, 255));
    }
}

INT WinMain(HINSTANCE hInstance, HINSTANCE hPreviousInst, LPSTR lpszCmdLine, int nCmdShow){
    Window window;
    Window window2;
    if(ErrCheck(createWindow(window, hInstance, 1000, 1000, 0, 0, 1, "Test-Fenster1"), "Fenster öffnen") != SUCCESS) return -1;
    init();

    Image image;
    if(ErrCheck(loadImage("images/cat.tex", image), "Image laden") != SUCCESS) return -1;

    Font font;
    if(ErrCheck(loadTTF(font, "fonts/OpenSans-Bold.ttf"), "Font laden") != SUCCESS) return -1;

    std::cout << font.yMax << " " << font.yMin << std::endl;
    for(WORD i=0; i < font.glyphStorage.glyphCount; ++i){
        Glyph& glyph = font.glyphStorage.glyphs[i];
        for(WORD j=0; j < glyph.numPoints; ++j){
            // glyph.yCoords[j] = glyph.yMax-glyph.yCoords[j];
            // glyph.xCoords[j] = (glyph.xCoords[j]*128)/(glyph.xMax-glyph.xMin);
            // glyph.yCoords[j] = (glyph.yCoords[j]*128)/(glyph.yMax-glyph.yMin);
            glyph.xCoords[j] /= 10;
            glyph.yCoords[j] /= 10;
        }
    }

    BYTE g = 0;
    while(1){
        getMessages(window);
        if(getWindowFlag(window, WINDOW_CLOSE)) break;

        clearWindow(window);

        DWORD xOffset = 20;
        
        char text[] = "The q";
        for(int i=0; i < strlen(text); ++i){
            drawChar(window, font, text[i], xOffset, 100);
            Glyph& glyph = font.glyphStorage.glyphs[font.asciiToGlyphMapping[text[i]]];
            // xOffset += 55;
            xOffset += 110;
        }

        drawWindow(window);
        Sleep(32);
    }
    destroyFont(font);
    destroyImage(image);
    if(ErrCheck(destroyWindow(window), "Fenster schließen") != SUCCESS) return -1;
    return 0;
}
