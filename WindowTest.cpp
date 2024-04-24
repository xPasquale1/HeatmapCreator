#include "windowgl.h"

WORD swapEndian(WORD* val)noexcept{
    BYTE* out = (BYTE*)val;
    return (out[0]<<8) | out[1];
}

DWORD swapEndian(DWORD* val)noexcept{
    BYTE* out = (BYTE*)val;
    return (out[0]<<24) | (out[1]<<16) | (out[2]<<8) | out[0];
}

constexpr DWORD tableStringToCode(const char* name)noexcept{
    return (name[3]<<24) | (name[2]<<16) | (name[1]<<8) | name[0];
} 

INT WinMain(HINSTANCE hInstance, HINSTANCE hPreviousInst, LPSTR lpszCmdLine, int nCmdShow){
    Window window;
    Window window2;
    if(ErrCheck(createWindow(window, hInstance, 800, 800, 0, 0, 1, "Test-Fenster1"), "Fenster öffnen") != SUCCESS) return -1;
    if(ErrCheck(createWindow(window2, hInstance, 800, 800, 400, 400, 1, "Test-Fenster2"), "Fenster öffnen") != SUCCESS) return -1;
    init();
    enableBlending(window2);

    Image image;
    if(ErrCheck(loadImage("images/cat.tex", image), "Image laden") != SUCCESS) return -1;

    std::fstream file;
    file.open("fonts/ARIAL.TTF", std::ios::in | std::ios::binary);
    if(!file.is_open()){
        std::cout << "Datei nicht gefunden" << std::endl;
        return -1;
    }
    file.seekp(0, std::ios::end);
    std::cout << "Dateigröße in Bytes: " << file.tellp() << std::endl;
    WORD numTables;
    file.seekg(4, std::ios::beg);
    file.read((char*)&numTables, 2);
    numTables = swapEndian(&numTables);
    std::cout << "Tabellen Anzahl: " << numTables << std::endl;
    WORD seekOffset = 12;
    for(WORD i=0; i < numTables; ++i){
        file.seekg(seekOffset, std::ios::beg);
        DWORD tag;
        file.read((char*)&tag, 4);
        for(BYTE j=0; j < 4; ++j){
            std::cout << ((char*)(&tag))[j];
        } std::cout << std::endl;

        if(tag == tableStringToCode("glyf")){
            std::cout << "Happy me!" << std::endl;
        }

        file.seekg(seekOffset+8, std::ios::beg);
        DWORD offset;
        file.read((char*)&offset, 4);
        offset = swapEndian(&offset);
        std::cout << "Offset: " << offset << std::endl;

        seekOffset += 16;
    }

    while(1){
        getMessages(window);
        getMessages(window2);
        if(getWindowFlag(window, WINDOW_CLOSE)) break;
        if(getWindowFlag(window2, WINDOW_CLOSE)) break;

        clearWindow(window);
        clearWindow(window2);

        drawRectangle(window, 20, 20, 80, 80, RGBA(255, 255, 255, 255));
        drawRectangle(window2, 20, 20, 80, 80, RGBA(255, 255, 255, 255));

        drawImage(window, image, 50, 50, 750, 750);
        drawImage(window2, image, 50, 50, 750, 750);

        drawWindow(window);
        drawWindow(window2);
        Sleep(16);
    }
    if(ErrCheck(destroyWindow(window), "Fenster schließen") != SUCCESS) return -1;
    if(ErrCheck(destroyWindow(window2), "Fenster schließen") != SUCCESS) return -1;
    return 0;
}
