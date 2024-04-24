#include "network.h"
#include "window.h"
#include "usb.h"
#include <thread>
#include <algorithm>

/*
    TODOS:
    Lineare Interpolation des Triangulationsalgorithmus zu einer logarithmischen umändern

    Eine Position als finale festlegen, anstatt nur eine "Heatmap" als Schätzung zu zeigen

    Routerdaten und IP an den esp32 senden können

    Datenpunkte RSSI Anzahl "dynamisch" machen
    Idee ist eine fest definierte Anzahl an RSSI-Werten auslesen zu wollen, sollte es zu wenige/viele geben,
    werden diese mit 0 befüllt, daher sollte jeder Datenpunkt die Werte + Anzahl speichern. In der Applikation
    kann man dann die Anzahl der RSSI Werte angeben. An die meisten Funktionen sollte dann ein Parameter
    übergeben werden, welches angibt, wie viele RSSI-Werte von den Datenpunkten ausgelesen werden sollen. Das
    Parameter sollte dann eine globale Variable sein, die zur Laufzeit verändert werden kann.

    Man sollte die Auflösung der Messpunkte festlegen können
*/

Window* window = nullptr;
Font* font = nullptr;
UDPServer mainServer;

//TODO Annahme Signalstärke von -20dB bis -90dB
#define MAXDB 90
#define MINDB 20

#define DATAPOINTRESOLUTIONX 20
#define DATAPOINTRESOLUTIONY 20
#define INTERPOLATEDHEATMAPX 800
#define INTERPOLATEDHEATMAPY 800
#define HEATMAPCOUNT 3

enum MODES{
    HEATMAPMODE,
    SEARCHMODE,
    ENDOFMODES
};

struct Datapoint{
    WORD x = 0;
    WORD y = 0;
    BYTE rssi[HEATMAPCOUNT];    //TODO BYTE* und zur Laufzeit allokieren und den max. Wert in rssiCount merken
    BYTE rssiCount = 0;
};
static Hashmap datapoints;
#define coordinatesToKey(x, y)((x<<16)|y)

static BYTE mode = 0;
static DWORD searchColor[HEATMAPCOUNT]{0};  //TODO auch zur Laufzeit allokieren und durch das Applikationsinterface veränderbar machen
static bool running = true;
static WORD gx = 0;
static WORD gy = 0;
static BYTE blink = 0;

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
                            case HEATMAPMODE: break;
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
    Datapoint scaledDatapoints[count];  //Speichert die Datenpunkte skaliert für die Interpolierung
    for(DWORD i=0; i < count; ++i){
        scaledDatapoints[i].x = (datapoints[i].x*INTERPOLATEDHEATMAPX)/DATAPOINTRESOLUTIONX;
        scaledDatapoints[i].y = (datapoints[i].y*INTERPOLATEDHEATMAPY)/DATAPOINTRESOLUTIONY;
        scaledDatapoints[i].rssi[heatmapIdx] = datapoints[i].rssi[heatmapIdx];
    }
    std::vector<DatapointTriangle> triangles;    //Hält alle Datenpunkt Dreiecke TODO kann man im vorab berechnen

    for(DWORD i=0; i < count; ++i){
        Datapoint& p1 = scaledDatapoints[i];
        for(DWORD j=0; j < count; ++j){
            if(j == i) continue;
            Datapoint& p2 = scaledDatapoints[j];
            for(DWORD k=0; k < count; ++k){
                if(k == j || k == i) continue;
                Datapoint& p3 = scaledDatapoints[k];
                bool valid = true;

                float totalArea = (p2.x-p1.x)*(p3.y-p2.y)-(p2.y-p1.y)*(p3.x-p2.x);
                if(totalArea <= 0) continue;

                float midXP1P2 = (p1.x+p2.x)/2;
                float midYP1P2 = (p1.y+p2.y)/2;
                float dyP1P2 = (p2.y-p1.y);
                float mP1P2;
                dyP1P2 != 0 ? mP1P2 = -(p2.x-p1.x)/dyP1P2 : mP1P2 = 1e6f;
                float bP1P2 = midYP1P2 - mP1P2*midXP1P2;

                float midXP1P3 = (p1.x+p3.x)/2;
                float midYP1P3 = (p1.y+p3.y)/2;
                float dyP1P3 = (p3.y-p1.y);
                float mP1P3;
                dyP1P3 != 0 ? mP1P3 = -(p3.x-p1.x)/dyP1P3 : mP1P3 = 1e6f;
                float bP1P3 = midYP1P3 - mP1P3*midXP1P3;

                float centerX = (bP1P3-bP1P2)/(mP1P2-mP1P3);
                float centerY = mP1P2*centerX+bP1P2;
                float radius2 = (centerX-p1.x)*(centerX-p1.x)+(centerY-p1.y)*(centerY-p1.y);
                for(DWORD l=0; l < count; ++l){
                    if(l == k || l == j || l == i) continue;
                    Datapoint& p = scaledDatapoints[l];
                    float distance2 = (centerX-p.x)*(centerX-p.x)+(centerY-p.y)*(centerY-p.y);
                    if(distance2 < radius2){
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

ErrCode toggleMode(void* buttonPtr)noexcept{
    Button* button = (Button*)buttonPtr;
    ++mode;
    if(mode >= ENDOFMODES) mode = 0;
    if(!mode) button->text = "Heatmap-Mode";
    else button->text = "Search-Mode";
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
    clearHashmap(datapoints);
    return SUCCESS;
}

/// @brief Speichert die globalen Datenpunkte in eine Datei, die nach dem aktuellen Zeitstempel benannt wird
/// @param -
/// @return ErrCode
ErrCode saveHeatmaps(void*)noexcept{
    std::fstream file;
    SYSTEMTIME timePoint;
    GetSystemTime(&timePoint);
    std::string filename;
    filename += longToString(timePoint.wDay);
    filename += '-';
    filename += longToString(timePoint.wMonth);
    filename += '-';
    filename += longToString(timePoint.wYear);
    filename += "--";
    filename += longToString(timePoint.wHour);
    filename += '-';
    filename += longToString(timePoint.wMinute);
    filename += '-';
    filename += longToString(timePoint.wSecond);
    filename += '.';
    filename += longToString(timePoint.wMilliseconds);
    std::cout << filename << std::endl;
    file.open(filename, std::ios::out);
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

/// @brief Lädt die Datei "heatmap" und speichert die gelesenen Datenpunkte in die globalen Datenpunkte
/// @param -
/// @return ErrCode
ErrCode loadHeatmaps(void*)noexcept{
    std::fstream file;
    std::string filename = "heatmap";
    file.open(filename, std::ios::in);
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

//TODO implementieren
ErrCode incRouterCount()noexcept{
    return SUCCESS;
}
ErrCode decRouterCount()noexcept{
    return SUCCESS;
}

//TODO Qualität berechnen kann bestimmt noch besser gehen

/// @brief Gibt eine Schätzung der Qualität einer Punktewolke der globalen Datenpunkte zurück
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
void createDistanceMap(DistanceMap& map){
    for(BYTE i=0; i < HEATMAPCOUNT; ++i){
        map.distances[i] = new BYTE[INTERPOLATEDHEATMAPX*INTERPOLATEDHEATMAPY]{0};
    }
}
void destroyDistanceMap(DistanceMap& map){
    for(BYTE i=0; i < HEATMAPCOUNT; ++i) delete[] map.distances[i];
}

//Berechnet eine Distanzemap von einer Heatmap zur SearchColor am Index idx
void getDistanceMap(Image* heatmapsInterpolated, BYTE idx, DistanceMap& map){
    for(DWORD j=0; j < INTERPOLATEDHEATMAPX*INTERPOLATEDHEATMAPY; ++j){
        DWORD& pixelColor = heatmapsInterpolated[idx].data[j];
        int diffRed = R(pixelColor)-R(searchColor[idx]);
        map.distances[idx][j] += abs(diffRed);
    }
}

//Berechnet das Distanzbild von allen Heatmaps, falls differenceMode true ist, sonst nur von der Heatmap am Index heatmapIndex
void calculateDistanceImage(Image* heatmapsInterpolated, Image& distanceImage, BYTE heatmapIndex){
    DistanceMap map;
    createDistanceMap(map);
    float* weightedDistances = new float[INTERPOLATEDHEATMAPX*INTERPOLATEDHEATMAPY]{0};
    float maxDiff = 0;
    if(differenceMode){
        for(BYTE i=0; i < HEATMAPCOUNT; ++i) getDistanceMap(heatmapsInterpolated, i, map);
        for(BYTE i=0; i < HEATMAPCOUNT; ++i){
            float quality;
            weightingQuality ? quality = getHeatmapQuality(i) : quality = 1;
            for(DWORD j=0; j < INTERPOLATEDHEATMAPX*INTERPOLATEDHEATMAPY; ++j){
                weightedDistances[j] += map.distances[i][j]*quality;
                if(weightedDistances[j] > maxDiff) maxDiff = weightedDistances[j];
            }
        }
    }else{
        getDistanceMap(heatmapsInterpolated, heatmapIndex, map);
        float quality;
        weightingQuality ? quality = getHeatmapQuality(showHeatmapIdx) : quality = 1;
        for(DWORD i=0; i < INTERPOLATEDHEATMAPX*INTERPOLATEDHEATMAPY; ++i){
            weightedDistances[i] = map.distances[heatmapIndex][i]*quality;
            if(weightedDistances[i] > maxDiff) maxDiff = weightedDistances[i];
        }
    }
    for(DWORD i=0; i < INTERPOLATEDHEATMAPX*INTERPOLATEDHEATMAPY; ++i){
        if(maxDiff == 0) break;
        float val = weightedDistances[i]*255/maxDiff;
        val *= searchRadius;
        val = clamp(val, 0, 255);
        distanceImage.data[i] = RGBA(255-val, (255-val)/2, 0);
    }
    delete[] weightedDistances;
    destroyDistanceMap(map);
}

INT WinMain(HINSTANCE hInstance, HINSTANCE hPreviousInst, LPSTR lpszCmdLine, int nCmdShow){    
    WSADATA wsaData;
    if(WSAStartup(MAKEWORD(2, 2), &wsaData) != 0){
        return ErrCheck(GENERIC_ERROR, "WSAstartup");
    }
    
    if(ErrCheck(createHashmap(datapoints), "Hashmap der Datenpunkte anlegen") != SUCCESS) return -1;
    if(ErrCheck(createUDPServer(mainServer, 4984), "Main UDP Server erstellen") != SUCCESS) return -1;
    if(ErrCheck(initApp(), "App init") != SUCCESS) return -1;
    if(ErrCheck(createWindow(hInstance, 1200, 1000, 300, 100, 1, window, "Fenster"), "Fenster erstellen") != SUCCESS) return -1;
    if(ErrCheck(assignAttributeBuffers(window, 1), "Attribute Buffer hinzufügen") != SUCCESS) return -1;
    
    if(ErrCheck(createFont(font), "Font erstellen") != SUCCESS) return -1;
    if(ErrCheck(loadFont("fonts/ascii.tex", *font, {82, 83}), "Font laden") != SUCCESS) return -1;
    font->font_size = 42/window->pixelSize;

    Image floorplan;
    if(ErrCheck(loadImage("images/layout.tex", floorplan), "Layout laden") != SUCCESS) return -1;
    for(DWORD i=0; i < floorplan.width*floorplan.height; ++i){
        floorplan.data[i] *= 4;     //Macht das Bild heller
    }

    Image heatmapsInterpolated[HEATMAPCOUNT];   //TODO dynamisch
    for(BYTE i=0; i < HEATMAPCOUNT; ++i){
        heatmapsInterpolated[i].width = INTERPOLATEDHEATMAPX;
        heatmapsInterpolated[i].height = INTERPOLATEDHEATMAPY;
        heatmapsInterpolated[i].data = new DWORD[INTERPOLATEDHEATMAPX*INTERPOLATEDHEATMAPY];
    }

    Button buttons[11];
    ivec2 buttonSize = {180/window->pixelSize, 60/window->pixelSize};
    ivec2 buttonPos = {10/window->pixelSize, 80/window->pixelSize};
    buttons[0].pos = buttonPos;
    buttons[0].size = buttonSize;
    buttons[0].text = "Datenpunkte";
    buttons[0].event = toggleHeatmap;
    buttons[0].data = &buttons[0];
    buttons[0].textsize = 30/window->pixelSize;
    buttonPos.y += buttonSize.y+buttonSize.y*0.125/window->pixelSize;
    buttons[1].pos = buttonPos;
    buttons[1].size = buttonSize;
    buttons[1].text = "Heatmap gen.";
    buttons[1].event = generateHeatmap;
    buttons[1].data = heatmapsInterpolated;
    buttons[1].textsize = 26/window->pixelSize;
    buttonPos.y += buttonSize.y+buttonSize.y*0.125/window->pixelSize;
    buttons[2].pos = buttonPos;
    buttons[2].size = buttonSize;
    buttons[2].text = "Loeschen";
    buttons[2].event = clearHeatmaps;
    buttons[2].textsize = 32/window->pixelSize;
    buttonPos.y += buttonSize.y+buttonSize.y*0.125/window->pixelSize;
    buttons[3].pos = buttonPos;
    buttons[3].size = buttonSize;
    buttons[3].text = "Speichern";
    buttons[3].event = saveHeatmaps;
    buttons[3].textsize = 32/window->pixelSize;
    buttonPos.y += buttonSize.y+buttonSize.y*0.125/window->pixelSize;
    buttons[4].pos = buttonPos;
    buttons[4].size = buttonSize;
    buttons[4].text = "Laden";
    buttons[4].event = loadHeatmaps;
    buttons[4].textsize = 32/window->pixelSize;
    buttonPos.y += buttonSize.y+buttonSize.y*0.125/window->pixelSize;
    buttons[5].pos = buttonPos;
    buttons[5].size = buttonSize;
    buttons[5].text = "Heatmap 0";
    buttons[5].event = iterateHeatmaps;
    buttons[5].data = &buttons[5];
    buttons[5].textsize = 32/window->pixelSize;
    buttonPos.y += buttonSize.y+buttonSize.y*0.125/window->pixelSize;
    buttons[6].pos = buttonPos;
    buttons[6].size = buttonSize;
    buttons[6].text = "Heatmap-Mode";
    buttons[6].event = toggleMode;
    buttons[6].data = &buttons[6];
    buttons[6].textsize = 24/window->pixelSize;

    buttonPos.y += buttonSize.y+buttonSize.y*0.125/window->pixelSize;
    buttons[7].pos = buttonPos;
    buttons[7].size = {buttonSize.y, buttonSize.y};
    buttons[7].text = "+";
    buttons[7].event = incSearchRadius;
    buttons[7].textsize = 24/window->pixelSize;
    buttons[8].pos = {(int)(buttonPos.x+buttonSize.y+buttonSize.y*0.125/window->pixelSize), buttonPos.y};
    buttons[8].size = {buttonSize.y, buttonSize.y};
    buttons[8].text = "-";
    buttons[8].event = decSearchRadius;
    buttons[8].textsize = 24/window->pixelSize;

    buttonPos.y += buttonSize.y+buttonSize.y*0.125/window->pixelSize;
    buttons[9].pos = buttonPos;
    buttons[9].size = buttonSize;
    buttons[9].text = "Einzeln";
    buttons[9].event = toggleDifferenceMode;
    buttons[9].data = &buttons[9];
    buttons[9].textsize = 32/window->pixelSize;
    buttonPos.y += buttonSize.y+buttonSize.y*0.125/window->pixelSize;
    buttons[10].pos = buttonPos;
    buttons[10].size = buttonSize;
    buttons[10].text = "Gewichtung an";
    buttons[10].event = toggleWeightingQuality;
    buttons[10].data = &buttons[10];
    buttons[10].textsize = 24/window->pixelSize;

    std::thread getStrengthThread(processNetworkPackets);

    Image distanceImage;
    distanceImage.width = INTERPOLATEDHEATMAPX;
    distanceImage.height = INTERPOLATEDHEATMAPY;
    distanceImage.data = new DWORD[INTERPOLATEDHEATMAPX*INTERPOLATEDHEATMAPY];
    float* distances = new float[INTERPOLATEDHEATMAPX*INTERPOLATEDHEATMAPY];

    while(running){
        FILETIME ft;
        GetSystemTimeAsFileTime(&ft);
        QWORD startTime = ((QWORD)ft.dwHighDateTime<<32) | ft.dwLowDateTime;

        getMessages(window);
        updateButtons(window, *font, buttons, sizeof(buttons)/sizeof(Button));
        clearWindow(window);

        if(showHeatmap) copyImageToWindow(window, heatmapsInterpolated[showHeatmapIdx], 200/window->pixelSize, 0, window->windowWidth/window->pixelSize, window->windowHeight/window->pixelSize, 0.5);
        else{
            Image dataPointsImage;
            dataPointsImage.width = DATAPOINTRESOLUTIONX;
            dataPointsImage.height = DATAPOINTRESOLUTIONY;
            dataPointsImage.data = new DWORD[DATAPOINTRESOLUTIONX*DATAPOINTRESOLUTIONY]{0};
            HashmapIterator iterator = {};
            iterator = iterateHashmap(datapoints, iterator);
            while(iterator.valid){
                Datapoint& dataPoint = *(Datapoint*)(iterator.data);
                iterator = iterateHashmap(datapoints, iterator);
                BYTE color = dataPoint.rssi[showHeatmapIdx];
                color = rssiToColorComponent(color);
                dataPointsImage.data[dataPoint.y*DATAPOINTRESOLUTIONX+dataPoint.x] = RGBA(color, 255-color, 0);
            }
            copyImageToWindow(window, dataPointsImage, 200/window->pixelSize, 0, window->windowWidth/window->pixelSize, window->windowHeight/window->pixelSize);
            destroyImage(dataPointsImage);
        }

        if(mode == SEARCHMODE){
            calculateDistanceImage(heatmapsInterpolated, distanceImage, showHeatmapIdx);
            copyImageToWindow(window, distanceImage, 200/window->pixelSize, 0, window->windowWidth/window->pixelSize, window->windowHeight/window->pixelSize);
        }else{
            WORD tileSizeX = (window->windowWidth-200)/window->pixelSize/DATAPOINTRESOLUTIONX;
            WORD tileSizeY = window->windowHeight/window->pixelSize/DATAPOINTRESOLUTIONY;
            if(blink%32 < 8) drawRectangle(window, 200/window->pixelSize+gx*tileSizeX, gy*tileSizeY, tileSizeX, tileSizeY, RGBA(0, 0, 255));
            blink++;
        }

        copyImageToWindow(window, floorplan, 200/window->pixelSize, 0, window->windowWidth/window->pixelSize, window->windowHeight/window->pixelSize, 0.25);

        drawButtons(window, *font, buttons, sizeof(buttons)/sizeof(Button));
        int selectedStrength = 90;
        Datapoint* point = (Datapoint*)searchHashmap(datapoints, coordinatesToKey(gx, gy));
        if(point){
            selectedStrength = point->rssi[showHeatmapIdx];
        }
        DWORD offset = drawFontString(window, *font, longToString(-selectedStrength), 10/window->pixelSize, 10/window->pixelSize);
        drawFontString(window, *font, floatToString(getHeatmapQuality(showHeatmapIdx), 3).c_str(), 10/window->pixelSize+offset+16/window->pixelSize, 10/window->pixelSize);

        offset = drawFontString(window, *font, longToString(searchRadius), (int)(buttons[7].pos.x+buttons[7].size.x+buttonSize.y*0.125/window->pixelSize), buttons[7].pos.y);
        buttons[8].pos = {(int)(buttons[7].pos.x+buttons[7].size.x+buttonSize.y*0.25/window->pixelSize+offset), buttons[7].pos.y};

        if(getButton(mouse, MOUSE_LMB)){
            int x = mouse.pos.x-200/window->pixelSize;
            int y = mouse.pos.y;
            if(x >= 0){
                gx = (float)x/(window->windowWidth/window->pixelSize-200/window->pixelSize)*(DATAPOINTRESOLUTIONX);
                gy = (float)y/(window->windowHeight/window->pixelSize)*(DATAPOINTRESOLUTIONY);
                delete (Datapoint*)removeHashmap(datapoints, coordinatesToKey(gx, gy));
            }
        }
        

        drawWindow(window);
        if(getWindowFlag(window, WINDOW_CLOSE)) running = false;
        //TODO muss nicht ständig aufgerufen werden...
        if(!SetThreadExecutionState(ES_DISPLAY_REQUIRED | ES_SYSTEM_REQUIRED)) ErrCheck(GENERIC_ERROR, "Exectution State setzen");

        //TODO das ist so ziemlich immer > 16 -> render thread
        GetSystemTimeAsFileTime(&ft);
        DWORD timediff = ((((QWORD)ft.dwHighDateTime<<32) | ft.dwLowDateTime)-startTime)/10000;
        if(timediff > 16) timediff = 16;
        Sleep(16-timediff);
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
