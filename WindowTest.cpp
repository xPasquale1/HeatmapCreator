#include "windowgl.h"
#include "font.h"

std::vector<LineData> lines;
std::vector<CircleData> circles;

struct Point{
    SWORD x;
    SWORD y;
};

struct PointTriangle{
    Point p1;
    Point p2;
    Point p3;
};

bool pointInTriangle(SWORD x1, SWORD y1, SWORD x2, SWORD y2, SWORD x3, SWORD y3, SWORD x, SWORD y){
    float totalArea = 1.f/((x2-x1)*(y3-y2)-(y2-y1)*(x3-x2));
    float m1 = ((x2-x)*(y3-y)-(x3-x)*(y2-y))*totalArea;
    float m2 = ((x3-x)*(y1-y)-(x1-x)*(y3-y))*totalArea;
    if(m1 >= 0 && m2 >= 0 && (m1+m2) <= 1) return true;
    return false;
}

bool lineSegmentIntersection(SWORD xBeg1, SWORD yBeg1, SWORD xEnd1, SWORD yEnd1, SWORD xBeg2, SWORD yBeg2, SWORD xEnd2, SWORD yEnd2)noexcept{
    if(xBeg1 == xBeg2 && yBeg1 == yBeg2 && xEnd1 == xEnd2 && yEnd1 == yEnd2) return true;
    if(xBeg1 == xEnd2 && yBeg1 == yEnd2 && xEnd1 == xBeg2 && yEnd1 == yBeg2) return true;
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
        if(tx1 >= 0 && tx1 <= 1 && ty1 >= 0 && ty1 <= 1) return true;
        if(tx2 >= 0 && tx2 <= 1 && ty2 >= 0 && ty2 <= 1) return true;
        if(tx3 >= 0 && tx3 <= 1 && ty3 >= 0 && ty3 <= 1) return true;
        if(tx4 >= 0 && tx4 <= 1 && ty4 >= 0 && ty4 <= 1) return true;
        return false;
    }

    int dxBeg = xBeg2-xBeg1;
    int dyBeg = yBeg2-yBeg1;

    float t1 = (dy2*dxBeg-dx2*dyBeg)/(float)denom;
    float t2 = (dy1*dxBeg-dx1*dyBeg)/(float)denom;

    return (t1 >= 0 && t1 <= 1 && t2 >= 0 && t2 <= 1);
}

bool lineSegmentCrossing(SWORD xBeg1, SWORD yBeg1, SWORD xEnd1, SWORD yEnd1, SWORD xBeg2, SWORD yBeg2, SWORD xEnd2, SWORD yEnd2)noexcept{
    if(xBeg1 == xBeg2 && yBeg1 == yBeg2 && xEnd1 == xEnd2 && yEnd1 == yEnd2) return true;
    if(xBeg1 == xEnd2 && yBeg1 == yEnd2 && xEnd1 == xBeg2 && yEnd1 == yBeg2) return true;
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
        if(tx1 > 0 && tx1 < 1 && ty1 > 0 && ty1 < 1) return true;
        if(tx2 > 0 && tx2 < 1 && ty2 > 0 && ty2 < 1) return true;
        if(tx3 > 0 && tx3 < 1 && ty3 > 0 && ty3 < 1) return true;
        if(tx4 > 0 && tx4 < 1 && ty4 > 0 && ty4 < 1) return true;
        return false;
    }

    int dxBeg = xBeg2-xBeg1;
    int dyBeg = yBeg2-yBeg1;

    float t1 = (dy2*dxBeg-dx2*dyBeg)/(float)denom;
    float t2 = (dy1*dxBeg-dx1*dyBeg)/(float)denom;

    return (t1 > 0 && t1 < 1 && t2 > 0 && t2 < 1);
}

void addContour(WORD startIdx, WORD endIdx, std::vector<WORD>& indices, Window& window, WORD x, WORD y, Glyph& glyph){
    for(WORD i=startIdx; i <= endIdx; ++i){
        indices.push_back(i);
    }
    indices.push_back(startIdx);
}

struct Edge{
    SWORD x1;
    SWORD y1;
    SWORD x2;
    SWORD y2;
};

bool testEdges(Glyph& glyph, WORD contourIdx, WORD pointIdx, std::vector<Edge>& bridges, Window& window, WORD x, WORD y){
    Glyphpoint& point = glyph.points[glyph.endOfContours[contourIdx-1]+1];      //Anfangsindex der zu testenden Lochkontur TODO sollte vllt mal alle Punkte testen
    WORD pointIdxHole = glyph.endOfContours[contourIdx-1]+1;
    SWORD bridgeDirX = point.x-glyph.points[pointIdx].x;
    SWORD bridgeDirY = point.y-glyph.points[pointIdx].y;
    SWORD prevDirX = glyph.points[pointIdx-1].x-glyph.points[pointIdx].x;
    SWORD prevDirY = glyph.points[pointIdx-1].y-glyph.points[pointIdx].y;
    SWORD nextDirX = glyph.points[pointIdx+1].x-glyph.points[pointIdx].x;
    SWORD nextDirY = glyph.points[pointIdx+1].y-glyph.points[pointIdx].y;
    SDWORD cross1 = bridgeDirX * prevDirY - bridgeDirY * prevDirX;
    SDWORD cross2 = bridgeDirX * nextDirY - bridgeDirY * nextDirX;
    if(cross1 < 0 || cross2 > 0) return false;
    for(WORD k=0; k < bridges.size(); ++k){
        if(lineSegmentCrossing(bridges[k].x1, bridges[k].y1, bridges[k].x2, bridges[k].y2, point.x, point.y, glyph.points[pointIdx].x, glyph.points[pointIdx].y)) return false;
    }

    WORD startIdx = 0;
    for(SWORD j=0; j < glyph.numContours; ++j){
        WORD endIdx = glyph.endOfContours[j];
        for(WORD k=startIdx+1; k <= endIdx; ++k){
            if(k == pointIdx) continue;
            if(k == pointIdxHole) continue;
            if(k-1 == pointIdx) continue;
            if(k-1 == pointIdxHole) continue;
            if(lineSegmentIntersection(glyph.points[k].x, glyph.points[k].y, glyph.points[k-1].x, glyph.points[k-1].y, point.x, point.y, glyph.points[pointIdx].x, glyph.points[pointIdx].y)) return false;
        }
        if(endIdx != pointIdx && endIdx != pointIdxHole && startIdx != pointIdx && startIdx != pointIdxHole){
            if(lineSegmentIntersection(glyph.points[endIdx].x, glyph.points[endIdx].y, glyph.points[startIdx].x, glyph.points[startIdx].y, point.x, point.y, glyph.points[pointIdx].x, glyph.points[pointIdx].y)) return false;
        }
        startIdx = endIdx + 1;
    }
    bridges.push_back({point.x, point.y, glyph.points[pointIdx].x, glyph.points[pointIdx].y});
    return true;
}

struct OutlineContour{
    WORD index;
    std::vector<WORD> indices;
};

void triangulateGlyph(Font& font, BYTE c, WORD x, WORD y, Window& window){
    Glyph& glyph = font.glyphStorage.glyphs[font.asciiToGlyphMapping[c]];
    std::vector<OutlineContour> outlines;
    std::vector<WORD> holes;                   //Speicher die Indexe der "Löcher" Konturen
    std::vector<PointTriangle> triangles;
    std::vector<Edge> bridges;
    WORD startIdx = 0;
    for(SWORD i=0; i < glyph.numContours; ++i){     //Berechnet die winding-order der Konturen, negativ ist mit dem Uhrzeigersinn, positiv gegen
        WORD endIdx = glyph.endOfContours[i];
        SDWORD sign = 0;
        for(WORD j=startIdx+1; j <= endIdx; ++j){
            sign += (glyph.points[j].x-glyph.points[j-1].x)*(glyph.points[j].y+glyph.points[j-1].y);
        }
        sign += (glyph.points[startIdx].x-glyph.points[endIdx].x)*(glyph.points[endIdx].y+glyph.points[startIdx].y);
        if(sign <= 0){
            OutlineContour outline;
            outline.index = i;
            outlines.push_back(std::move(outline));     //TODO idk ob std::move nötig ist
        }else{
            holes.push_back(i);
        }
        startIdx = endIdx + 1;
    }

    for(WORD i=0; i < outlines.size(); ++i){
        OutlineContour& outline = outlines[i];
        WORD startIdx = outline.index > 0 ? glyph.endOfContours[outline.index-1]+1 : 0;
        WORD endIdx = glyph.endOfContours[outline.index];
        std::vector<WORD> remainingContours;
        for(WORD j=0; j < holes.size(); ++j) remainingContours.push_back(holes[j]);
        for(WORD j=startIdx; j <= endIdx; ++j){
            outline.indices.push_back(j);
            if(remainingContours.size() > 0){
                for(size_t k=0; k < remainingContours.size(); ++k){
                    WORD contourIdx = remainingContours[k];
                    if(j == startIdx) continue;     //TODO nur hier weil ich zu faul bin den Fall im testEdges abzudecken
                    if(!testEdges(glyph, contourIdx, j, bridges, window, x, y)) continue;
                    addContour(glyph.endOfContours[contourIdx-1]+1, glyph.endOfContours[contourIdx], outline.indices, window, x, y, glyph);
                    remainingContours.erase(remainingContours.begin()+k);
                    outline.indices.push_back(j);
                    break;
                }
            }
        }
        outline.indices.push_back(startIdx);
    }

    for(WORD i=0; i < outlines.size(); ++i){
        OutlineContour& outline = outlines[i];
        while(outline.indices.size() > 3){
            for(WORD j=1; j < outline.indices.size(); ++j){
                WORD n = outline.indices.size();
                WORD current = outline.indices[j];
                WORD prev = outline.indices[(j-1+n)%n];
                WORD next = outline.indices[(j+1)%n];
                Glyphpoint& p = glyph.points[current];
                Glyphpoint& p1 = glyph.points[prev];
                Glyphpoint& p2 = glyph.points[next];
                bool valid = true;
                SWORD dx1 = p1.x - p.x;
                SWORD dy1 = p1.y - p.y;
                SWORD dx2 = p2.x - p.x;
                SWORD dy2 = p2.y - p.y;
                SDWORD sign = dx1 * dy2 - dy1 * dx2;
                if(sign >= 0) continue;
                for(WORD k=0; k < outline.indices.size(); ++k){
                    Glyphpoint& point = glyph.points[outline.indices[k]];
                    if(&point == &p || &point == &p1 || &point == &p2) continue;
                    if(pointInTriangle(p.x, p.y, p1.x, p1.y, p2.x, p2.y, point.x, point.y)){
                        valid = false;
                        break;
                    }
                }
                if(!valid) continue;
                triangles.push_back({{(SWORD)(p.x+x), (SWORD)(p.y+y)}, {(SWORD)(p1.x+x), (SWORD)(p1.y+y)}, {(SWORD)(p2.x+x), (SWORD)(p2.y+y)}});
                outline.indices.erase(outline.indices.begin()+j);

                #define VISUALIZETRIANGULATION
                #ifdef VISUALIZETRIANGULATION
                clearWindow(window);
                for(WORD k=0; k < outline.indices.size(); ++k){
                    Glyphpoint& point = glyph.points[outline.indices[k]];
                    circles.push_back({(WORD)(point.x+x), (WORD)(point.y+y), 3, 0, RGBA(180, 180, 180)});
                }
                for(WORD k=1; k < outline.indices.size(); ++k){
                    Glyphpoint& p1 = glyph.points[outline.indices[k]];
                    Glyphpoint& p2 = glyph.points[outline.indices[k-1]];
                    lines.push_back({(WORD)(p1.x+x), (WORD)(p1.y+y), (WORD)(p2.x+x), (WORD)(p2.y+y), 1, RGBA(180, 180, 180)});
                }
                for(size_t m=0; m < triangles.size(); ++m){
                    PointTriangle& tri = triangles[m];
                    lines.push_back({(WORD)(tri.p1.x), (WORD)(tri.p1.y), (WORD)(tri.p2.x), (WORD)(tri.p2.y), 1, RGBA(0, 255, 0)});
                    lines.push_back({(WORD)(tri.p1.x), (WORD)(tri.p1.y), (WORD)(tri.p3.x), (WORD)(tri.p3.y), 1, RGBA(0, 255, 0)});
                    lines.push_back({(WORD)(tri.p2.x), (WORD)(tri.p2.y), (WORD)(tri.p3.x), (WORD)(tri.p3.y), 1, RGBA(0, 255, 0)});
                }
                lines.push_back({(WORD)(p.x+x), (WORD)(p.y+y), (WORD)(p1.x+x), (WORD)(p1.y+y), 1, RGBA(255, 0, 0)});
                lines.push_back({(WORD)(p.x+x), (WORD)(p.y+y), (WORD)(p2.x+x), (WORD)(p2.y+y), 1, RGBA(255, 0, 0)});
                lines.push_back({(WORD)(p1.x+x), (WORD)(p1.y+y), (WORD)(p2.x+x), (WORD)(p2.y+y), 1, RGBA(255, 0, 0)});
                renderCircles(window, circles.data(), circles.size());
                renderLines(window, lines.data(), lines.size());
                circles.clear();
                lines.clear();
                drawWindow(window);
                // getchar();
                #endif
            }
        }
    }
}

INT WinMain(HINSTANCE hInstance, HINSTANCE hPreviousInst, LPSTR lpszCmdLine, int nCmdShow){
    Window window;
    if(ErrCheck(createWindow(window, hInstance, 1000, 1000, 0, 0, 1, "Fenster"), "Fenster öffnen") != SUCCESS) return -1;
    if(init() != SUCCESS) return -1;

    Image image;
    if(ErrCheck(loadImage("images/cat.tex", image), "Image laden") != SUCCESS) return -1;

    Font font;
    if(ErrCheck(loadTTF(font, "fonts/OpenSans-Bold.ttf", 800), "Font laden") != SUCCESS) return -1;

    BYTE id = 0;
    while(1){
        getMessages(window);
        if(getWindowFlag(window, WINDOW_CLOSE)) break;

        if(getButton(mouse, MOUSE_LMB)){
            resizeFont(font, 90);
        }
        if(getButton(mouse, MOUSE_RMB)){
            resizeFont(font, 120);
        }

        clearWindow(window);

        // drawFontString(font, lines, "The quick brown fox jumps over the lazy dog.", 10, 10);
        triangulateGlyph(font, id++, 160, 100, window);

        renderCircles(window, circles.data(), circles.size());
        renderLines(window, lines.data(), lines.size());
        drawWindow(window);
        circles.clear();
        lines.clear();
        Sleep(16);
    }
    destroyFont(font);
    destroyImage(image);
    if(ErrCheck(destroyWindow(window), "Fenster schließen") != SUCCESS) return -1;
    return 0;
}
