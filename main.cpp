#include "network.h"
#include "../OpenGL-Library/windowgl.h"
#include <thread>
#include <algorithm>

/*
    TODOS:
    Webassembly, also das man alles im browser nutzen kann

    Lineare Interpolation des Triangulationsalgorithmus zu einer logarithmischen umändern

    IP an den esp32 senden können

    Datenpunkte RSSI Anzahl "dynamisch" machen

    Datepunktauflösung in der Applikation festelegen können

    Fehler-/Statusmeldungen in der Applikation anzeigen
*/

Window window;
Font font;
UDPServer mainServer;

TextInput routerInput;
TextInput ipInput;

std::vector<LineData> lines;
std::vector<CircleData> circles;
std::vector<RectangleData> rectangles;
std::vector<CharData> chars;

//TODO Annahme Signalstärke von -20dB bis -90dB
#define MAXDB 100
#define MINDB 20

#define DATAPOINTRESOLUTIONX 100
#define DATAPOINTRESOLUTIONY 100
#define HEATMAPCOUNT 3
ScreenVec routerPositions[HEATMAPCOUNT];
BYTE routerPositionsCount = 0;

enum MODES{
    HEATMAPMODE,
    SEARCHMODE,
    DISPLAYMODE,
    ENDOFMODES
};

struct Datapoint{
    WORD x = 0;
    WORD y = 0;
    BYTE rssi[HEATMAPCOUNT];    //TODO BYTE* und zur Laufzeit allokieren und den max. Wert in rssiCount merken
    BYTE rssiCount = 0;
};
Hashmap datapoints;      //Speichert die Datenpunkte, welche Angeziegt, für Berechnungen,... verwendet werden

#define coordinatesToKey(x, y)((x<<16)|y)

std::vector<SBYTE> singleRssiData;
std::vector<SBYTE> rssiData[HEATMAPCOUNT];

BYTE mode = 0;
DWORD searchColor[HEATMAPCOUNT]{0};  //TODO auch zur Laufzeit allokieren und durch das Applikationsinterface veränderbar machen
bool running = true;
WORD gx = 0;
WORD gy = 0;
BYTE blink = 0;

Image heatmapsInterpolated[HEATMAPCOUNT];   //TODO dynamisch

Button buttons[15];
TextInput inputs[2];
WORD buttonCount = 0;
WORD inputCount = 0;

LRESULT CALLBACK mainWindowCallback(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

//Fügt einen neuen globalen Datepunkt hinzu, sollte es diesen nicht schon bereits an der Position geben
void changeDatapoint(BYTE* rssi, WORD x, WORD y){
    Datapoint* datapoint = (Datapoint*)searchHashmap(datapoints, coordinatesToKey(x, y));
    if(!datapoint){
        datapoint = new Datapoint;
        datapoint->x = x;
        datapoint->y = y;
        for(BYTE i=0; i < HEATMAPCOUNT; ++i) datapoint->rssi[i] = rssi[i];
    }
    insertHashmap(datapoints, coordinatesToKey(x, y), datapoint);
}

//Konvertiert einen RSSI-Wert (in der Range MINDB-MAXDB) zu einem 8Bit Wert (0-255)
BYTE rssiToColorComponent(BYTE rssi)noexcept{
    return ((rssi-MINDB)*255)/(MAXDB-MINDB);
}

BYTE colorComponentToRssi(BYTE color)noexcept{
    return color*(MAXDB-MINDB)/255+MINDB;
}

//TODO Fehler melden?

/// @brief Öffnet den globalen Server und hört einfach ob Pakete vom esp32 ankommen und verarbeitet diese
/// @param -
void processNetworkPackets()noexcept{
    char buffer[1024];
    while(1){
        if(!running) break;
        int length = receiveUDPServer(mainServer, buffer, sizeof(buffer));
        if(length != SOCKET_ERROR){     //TODO Problem ist der Timeout wird auch als SOCKET_ERROR ausgegeben...
            switch((BYTE)buffer[0]){
                case 2:{    //Signalstärke der Router
                    for(int i=1; i < length; ++i){
                        if(i > HEATMAPCOUNT) break;
                        buffer[i] = -buffer[i];         //Forme die RSSI-Werte vom negativen ins positive um
                        if(buffer[i] == 0){
                            buffer[i] = MAXDB;
                            ErrCheck(GENERIC_ERROR, std::string("Heatmapdaten von Netzwerk " + std::to_string(i) + " wurden nicht aufgezeichnet!").c_str());
                        }
                        switch(mode){
                            case DISPLAYMODE:
                            case HEATMAPMODE:{
                                rssiData[i-1].push_back(buffer[i]);
                                break;
                            }
                            case SEARCHMODE:{
                                BYTE color = rssiToColorComponent(buffer[i]);
                                searchColor[i-1] = RGBA(color, 255-color, 1);
                                break;
                            }
                        }
                    }
                    if(mode == HEATMAPMODE) changeDatapoint((BYTE*)(buffer+1), gx, gy);
                    blink = 8;
                    break;
                }
                case 0:{    //X
                    gx++;
                    blink = 0;
                    if(gx >= DATAPOINTRESOLUTIONX) gx = 0;
                    break;
                }
                case 1:{    //Y
                    gy++;
                    blink = 0;
                    if(gy >= DATAPOINTRESOLUTIONY) gy = 0;
                    break;
                }
                case 6:{    //Einzelne Rssi Daten
                    if(buffer[1] != 0) singleRssiData.push_back(buffer[1]);
                }
            }
        }
    }
}

//Zeichnet ein linear interpoliertes Dreick in das Image image, mit den Eckpunkten (x1, y1),... und den Eckpunktfarben val1,...
void drawInterpolatedTriangle(Image& image, WORD x1, WORD y1, WORD x2, WORD y2, WORD x3, WORD y3, BYTE val1, BYTE val2, BYTE val3)noexcept{
    WORD minX = std::min(x1, std::min(x2, x3));
    WORD maxX = std::max(x1, std::max(x2, x3));
    WORD minY = std::min(y1, std::min(y2, y3));
    WORD maxY = std::max(y1, std::max(y2, y3));

    float totalArea = 1.f/((x2-x1)*(y3-y2)-(y2-y1)*(x3-x2));

    for(WORD y=minY; y <= maxY; ++y){
        for(WORD x=minX; x <= maxX; ++x){
            float m1 = ((x2-x)*(y3-y)-(x3-x)*(y2-y))*totalArea;
            float m2 = ((x3-x)*(y1-y)-(x1-x)*(y3-y))*totalArea;     //TODO können inkremental in jeder Schleife berechnet werden
            float m3 = 1-m1-m2;
            if(m1 >= 0 && m2 >= 0 && m3 >= 0){   //TODO Funktioniert noch nicht ganz korrekt, vllt kann man float Zahlen vermeiden?
                // m1 = std::pow(m1, 1.1f);
                // m2 = std::pow(m2, 1.1f);
                // m3 = std::pow(m3, 1.1f);
                BYTE value = val1*m1 + val2*m2 + val3*m3;
                image.data[y*image.width+x] = RGBA(value, 255-value, 0);
            }
        }
    }
}

bool lineSegmentIntersection(WORD xBeg1, WORD yBeg1, WORD xEnd1, WORD yEnd1, WORD xBeg2, WORD yBeg2, WORD xEnd2, WORD yEnd2, DWORD& identical)noexcept{
    if(xBeg1 == xBeg2 && yBeg1 == yBeg2 && xEnd1 == xEnd2 && yEnd1 == yEnd2){   //Linien sind identisch
        identical++;
        return false;
    }
    if(xBeg1 == xEnd2 && yBeg1 == yEnd2 && xEnd1 == xBeg2 && yEnd1 == yBeg2){
        identical++;
        return false;
    }
    int dx1 = xEnd1 - xBeg1;
    int dy1 = yEnd1 - yBeg1;
    int dx2 = xEnd2 - xBeg2;
    int dy2 = yEnd2 - yBeg2;

    int denom = dy2*dx1-dx2*dy1;
    if(denom == 0){     //Linien parallel, teste ob einer der Punkte in der jeweils anderen Linie liegt
        float tx1 = (xBeg2 - xBeg1)/(float)dx1;
        float ty1 = (yBeg2 - yBeg1)/(float)dy1;
        float tx2 = (xEnd2 - xBeg1)/(float)dx1;
        float ty2 = (yEnd2 - yBeg1)/(float)dy1;
        float tx3 = (xBeg1 - xBeg2)/(float)dx2;
        float ty3 = (yBeg1 - yBeg2)/(float)dy2;
        float tx4 = (xEnd1 - xBeg2)/(float)dx2;
        float ty4 = (yEnd1 - yBeg2)/(float)dy2;
        if(tx1 > 0.001 && tx1 < 0.999 && ty1 > 0.001 && ty1 < 0.999) return true;
        if(tx2 > 0.001 && tx2 < 0.999 && ty2 > 0.001 && ty2 < 0.999) return true;
        if(tx3 > 0.001 && tx3 < 0.999 && ty3 > 0.001 && ty3 < 0.999) return true;
        if(tx4 > 0.001 && tx4 < 0.999 && ty4 > 0.001 && ty4 < 0.999) return true;
        return false;
    }

    int dxBeg = xBeg2-xBeg1;
    int dyBeg = yBeg2-yBeg1;

    float t1 = (dy2*dxBeg-dx2*dyBeg)/(float)denom;
    float t2 = (dy1*dxBeg-dx1*dyBeg)/(float)denom;

    return (t1 > 0 && t1 < 1 && t2 > 0 && t2 < 1);  //TODO ist noch nicht ganz richtig, es könnte ja ein Endpunkt auf einer Kante liegen, was okay ist
}

bool triangleOverlap(WORD x11, WORD y11, WORD x12, WORD y12, WORD x13, WORD y13, WORD x21, WORD y21, WORD x22, WORD y22, WORD x23, WORD y23, DWORD& identical)noexcept{
    if(lineSegmentIntersection(x11, y11, x12, y12, x21, y21, x22, y22, identical)) return true;
    if(lineSegmentIntersection(x11, y11, x13, y13, x21, y21, x22, y22, identical)) return true;
    if(lineSegmentIntersection(x12, y12, x13, y13, x21, y21, x22, y22, identical)) return true;

    if(lineSegmentIntersection(x11, y11, x12, y12, x21, y21, x23, y23, identical)) return true;
    if(lineSegmentIntersection(x11, y11, x13, y13, x21, y21, x23, y23, identical)) return true;
    if(lineSegmentIntersection(x12, y12, x13, y13, x21, y21, x23, y23, identical)) return true;

    if(lineSegmentIntersection(x11, y11, x12, y12, x22, y22, x23, y23, identical)) return true;
    if(lineSegmentIntersection(x11, y11, x13, y13, x22, y22, x23, y23, identical)) return true;
    if(lineSegmentIntersection(x12, y12, x13, y13, x22, y22, x23, y23, identical)) return true;

    return false;
}

struct DatapointTriangle{
    Datapoint* d1;
    Datapoint* d2;
    Datapoint* d3;
};

//TODO O(n^4) brute force Delauney Umsetzung, es gibt deutlich effizientere Methoden
//TODO man könnte auch einfach alle Heatmaps berechnen, da man dann nicht die Triangulation immer neu berechnen muss

// #define VISUALIZETRIANGULATION

/// @brief Trianguliert eine Punktewolke von Datenpunkten, interpoliert die Werte dazwischen und füllt damit das Image aus heatmaps am Index heatmapIdx
/// @param heatmaps Ein Array aus Images
/// @param heatmapIdx Das Image aus heatmaps das beschrieben werden soll
/// @param datapoints Die Datenpunkte
/// @param count Die Anzahl der Datenpunkte
void interpolateTriangulation(Image* heatmaps, BYTE heatmapIdx, Datapoint* datapoints, DWORD count)noexcept{
    if(count < 3) return;
    std::vector<DatapointTriangle> triangles;    //Hält alle Datenpunkt Dreiecke TODO kann man im vorab berechnen

    for(DWORD i=0; i < count; ++i){
        Datapoint& p1 = datapoints[i];
        for(DWORD j=0; j < count; ++j){
            if(j == i) continue;
            Datapoint& p2 = datapoints[j];
            for(DWORD k=0; k < count; ++k){
                if(k == j || k == i) continue;
                Datapoint& p3 = datapoints[k];
                bool valid = true;

                float totalArea = (p2.x-p1.x)*(p3.y-p2.y)-(p2.y-p1.y)*(p3.x-p2.x);
                if(totalArea <= 0) continue;

                float midXP1P2 = (p1.x+p2.x)/2;
                float midYP1P2 = (p1.y+p2.y)/2;
                float dyP1P2 = (p2.y-p1.y);
                float mP1P2;
                dyP1P2 != 0 ? mP1P2 = -(p2.x-p1.x)/dyP1P2 : mP1P2 = 1e4f;
                float bP1P2 = midYP1P2 - mP1P2*midXP1P2;

                float midXP1P3 = (p1.x+p3.x)/2;
                float midYP1P3 = (p1.y+p3.y)/2;
                float dyP1P3 = (p3.y-p1.y);
                float mP1P3;
                dyP1P3 != 0 ? mP1P3 = -(p3.x-p1.x)/dyP1P3 : mP1P3 = 1e4f;
                float bP1P3 = midYP1P3 - mP1P3*midXP1P3;

                float centerX = (bP1P3-bP1P2)/(mP1P2-mP1P3);
                float centerY = mP1P2*centerX+bP1P2;
                float radius2 = (centerX-p1.x)*(centerX-p1.x)+(centerY-p1.y)*(centerY-p1.y);
                for(DWORD l=0; l < count; ++l){
                    if(l == k || l == j || l == i) continue;
                    Datapoint& p = datapoints[l];
                    float distance2 = (centerX-p.x)*(centerX-p.x)+(centerY-p.y)*(centerY-p.y);
                    if(distance2 <= radius2){   //Das sollte eigentlich ja < sein, aber dann gab es Lücken und so hat das bisher keine Probleme gemacht
                        valid = false;
                        break;
                    }
                }
                if(!valid) continue;
                DWORD identical = 0;
                for(size_t l=0; l < triangles.size(); ++l){     //Testet ob das Dreieck mit anderen überlappt
                    DatapointTriangle& tri = triangles[l];
                    if(triangleOverlap(p1.x, p1.y, p2.x, p2.y, p3.x, p3.y, tri.d1->x, tri.d1->y, tri.d2->x, tri.d2->y, tri.d3->x, tri.d3->y, identical)){
                        valid = false;
                        break;
                    }
                }
                if(!valid) continue;    //TODO identical muss vllt auch noch geprüft werden

                #ifdef VISUALIZETRIANGULATION
                clearWindow(window);
                for(DWORD l=0; l < count; ++l){
                    drawRectangle(window, scaledDatapoints[l].x+200-2, scaledDatapoints[l].y-2, 4, 4, RGBA(255, 255, 255));
                }
                for(size_t l=0; l < triangles.size(); ++l){
                    DatapointTriangle& tri = triangles[l];
                    drawLine(window, tri.d1->x+200, tri.d1->y, tri.d2->x+200, tri.d2->y, RGBA(255, 255, (l+1)/triangles.size()));
                    drawLine(window, tri.d1->x+200, tri.d1->y, tri.d3->x+200, tri.d3->y, RGBA(255, 255, (l+1)/triangles.size()));
                    drawLine(window, tri.d3->x+200, tri.d3->y, tri.d2->x+200, tri.d2->y, RGBA(255, 255, (l+1)/triangles.size()));
                }
                drawLine(window, p1.x+200, p1.y, p2.x+200, p2.y, RGBA(255, 0, 0));
                drawLine(window, p1.x+200, p1.y, p3.x+200, p3.y, RGBA(255, 0, 0));
                drawLine(window, p3.x+200, p3.y, p2.x+200, p2.y, RGBA(255, 0, 0));
                drawWindow(window);
                getchar();
                #endif

                triangles.push_back({&p1, &p2, &p3});
            }
        }
    }

    for(size_t i=0; i < triangles.size(); ++i){
        DatapointTriangle& tri = triangles[i];
        drawInterpolatedTriangle(heatmaps[heatmapIdx], tri.d1->x, tri.d1->y, tri.d2->x, tri.d2->y, tri.d3->x, tri.d3->y,
        rssiToColorComponent(tri.d1->rssi[heatmapIdx]), rssiToColorComponent(tri.d2->rssi[heatmapIdx]), rssiToColorComponent(tri.d3->rssi[heatmapIdx]));
    }
}

static bool showHeatmap = false;
ErrCode toggleHeatmap(void* buttonPtr)noexcept{
    Button* button = (Button*)buttonPtr;
    showHeatmap = !showHeatmap;
    if(showHeatmap) button->text = "Heatmap";
    else button->text = "Datenpunkte";
    return SUCCESS;
}

ErrCode generateHeatmap(void* heatmapImages)noexcept{
    Image* images = (Image*)heatmapImages;
    for(BYTE i=0; i < HEATMAPCOUNT; ++i){
        for(DWORD j=0; j < images[i].width*images[i].height; ++j) images[i].data[j] = 0;
        DWORD count = sizeHashmap(datapoints);
        Datapoint* points = new Datapoint[count];
        void* pointsPointer[count];
        getAllElementsHashmap(datapoints, pointsPointer);
        for(DWORD i=0; i < count; ++i) points[i] = *(Datapoint*)pointsPointer[i];
        interpolateTriangulation(images, i, points, count);
        delete[] points;
    }
    return SUCCESS;
}

static bool differenceMode = false;
ErrCode toggleDifferenceMode(void* buttonPtr)noexcept{
    Button* button = (Button*)buttonPtr;
    differenceMode = !differenceMode;
    if(!differenceMode) button->text = "Einzeln";
    else button->text = "Zusammen";
    return SUCCESS;
}

static bool weightingQuality = true;
ErrCode toggleWeightingQuality(void* buttonPtr)noexcept{
    Button* button = (Button*)buttonPtr;
    weightingQuality = !weightingQuality;
    if(weightingQuality) button->text = "Gewichtung an";
    else button->text = "Gewichtung aus";
    return SUCCESS;
}

static BYTE showHeatmapIdx = 0;
ErrCode iterateHeatmaps(void* buttonPtr)noexcept{
    Button* button = (Button*)buttonPtr;
    ++showHeatmapIdx;
    if(showHeatmapIdx >= HEATMAPCOUNT) showHeatmapIdx = 0;
    button->text = "Heatmap " + std::to_string((WORD)showHeatmapIdx);
    return SUCCESS;
}

ErrCode clearHeatmaps(void*)noexcept{
    switch(mode){
        case HEATMAPMODE:
            clearHashmap(datapoints);
            break;
        case DISPLAYMODE:
            for(BYTE i=0; i < HEATMAPCOUNT; ++i) rssiData[i].clear();
            singleRssiData.clear();
            break;
    }
    return SUCCESS;
}

/// @brief Speichert die globalen Datenpunkte in einer .hmap Datei, die nach dem aktuellen Zeitstempel benannt wird
/// @param -
/// @return ErrCode
ErrCode saveHeatmaps(void*)noexcept{
    OPENFILENAME ofn = {};
    ofn.lStructSize = sizeof(OPENFILENAME);
    // ofn.hwndOwner = window.handle;
    BYTE fileDir[MAX_PATH]{};
    ofn.lpstrFile = (LPSTR)fileDir;
    ofn.nMaxFile = sizeof(fileDir);
    ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST;
    char currentDir[MAX_PATH]{0};
    DWORD directoryLength = GetCurrentDirectoryA(MAX_PATH, currentDir);
    if(directoryLength == 0) return OPEN_FILE;
    currentDir[directoryLength] = '\\';
    ofn.lpstrInitialDir = currentDir;
    ofn.lpstrFilter = "Heatmap .hmap\0*.hmap\0";
    ofn.nFilterIndex = 1;
    ofn.lpstrDefExt = "hmap";
    if(GetSaveFileName(&ofn) != TRUE) return SUCCESS;

    std::fstream file;
    file.open(ofn.lpstrFile, std::ios::out);
    if(!file.is_open()) return OPEN_FILE;

    DWORD count = sizeHashmap(datapoints);
    Datapoint* points = new Datapoint[count];
    void* pointsPointer[count];
    getAllElementsHashmap(datapoints, pointsPointer);
    for(DWORD i=0; i < count; ++i) points[i] = *(Datapoint*)pointsPointer[i];

    file << count << '\n';
    for(DWORD i=0; i < count; ++i){
        file << points[i].x << ' ';
        file << points[i].y << ' ';
        for(BYTE j=0; j < HEATMAPCOUNT; ++j) file << (int)points[i].rssi[j] << ' ';
        file << '\n';
    }
    file.close();
    delete[] points;
    return SUCCESS;
}

/// @brief Öffnet eine .hmap Datei und speichert die gelesenen Datenpunkte in die globalen Datenpunkte
/// @param -
/// @return ErrCode
ErrCode loadHeatmaps(void*)noexcept{
    OPENFILENAME ofn = {};
    ofn.lStructSize = sizeof(OPENFILENAME);
    // ofn.hwndOwner = window.handle;
    BYTE fileDir[MAX_PATH]{};
    ofn.lpstrFile = (LPSTR)fileDir;
    ofn.nMaxFile = sizeof(fileDir);
    ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST;
    char currentDir[MAX_PATH]{0};
    DWORD directoryLength = GetCurrentDirectoryA(MAX_PATH, currentDir);
    if(directoryLength == 0) return OPEN_FILE;
    currentDir[directoryLength] = '\\';
    ofn.lpstrInitialDir = currentDir;
    ofn.lpstrFilter = "Heatmap .hmap\0*.hmap\0";
    ofn.nFilterIndex = 1;
    if(GetOpenFileName(&ofn) != TRUE) return SUCCESS;

    std::fstream file;
    file.open(ofn.lpstrFile, std::ios::in);
    if(!file.is_open()) return OPEN_FILE;
    DWORD count;
    file >> count;
    for(DWORD i=0; i < count; ++i){
        Datapoint* point = new Datapoint;
        file >> point->x;
        file >> point->y;
        for(BYTE j=0; j < HEATMAPCOUNT; ++j){
            int val;
            file >> val;
            point->rssi[j] = val;
        }
        insertHashmap(datapoints, coordinatesToKey(point->x, point->y), point);
    }
    file.close();
    return SUCCESS;
}

static int searchRadius = 1;
ErrCode incSearchRadius(void*)noexcept{
    searchRadius++;
    return SUCCESS;
}
ErrCode decSearchRadius(void*)noexcept{
    searchRadius--;
    if(searchRadius < 1) searchRadius = 1;
    return SUCCESS;
}

ErrCode requestScan(void*)noexcept{
    WORD scanCount = 600;
    char* buffer = (char*)&scanCount;
    if(sendMessagecodeUDPServer(mainServer, REQUEST_SCANS, buffer, 2) <= 0){
        std::cerr << WSAGetLastError() << std::endl;
        return GENERIC_ERROR;
    }
    return SUCCESS;
}

ErrCode sendRouterName(void*)noexcept{
    if(routerInput.text.size() < 1) return SUCCESS;
    char buffer[routerInput.text.size()];
    if(sendMessagecodeUDPServer(mainServer, ADD_ROUTER, routerInput.text.c_str(), routerInput.text.size()) < 1) return GENERIC_ERROR;
    routerInput.text.clear();
    return SUCCESS;
}

ErrCode resetRouters(void*)noexcept{
    if(sendMessagecodeUDPServer(mainServer, RESET_ROUTERS, nullptr, 0) < 1) return GENERIC_ERROR;
    return SUCCESS;
}

ErrCode setEspIP(void*)noexcept{
    if(ipInput.text.size() < 1) return SUCCESS;
    changeUDPServerDestination(mainServer, ipInput.text.c_str(), 4984);
    ipInput.text.clear();
    return SUCCESS;
}

static bool simulateRSSI = false;
ErrCode toggleSimulation(void* buttonPtr)noexcept{
    Button* button = (Button*)buttonPtr;
    simulateRSSI = !simulateRSSI;
    if(simulateRSSI) button->text = "Simulation an";
    else button->text = "Simulation aus";
    return SUCCESS;
}

enum SEARCHMETHOD{
    MAXIMUM,
    CLUSTER,
    END
};

BYTE searchMethod = SEARCHMETHOD::MAXIMUM;
ErrCode setSearchMethod(void* buttonPtr)noexcept{
    Button* button = (Button*)buttonPtr;
    ++searchMethod;
    switch(searchMethod){
        case SEARCHMETHOD::END:
            searchMethod = SEARCHMETHOD::MAXIMUM;
        case SEARCHMETHOD::MAXIMUM:
            button->text = "Max-Methode";
            break;
        case SEARCHMETHOD::CLUSTER:
            button->text = "Cluster-Methode";
            break;
    }
    return SUCCESS;
}

/// @brief Gibt eine Schätzung der Qualität einer Punktewolke der globalen Datenpunkte zurück
// TODO geht bestimmt besser
/// @param idx Der RSSI-Index der Datenpunkte
/// @return Zahl zwischen 1 (sehr gute Qualität) und 0 (sehr schlechte Qualität)
float getHeatmapQuality(BYTE idx)noexcept{
    if(sizeHashmap(datapoints) < 1) return 0;
    float avg = 0;
    BYTE minVal = 255;
    BYTE maxVal = 0;
    HashmapIterator iterator = {};
    iterator = iterateHashmap(datapoints, iterator);
    while(iterator.valid){
        BYTE val = ((Datapoint*)iterator.data)->rssi[idx]-MINDB;
        avg += val;
        if(val < minVal) minVal = val;
        if(val > maxVal) maxVal = val;
        iterator = iterateHashmap(datapoints, iterator);
    }
    avg /= sizeHashmap(datapoints);
    float midVal = (minVal + maxVal)/2.f;
    float diff;
    if(avg < midVal) diff = 1.f - (midVal-avg)/(midVal-minVal);
    else diff = 1.f - (avg-midVal)/(maxVal-midVal);
    return (1.f - (float)(maxVal-minVal)/(MAXDB-MINDB)) * diff;
}

struct DistanceMap{
    BYTE* distances[HEATMAPCOUNT];
};
void createDistanceMap(DistanceMap& map)noexcept{
    for(BYTE i=0; i < HEATMAPCOUNT; ++i){
        map.distances[i] = new BYTE[DATAPOINTRESOLUTIONX*DATAPOINTRESOLUTIONY]{0};
    }
}
void destroyDistanceMap(DistanceMap& map)noexcept{
    for(BYTE i=0; i < HEATMAPCOUNT; ++i) delete[] map.distances[i];
}

//Berechnet eine Distanzemap von einer Heatmap zur SearchColor am Index idx
void getDistanceMap(Image* heatmapsInterpolated, BYTE idx, DistanceMap& map){
    for(DWORD j=0; j < DATAPOINTRESOLUTIONX*DATAPOINTRESOLUTIONY; ++j){
        DWORD& pixelColor = heatmapsInterpolated[idx].data[j];
        int diffRed = R(pixelColor)-R(searchColor[idx]);
        map.distances[idx][j] += abs(diffRed);
    }
}

//Berechnet das Distanzbild von allen Heatmaps, falls differenceMode true ist, sonst nur von der Heatmap am Index heatmapIndex
void calculateDistanceImage(Image* heatmapsInterpolated, Image& distanceImage, BYTE heatmapIndex){
    DistanceMap map;
    createDistanceMap(map);
    float* weightedDistances = new float[DATAPOINTRESOLUTIONX*DATAPOINTRESOLUTIONY]{0};
    float maxDiff = 0;
    if(differenceMode){
        for(BYTE i=0; i < HEATMAPCOUNT; ++i) getDistanceMap(heatmapsInterpolated, i, map);
        for(BYTE i=0; i < HEATMAPCOUNT; ++i){
            float quality;
            weightingQuality ? quality = getHeatmapQuality(i) : quality = 1;
            for(DWORD j=0; j < DATAPOINTRESOLUTIONX*DATAPOINTRESOLUTIONY; ++j){
                weightedDistances[j] += map.distances[i][j]*quality;
                if(weightedDistances[j] > maxDiff) maxDiff = weightedDistances[j];
            }
        }
    }else{
        getDistanceMap(heatmapsInterpolated, heatmapIndex, map);
        float quality;
        weightingQuality ? quality = getHeatmapQuality(showHeatmapIdx) : quality = 1;
        for(DWORD i=0; i < DATAPOINTRESOLUTIONX*DATAPOINTRESOLUTIONY; ++i){
            weightedDistances[i] = map.distances[heatmapIndex][i]*quality;
            if(weightedDistances[i] > maxDiff) maxDiff = weightedDistances[i];
        }
    }
    for(DWORD i=0; i < DATAPOINTRESOLUTIONX*DATAPOINTRESOLUTIONY; ++i){
        if(maxDiff == 0) break;
        int val = (weightedDistances[i]*255*searchRadius)/maxDiff;
        val = clamp(val, 0, 255);
        distanceImage.data[i] = RGBA(255-val, (255-val)/2, 0);
    }
    delete[] weightedDistances;
    destroyDistanceMap(map);
}

void checkTile(Image& distanceImage, Image& bufferImage, std::vector<std::pair<WORD, WORD>>& cluster, WORD x, WORD y)noexcept{
    if(y >= distanceImage.height || x < 0 || x >= distanceImage.width) return;
    DWORD idx = y*distanceImage.width+x;
    if(bufferImage.data[idx] == 0){
        bufferImage.data[idx] = 1;
        if(R(distanceImage.data[idx]) > 0){
            cluster.push_back({x, y});
            checkTile(distanceImage, bufferImage, cluster, x+1, y);
            checkTile(distanceImage, bufferImage, cluster, x-1, y);
            checkTile(distanceImage, bufferImage, cluster, x, y+1);
        }
    }
    return;
}

constexpr DWORD colorPicker(BYTE idx)noexcept{
    switch(idx%8){
        case 0: return RGBA(255, 255, 255);
        case 1: return RGBA(255, 0, 0);
        case 2: return RGBA(0, 255, 0);
        case 3: return RGBA(0, 0, 255);
        case 4: return RGBA(128, 128, 128);
        case 5: return RGBA(255, 255, 0);
        case 6: return RGBA(0, 255, 255);
        case 7: return RGBA(255, 0, 255);
    }
    return RGBA(0, 0, 0);
}

ScreenVec findCluster(Image& distanceImage, float threshold){
    BYTE min = 0xFF;
    BYTE max = 0;
    for(DWORD i=0; i < distanceImage.height*distanceImage.width; ++i){
        BYTE value = R(distanceImage.data[i]);
        if(value < min && value != 0) min = value;
        if(value > max) max = value;
    }
    BYTE cutoff = (max-min)*threshold+min;
    for(DWORD i=0; i < distanceImage.height*distanceImage.width; ++i){
        if(R(distanceImage.data[i]) < cutoff) distanceImage.data[i] = RGBA(0, 0, 0);
    }
    Image bufferImage;
    std::vector<std::vector<std::pair<WORD, WORD>>> clusters;
    createImage(bufferImage, distanceImage.width, distanceImage.height);
    for(DWORD i=0; i < bufferImage.width*bufferImage.height; ++i) bufferImage.data[i] = 0;
    for(WORD y=0; y < bufferImage.height; ++y){
        for(WORD x=0; x < bufferImage.width; ++x){
            if(bufferImage.data[y*bufferImage.width+x] == 0){
                if(R(distanceImage.data[y*bufferImage.width+x]) > 0){
                    std::vector<std::pair<WORD, WORD>> cluster;
                    checkTile(distanceImage, bufferImage, cluster, x, y);
                    clusters.push_back(cluster);
                }
            }
        }
    }
    destroyImage(bufferImage);
    DWORD maxCluster = 0;
    WORD maxClusterIndex = 0xFFFF;
    for(DWORD i=0; i < clusters.size(); ++i){
        std::vector<std::pair<WORD, WORD>>& cluster = clusters[i];
        DWORD color = colorPicker(i);
        if(cluster.size() > maxCluster){
            maxCluster = cluster.size();
            maxClusterIndex = i;
        }
        for(DWORD j=0; j < cluster.size(); ++j){
            std::pair<WORD, WORD>& coord = cluster[j];
            distanceImage.data[coord.second*distanceImage.width+coord.first] = color;
        }
    }
    QWORD x = 0;
    QWORD y = 0;
    if(maxClusterIndex != 0xFFFF){
        std::vector<std::pair<WORD, WORD>>& cluster = clusters[maxClusterIndex];
        for(DWORD i=0; i < cluster.size(); ++i){
            x += cluster[i].first;
            y += cluster[i].second;
        }
        x = (x*(window.windowWidth-200))/distanceImage.width/cluster.size();
        y = (y*window.windowHeight)/distanceImage.height/cluster.size();
    }

    return {(WORD)x, (WORD)y};
}

ScreenVec calculateFinalPosition(Image& distanceImage){
    std::vector<ScreenVec> positions;
    BYTE maxVal = 0;
    for(WORD y=0; y < distanceImage.height; ++y){
        for(WORD x=0; x < distanceImage.width; ++x){
            BYTE val = R(distanceImage.data[y*distanceImage.width+x]);
            if(val > maxVal){
                maxVal = val;
                positions.clear();
                positions.push_back({x, y});
            }
            else if(val == maxVal) positions.push_back({x, y});
        }
    }
    QWORD x = 0;
    QWORD y = 0;
    for(size_t i=0; i < positions.size(); ++i){
        x += positions[i].x;
        y += positions[i].y;
    }
    return {(WORD)((x*(window.windowWidth-200))/distanceImage.width/positions.size()), (WORD)((y*window.windowHeight)/distanceImage.height/positions.size())};
}

ErrCode changeMode(void* buttonPtr)noexcept{
    Button* button = (Button*)buttonPtr;
    ++mode;
    switch(mode){
        case ENDOFMODES:
            mode = HEATMAPMODE;
        case HEATMAPMODE:{
            button->text = "Heatmap-Mode";
            ScreenVec buttonSize = {180, 60};
            ScreenVec buttonPos = {10, (WORD)(buttons[0].pos.y+buttons[0].size.y+buttonSize.y*0.125)};
            buttons[1].pos = buttonPos;
            buttons[1].size = buttonSize;
            buttons[1].text = "Datenpunkte";
            buttons[1].color = RGBA(120, 120, 120);
            buttons[1].event = toggleHeatmap;
            buttons[1].data = &buttons[1];
            buttons[1].textsize = 30;
            buttonPos.y += buttonSize.y+buttonSize.y*0.125;
            buttons[2].pos = buttonPos;
            buttons[2].size = buttonSize;
            buttons[2].color = RGBA(120, 120, 120);
            buttons[2].text = "Heatmap gen.";
            buttons[2].event = generateHeatmap;
            buttons[2].data = heatmapsInterpolated;
            buttons[2].textsize = 26;
            buttonPos.y += buttonSize.y+buttonSize.y*0.125;
            buttons[3].pos = buttonPos;
            buttons[3].size = buttonSize;
            buttons[3].color = RGBA(120, 120, 120);
            buttons[3].text = "Loeschen";
            buttons[3].event = clearHeatmaps;
            buttons[3].textsize = 32;
            buttonPos.y += buttonSize.y+buttonSize.y*0.125;
            buttons[4].pos = buttonPos;
            buttons[4].size = buttonSize;
            buttons[4].color = RGBA(120, 120, 120);
            buttons[4].text = "Speichern";
            buttons[4].event = saveHeatmaps;
            buttons[4].textsize = 32;
            buttonPos.y += buttonSize.y+buttonSize.y*0.125;
            buttons[5].pos = buttonPos;
            buttons[5].size = buttonSize;
            buttons[5].color = RGBA(120, 120, 120);
            buttons[5].text = "Laden";
            buttons[5].event = loadHeatmaps;
            buttons[5].textsize = 32;
            buttonPos.y += buttonSize.y+buttonSize.y*0.125;
            buttons[6].pos = buttonPos;
            buttons[6].size = buttonSize;
            buttons[6].color = RGBA(120, 120, 120);
            buttons[6].text = "Heatmap 0";
            buttons[6].event = iterateHeatmaps;
            buttons[6].data = &buttons[6];
            buttons[6].textsize = 32;
            buttonPos.y += buttonSize.y+buttonSize.y*0.125;
            buttons[7].pos = buttonPos;
            buttons[7].size = buttonSize;
            buttons[7].color = RGBA(0, 140, 40);
            buttons[7].text = "RSSI Anfrage";
            buttons[7].event = requestScan;
            buttons[7].data = &buttons[7];
            buttons[7].textsize = 26;
            buttonPos.y += buttonSize.y+buttonSize.y*0.125;
            inputs[0].pos = buttonPos;
            inputs[0].size = buttonSize;
            inputs[0].backgroundText = "SSID hinzu.";
            inputs[0].textSize = 28;
            inputs[0].event = sendRouterName;
            setTextInputFlag(inputs[0], TEXTCENTERED);
            buttonPos.y += buttonSize.y+buttonSize.y*0.125;
            inputs[1].pos = buttonPos;
            inputs[1].size = buttonSize;
            inputs[1].backgroundText = "Esp32 IP";
            inputs[1].textSize = 28;
            inputs[1].event = setEspIP;
            setTextInputFlag(inputs[1], TEXTCENTERED);
            buttonPos.y += buttonSize.y+buttonSize.y*0.125;
            buttons[8].pos = buttonPos;
            buttons[8].size = buttonSize;
            buttons[8].color = RGBA(120, 120, 120);
            buttons[8].text = "Reset Router";
            buttons[8].event = resetRouters;
            buttons[8].textsize = 24;
            buttonCount = 9;
            inputCount = 2;
            break;
        }
        case SEARCHMODE:{
            button->text = "Search-Mode";
            ScreenVec buttonSize = {180, 60};
            ScreenVec buttonPos = {10, (WORD)(buttons[0].pos.y+buttons[0].size.y+buttonSize.y*0.125)};
            buttons[1].pos = buttonPos;
            buttons[1].size = buttonSize;
            buttons[1].text = "Heatmap 0";
            buttons[1].color = RGBA(120, 120, 120);
            buttons[1].event = iterateHeatmaps;
            buttons[1].data = &buttons[1];
            buttons[1].textsize = 32;
            buttonPos.y += buttonSize.y+buttonSize.y*0.125;
            buttons[2].pos = buttonPos;
            buttons[2].size = {buttonSize.y, buttonSize.y};
            buttons[2].text = "+";
            buttons[2].color = RGBA(120, 120, 120);
            buttons[2].event = incSearchRadius;
            buttons[2].textsize = 24;
            buttons[3].pos = {(WORD)(buttonPos.x+buttonSize.y+buttonSize.y*0.125), buttonPos.y};
            buttons[3].size = {buttonSize.y, buttonSize.y};
            buttons[3].color = RGBA(120, 120, 120);
            buttons[3].text = "-";
            buttons[3].event = decSearchRadius;
            buttons[3].textsize = 24;
            buttonPos.y += buttonSize.y+buttonSize.y*0.125;
            buttons[4].pos = buttonPos;
            buttons[4].size = buttonSize;
            buttons[4].text = "Einzeln";
            buttons[4].color = RGBA(120, 120, 120);
            buttons[4].event = toggleDifferenceMode;
            buttons[4].data = &buttons[4];
            buttons[4].textsize = 32;
            buttonPos.y += buttonSize.y+buttonSize.y*0.125;
            buttons[5].pos = buttonPos;
            buttons[5].size = buttonSize;
            buttons[5].color = RGBA(120, 120, 120);
            buttons[5].text = "Gewichtung an";
            buttons[5].event = toggleWeightingQuality;
            buttons[5].data = &buttons[5];
            buttons[5].textsize = 24;
            buttonPos.y += buttonSize.y+buttonSize.y*0.125;
            buttons[6].pos = buttonPos;
            buttons[6].size = buttonSize;
            buttons[6].color = RGBA(0, 140, 40);
            buttons[6].text = "RSSI Anfrage";
            buttons[6].event = requestScan;
            buttons[6].data = &buttons[6];
            buttons[6].textsize = 26;
            buttonPos.y += buttonSize.y+buttonSize.y*0.125;
            buttons[7].pos = buttonPos;
            buttons[7].size = buttonSize;
            buttons[7].color = RGBA(120, 120, 120);
            buttons[7].text = "Max-Methode";
            buttons[7].event = setSearchMethod;
            buttons[7].data = &buttons[7];
            buttons[7].textsize = 23;
            buttonPos.y += buttonSize.y+buttonSize.y*0.125;
            buttons[8].pos = buttonPos;
            buttons[8].size = buttonSize;
            buttons[8].color = RGBA(120, 120, 120);
            buttons[8].text = "Simulation aus";
            buttons[8].event = toggleSimulation;
            buttons[8].data = &buttons[8];
            buttons[8].textsize = 22;
            buttonPos.y += buttonSize.y+buttonSize.y*0.125;
            buttons[9].pos = buttonPos;
            buttons[9].size = buttonSize;
            buttons[9].color = RGBA(120, 120, 120);
            buttons[9].text = "Reset Router";
            buttons[9].event = resetRouters;
            buttons[9].textsize = 26;
            buttonCount = 10;
            inputCount = 0;
            break;
        }
        case DISPLAYMODE:
            button->text = "Display-Mode";
            ScreenVec buttonSize = {180, 60};
            ScreenVec buttonPos = {10, (WORD)(buttons[0].pos.y+buttons[0].size.y+buttonSize.y*0.125)};
            buttons[1].pos = buttonPos;
            buttons[1].size = buttonSize;
            buttons[1].text = "Heatmap 0";
            buttons[1].color = RGBA(120, 120, 120);
            buttons[1].event = iterateHeatmaps;
            buttons[1].data = &buttons[1];
            buttons[1].textsize = 32;
            buttonPos.y += buttonSize.y+buttonSize.y*0.125;
            buttons[2].pos = buttonPos;
            buttons[2].size = buttonSize;
            buttons[2].color = RGBA(0, 140, 40);
            buttons[2].text = "RSSI Anfrage";
            buttons[2].event = requestScan;
            buttons[2].data = &buttons[2];
            buttons[2].textsize = 26;
            buttonPos.y += buttonSize.y+buttonSize.y*0.125;
            buttons[3].pos = buttonPos;
            buttons[3].size = buttonSize;
            buttons[3].color = RGBA(120, 120, 120);
            buttons[3].text = "Reset Router";
            buttons[3].event = resetRouters;
            buttons[3].textsize = 26;
            buttonCount = 4;
            inputCount = 0;
            break;
    }
    return SUCCESS;
}

INT WinMain(HINSTANCE hInstance, HINSTANCE hPreviousInst, LPSTR lpszCmdLine, int nCmdShow){    
    WSADATA wsaData;
    if(WSAStartup(MAKEWORD(2, 2), &wsaData) != 0){
        return ErrCheck(GENERIC_ERROR, "WSAstartup");
    }
    
    if(ErrCheck(createHashmap(datapoints), "Hashmap der Datenpunkte anlegen") != SUCCESS) return -1;
    if(ErrCheck(createUDPServer(mainServer, 4984), "Main UDP Server erstellen") != SUCCESS) return -1;
    changeUDPServerDestination(mainServer, "192.168.137.34", 4984);
    RECT workArea;
    SystemParametersInfoA(SPI_GETWORKAREA, 0, &workArea, 0);
    int winHeight = workArea.bottom-workArea.top-(GetSystemMetrics(SM_CYFRAME) + GetSystemMetrics(SM_CYCAPTION) + GetSystemMetrics(SM_CXPADDEDBORDER));
    int winWidth = workArea.right-workArea.left;
    if(ErrCheck(createWindow(window, hInstance, winHeight+200, winHeight, winWidth-(winHeight+210), 0, 1, "Fenster", mainWindowCallback), "Fenster erstellen") != SUCCESS) return -1;
    if(ErrCheck(init(), "Init OpenGL") != SUCCESS) return -1;
    
    if(ErrCheck(loadTTF(font, "fonts/OpenSans-Bold.ttf"), "Font laden") != SUCCESS) return -1;

    Image floorplan;
    if(ErrCheck(loadImage("images/layout.tex", floorplan), "Layout laden") != SUCCESS) return -1;
    std::cout << floorplan.width << ", " << floorplan.height << std::endl;
    for(DWORD i=0; i < floorplan.width*floorplan.height; ++i){
        DWORD color =  floorplan.data[i];
        floorplan.data[i] = RGBA(R(color)*4, G(color)*4, B(color)*4, 65);   //Macht das Bild heller und durchsichtig
    }

    for(BYTE i=0; i < HEATMAPCOUNT; ++i) createImage(heatmapsInterpolated[i], DATAPOINTRESOLUTIONX, DATAPOINTRESOLUTIONY);

    std::thread getStrengthThread(processNetworkPackets);

    ScreenVec buttonSize = {180, 60};
    ScreenVec buttonPos = {10, 70};
    buttons[0].pos = buttonPos;
    buttons[0].size = buttonSize;
    buttons[0].event = changeMode;
    buttons[0].data = &buttons[0];
    buttons[0].textsize = 26;
    mode = -1;
    changeMode(&buttons[0]);

    Image distanceImage;
    createImage(distanceImage, DATAPOINTRESOLUTIONX, DATAPOINTRESOLUTIONY);

    Timer timer;
    DWORD deltaTime = 1;

    Slider<BYTE> valueSimulationSliders[HEATMAPCOUNT];
    WORD y = 20;
    for(BYTE i=0; i < HEATMAPCOUNT; ++i){
        valueSimulationSliders[i].pos = {220, y};
        valueSimulationSliders[i].size = {400, 5};
        valueSimulationSliders[i].silderRadius = 12;
        valueSimulationSliders[i].textSize = 24;
        valueSimulationSliders[i].minValue = MINDB;
        valueSimulationSliders[i].value = valueSimulationSliders[i].minValue;
        valueSimulationSliders[i].maxValue = MAXDB;
        y += valueSimulationSliders[i].silderRadius*2+10;
    }

    Slider<float> thresholdSilder;
    thresholdSilder.pos = {220, y};
    thresholdSilder.size = {400, 5};
    thresholdSilder.silderRadius = 12;
    thresholdSilder.textSize = 24;
    thresholdSilder.minValue = 0;
    thresholdSilder.maxValue = 1;

    Slider<WORD> displayHistoryLength;
    displayHistoryLength.pos = {220, y};
    displayHistoryLength.size = {800, 5};
    displayHistoryLength.silderRadius = 12;
    displayHistoryLength.textSize = 24;
    displayHistoryLength.minValue = 100;
    displayHistoryLength.maxValue = 20000;
    displayHistoryLength.value = displayHistoryLength.minValue;

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    while(running){
        resetTimer(timer);

        getMessages(window);
        updateButtons(window, font, rectangles, chars, buttons, buttonCount);
        for(WORD i=0; i < inputCount; ++i) updateTextInput(window, inputs[i], font, rectangles, chars);
        clearWindow(window);

        switch(mode){
            case SEARCHMODE:{
                calculateDistanceImage(heatmapsInterpolated, distanceImage, showHeatmapIdx);
                if(simulateRSSI){
                    for(BYTE i=0; i < HEATMAPCOUNT; ++i){
                        updateSlider(window, valueSimulationSliders[i], font, rectangles, circles, chars);
                    }
                    for(BYTE i=0; i < HEATMAPCOUNT; ++i){
                        BYTE value = rssiToColorComponent(valueSimulationSliders[i].value);
                        searchColor[i] = RGBA(value, 255-value, 1);
                    }
                }
                updateSlider(window, thresholdSilder, font, rectangles, circles, chars);
                ScreenVec position = (searchMethod == SEARCHMETHOD::MAXIMUM ? calculateFinalPosition(distanceImage) : findCluster(distanceImage, thresholdSilder.value));
                drawImage(window, distanceImage, 200, 0, window.windowWidth, window.windowHeight);
                drawImage(window, floorplan, 200, 0, window.windowWidth, window.windowHeight);
                circles.push_back({(WORD)(position.x+200), position.y, 5, 0, RGBA(255, 255, 255)});
                break;
            }
            case HEATMAPMODE:{
                if(showHeatmap) drawImage(window, heatmapsInterpolated[showHeatmapIdx], 200, 0, window.windowWidth, window.windowHeight);
                else{
                    Image dataPointsImage;
                    createImage(dataPointsImage, DATAPOINTRESOLUTIONX, DATAPOINTRESOLUTIONY);
                    for(DWORD i=0; i < DATAPOINTRESOLUTIONX*DATAPOINTRESOLUTIONY; ++i) dataPointsImage.data[i] = 0;
                    HashmapIterator iterator = {};
                    iterator = iterateHashmap(datapoints, iterator);
                    while(iterator.valid){
                        Datapoint& dataPoint = *(Datapoint*)(iterator.data);
                        iterator = iterateHashmap(datapoints, iterator);
                        BYTE color = dataPoint.rssi[showHeatmapIdx];
                        color = rssiToColorComponent(color);
                        dataPointsImage.data[dataPoint.y*DATAPOINTRESOLUTIONX+dataPoint.x] = RGBA(color, 255-color, 0);
                    }
                    drawImage(window, dataPointsImage, 200, 0, window.windowWidth, window.windowHeight);
                    destroyImage(dataPointsImage);
                }
                WORD tileSizeX = (window.windowWidth-200)/DATAPOINTRESOLUTIONX;
                WORD tileSizeY = window.windowHeight/DATAPOINTRESOLUTIONY;
                if(blink%32 < 8) rectangles.push_back({(WORD)((window.windowWidth-200)*gx/DATAPOINTRESOLUTIONX+200), (WORD)(window.windowHeight*gy/DATAPOINTRESOLUTIONY), tileSizeX, tileSizeY, RGBA(0, 0, 255)});
                blink++;
                drawImage(window, floorplan, 200, 0, window.windowWidth, window.windowHeight);
                for(BYTE i=0; i < routerPositionsCount; ++i){
                    circles.push_back({(WORD)routerPositions[i].x, (WORD)routerPositions[i].y, 8, 0, RGBA(0, 192, 255)});
                }
                break;
            }
            #define DISTANCEFROMTOP window.windowHeight/2
            case DISPLAYMODE:{
                WORD valueCounter[MAXDB-MINDB]{0};
                for(SBYTE rssi : rssiData[showHeatmapIdx]) valueCounter[abs(rssi)-MINDB] += 1;
                WORD maxCount = 0;
                BYTE maxIdx = 0;
                for(WORD i=0; i < MAXDB-MINDB; ++i){
                    if(valueCounter[i] > maxCount){
                        maxCount = valueCounter[i];
                        maxIdx = i;
                    }
                }
                if(maxCount == 0) maxCount = 1;
                WORD posX = 200;
                WORD incX = (window.windowWidth-200)/(MAXDB-MINDB);
                for(WORD i=0; i < MAXDB-MINDB; ++i){
                    WORD height = (valueCounter[i]*(window.windowHeight-DISTANCEFROMTOP))/maxCount;
                    if(i == maxIdx){
                        const char* string = longToString(-(i+MINDB));
                        DWORD size = getFontStringSize(font, string);
                        drawFontString(window, font, chars, string, (WORD)(posX+incX*i+incX/2-size/2), (WORD)(window.windowHeight-height-font.pixelSize-incX/2.f));
                        lines.push_back({(WORD)(posX+incX*i+incX/2), window.windowHeight, (WORD)(posX+incX*i+incX/2), (WORD)(window.windowHeight-height), incX/2.f, RGBA(255, 40, 40)});
                    }else{
                        lines.push_back({(WORD)(posX+incX*i+incX/2), window.windowHeight, (WORD)(posX+incX*i+incX/2), (WORD)(window.windowHeight-height), incX/2.f, RGBA(255, 255, 255)});
                    }
                }
                drawFontString(window, font, chars, longToString(rssiData[showHeatmapIdx].size()), 220, 80);
                DWORD dist = drawFontString(window, font, chars, longToString(-MINDB), 200, 0)+20;
                drawFontString(window, font, chars, longToString(-MAXDB), 200, DISTANCEFROMTOP-font.pixelSize);
                drawFontString(window, font, chars, longToString(-(MAXDB-MINDB)/2-MINDB), 200, DISTANCEFROMTOP/2-font.pixelSize/2);
                if(rssiData[showHeatmapIdx].size() < 1) break;
                SBYTE rssi = rssiData[showHeatmapIdx][rssiData[showHeatmapIdx].size()-1];
                WORD y1 = (abs(rssi)-MINDB)*(window.windowHeight-DISTANCEFROMTOP)/(MAXDB-MINDB);
                // drawFontString(window, font, chars, longToString(rssi), 200+dist, y1);
                updateSlider(window, displayHistoryLength, font, rectangles, circles, chars);
                WORD count = displayHistoryLength.value;
                if(rssiData[showHeatmapIdx].size() < displayHistoryLength.value) count = rssiData[showHeatmapIdx].size();
                WORD x = 200+dist;
                WORD prevX = x;
                for(WORD i=1; i < count; ++i){
                    SBYTE rssi = abs(rssiData[showHeatmapIdx][rssiData[showHeatmapIdx].size()-1-i])-MINDB;
                    WORD y2 = rssi*(window.windowHeight-DISTANCEFROMTOP)/(MAXDB-MINDB);
                    x = 200+dist+(i*(window.windowWidth-200-dist))/count;
                    lines.push_back({prevX, y1, x, y2, 1, RGBA(0, 160, 255)});
                    y1 = y2;
                    prevX = x;
                }
                break;
            }
        }

        WORD imageX = (gx*heatmapsInterpolated[showHeatmapIdx].width)/DATAPOINTRESOLUTIONX;
        WORD imageY = (gy*heatmapsInterpolated[showHeatmapIdx].height)/DATAPOINTRESOLUTIONY;
        BYTE selectedStrength = colorComponentToRssi(R(heatmapsInterpolated[showHeatmapIdx].data[imageY*heatmapsInterpolated[showHeatmapIdx].width+imageX]));
        DWORD selectedStrengthStringoffset = drawFontString(window, font, chars, longToString(-selectedStrength), 10, 10);
        selectedStrengthStringoffset += drawFontString(window, font, chars, floatToString(getHeatmapQuality(showHeatmapIdx), 3).c_str(), 30+selectedStrengthStringoffset, 10);

        if(mode == SEARCHMODE){
            DWORD offset = drawFontString(window, font, chars, longToString(searchRadius), (buttons[2].pos.x+buttons[2].size.x+buttons[2].size.y*0.125), buttons[2].pos.y);
            buttons[3].pos = {(WORD)(buttons[2].pos.x+buttons[2].size.x+buttons[2].size.y*0.25+offset), buttons[2].pos.y};
        }

        if(getButton(mouse, MOUSE_LMB)){
            int x = mouse.x-200;
            int y = mouse.y;
            if(x >= 0){
                gx = (float)(x*DATAPOINTRESOLUTIONX)/(window.windowWidth-200);
                gy = (float)(y*DATAPOINTRESOLUTIONY)/(window.windowHeight);
                delete (Datapoint*)removeHashmap(datapoints, coordinatesToKey(gx, gy));
            }
        }
        if(getButton(mouse, MOUSE_RMB)){
            int x = mouse.x-200;
            int y = mouse.y;
            if(!getButton(mouse, MOUSE_PREV_RMB) && x >= 0){
                if(routerPositionsCount >= HEATMAPCOUNT) routerPositionsCount = 0;
                else routerPositions[routerPositionsCount++] = {mouse.x, mouse.y};
            }
            setButton(mouse, MOUSE_PREV_RMB);
        }else{
            resetButton(mouse, MOUSE_PREV_RMB);
        }
        
        std::string fpsTime = "FPS: ";
        WORD fps = deltaTime != 0 ? 1000000/deltaTime : 1000;
        fpsTime += longToString(fps);
        // drawFontString(window, font, chars, fpsTime.c_str(), 50+selectedStrengthStringoffset, 10);

        renderRectangles(window, rectangles.data(), rectangles.size());
        renderLines(window, lines.data(), lines.size());
        renderCircles(window, circles.data(), circles.size());
        renderFontChars(window, font, chars.data(), chars.size());
        drawWindow(window);
        lines.clear();
        rectangles.clear();
        circles.clear();
        chars.clear();
        if(getWindowFlag(window, WINDOW_CLOSE)) running = false;
        //TODO muss nicht ständig aufgerufen werden...
        if(!SetThreadExecutionState(ES_DISPLAY_REQUIRED | ES_SYSTEM_REQUIRED)) ErrCheck(GENERIC_ERROR, "Exectution State setzen");

        deltaTime = getTimerMicros(timer);
        DWORD timediff = deltaTime > 16000 ? 16000 : deltaTime;

        Sleep((16000-timediff)/1000);

        if(getButton(mouse, MOUSE_LMB)) setButton(mouse, MOUSE_PREV_LMB);
        else resetButton(mouse, MOUSE_PREV_LMB);
    }

    destroyImage(distanceImage);

    getStrengthThread.join();

    HashmapIterator iter = {};
    iter = iterateHashmap(datapoints, iter);
    while(iter.valid){
        delete (Datapoint*)iter.data;
        iter = iterateHashmap(datapoints, iter);
    }
    destroyHashmap(datapoints);

    destroyUDPServer(mainServer);
    WSACleanup();
    destroyFont(font);
    for(int i=0; i < HEATMAPCOUNT; ++i){
        destroyImage(heatmapsInterpolated[i]);
    }
    return 0;
}

LRESULT CALLBACK mainWindowCallback(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam){
	Window* window = (Window*)GetWindowLongPtr(hwnd, GWLP_USERDATA);
	if(window == nullptr || window->handle == NULL) return DefWindowProc(hwnd, uMsg, wParam, lParam);	//TODO das ist ein Fehler, wie melden aber?
	switch(uMsg){
		case WM_CLOSE:
		case WM_DESTROY:{
			ErrCheck(setWindowFlag(*window, WINDOW_CLOSE), "setze close Fensterstatus");
			return 0;
		}
		case WM_SIZE:{
			UINT width = LOWORD(lParam);
			UINT height = HIWORD(lParam);
			if(!width || !height) break;
			ErrCheck(setWindowFlag(*window, WINDOW_RESIZE), "setzte resize Fensterstatus");
			ErrCheck(resizeWindow(*window, width, height, 1), "Fenster skalieren");
			break;
		}
		case WM_LBUTTONDOWN:{
			setButton(mouse, MOUSE_LMB);
			break;
		}
		case WM_LBUTTONUP:{
			resetButton(mouse, MOUSE_LMB);
			break;
		}
		case WM_RBUTTONDOWN:{
			setButton(mouse, MOUSE_RMB);
			break;
		}
		case WM_RBUTTONUP:{
			resetButton(mouse, MOUSE_RMB);
			break;
		}
		case WM_MOUSEMOVE:{
            SetCursor(LoadCursor(NULL, IDC_ARROW));     //TODO Keine Ahnung ob das Performance mäßig ein Problem sein könnte
			mouse.x = GET_X_LPARAM(lParam);
			mouse.y = GET_Y_LPARAM(lParam);
			break;
		}
        case WM_CHAR:{
            // std::cout << wParam << std::endl;
            for(WORD i=0; i < sizeof(inputs)/sizeof(TextInput); ++i) textInputCharEvent(inputs[i], wParam);
        }
	}
	return DefWindowProc(hwnd, uMsg, wParam, lParam);
}
