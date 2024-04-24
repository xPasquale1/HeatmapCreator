#include "windowgl.h"

INT WinMain(HINSTANCE hInstance, HINSTANCE hPreviousInst, LPSTR lpszCmdLine, int nCmdShow){
    Window window;
    Window window2;
    // if(ErrCheck(createWindow(window, hInstance, 800, 800, 0, 0, 1, "Test-Fenster1"), "Fenster öffnen") != SUCCESS) return -1;
    // if(ErrCheck(createWindow(window2, hInstance, 800, 800, 400, 0, 1, "Test-Fenster2"), "Fenster öffnen") != SUCCESS) return -1;
    // init();
    // enableBlending(window2);

    Image image;
    if(ErrCheck(loadImage("images/cat.tex", image), "Image laden") != SUCCESS) return -1;

    if(ErrCheck(loadTTF("fonts/ARIAL.TTF"), "Font laden") != SUCCESS) return -1;

    while(1){
        getMessages(window);
        getMessages(window2);
        if(getWindowFlag(window, WINDOW_CLOSE)) break;
        if(getWindowFlag(window2, WINDOW_CLOSE)) break;

        // clearWindow(window);
        // clearWindow(window2);

        // drawRectangle(window, 20, 20, 80, 80, RGBA(255, 255, 255, 255));
        // drawRectangle(window2, 20, 20, 80, 80, RGBA(255, 255, 255, 255));

        // drawImage(window, image, 50, 50, 750, 750);
        // drawImage(window2, image, 50, 50, 750, 750);

        // drawWindow(window);
        // drawWindow(window2);
        Sleep(16);
    }
    // if(ErrCheck(destroyWindow(window), "Fenster schließen") != SUCCESS) return -1;
    // if(ErrCheck(destroyWindow(window2), "Fenster schließen") != SUCCESS) return -1;
    return 0;
}
