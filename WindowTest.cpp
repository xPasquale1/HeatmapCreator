#include "windowgl.h"
#include "font.h"

INT WinMain(HINSTANCE hInstance, HINSTANCE hPreviousInst, LPSTR lpszCmdLine, int nCmdShow){
    Window window;
    Window window2;
    if(ErrCheck(createWindow(window, hInstance, 1000, 1000, 0, 0, 1, "Test-Fenster1"), "Fenster öffnen") != SUCCESS) return -1;
    init();

    Image image;
    if(ErrCheck(loadImage("images/cat.tex", image), "Image laden") != SUCCESS) return -1;

    if(ErrCheck(loadTTF("fonts/SpaceMono-Regular.ttf"), "Font laden") != SUCCESS) return -1;
    // if(ErrCheck(loadTTF("fonts/ARIAL.TTF"), "Font laden") != SUCCESS) return -1;

    for(WORD i=0; i < glyphStorage.glyphCount; ++i){
        Glyph& glyph = glyphStorage.glyphs[i];
        for(WORD j=0; j < glyph.numPoints; ++j){
            glyph.yCoords[j] = (glyph.yMax-glyph.yCoords[j])+glyph.yMin;
            glyph.xCoords[j] /= 5;
            glyph.yCoords[j] /= 5;
        }
    }

    while(1){
        getMessages(window);
        if(getWindowFlag(window, WINDOW_CLOSE)) break;

        clearWindow(window);

        DWORD xOffset = 50;
        DWORD yOffset = 50;
        for(WORD gs=4; gs < 39; ++gs){
            Glyph& glyph = glyphStorage.glyphs[gs];
            WORD startIdx = 0;
            for(SWORD i=0; i < glyph.numContours; ++i){
                WORD endIdx = glyph.endOfContours[i];
                for(WORD j=startIdx; j < endIdx; ++j){
                    WORD end = j+1;
                    drawLine(window, glyph.xCoords[j]+xOffset, glyph.yCoords[j]+yOffset, glyph.xCoords[end]+xOffset, glyph.yCoords[end]+yOffset, 2, RGBA(255, 255, 255));
                }
                drawLine(window, glyph.xCoords[endIdx]+xOffset, glyph.yCoords[endIdx]+yOffset, glyph.xCoords[startIdx]+xOffset, glyph.yCoords[startIdx]+yOffset, 1, RGBA(255, 255, 255));
                startIdx = endIdx+1;
            }
            for(SWORD i=0; i < glyph.numPoints; ++i){
                drawCircle(window, glyph.xCoords[i]+xOffset, glyph.yCoords[i]+yOffset, 4, RGBA(0, 180, 255));
            }
            xOffset += 120;
            if(xOffset >= window.windowWidth-120-50){
                xOffset = 50;
                yOffset += 190;
            }
        }

        drawWindow(window);
        Sleep(32);
    }
    destroyGlyphStorage(glyphStorage);
    destroyImage(image);
    if(ErrCheck(destroyWindow(window), "Fenster schließen") != SUCCESS) return -1;
    return 0;
}
