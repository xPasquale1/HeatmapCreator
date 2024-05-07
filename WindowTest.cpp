#include "windowgl.h"
#include "font.h"

std::vector<LineData> lines;
std::vector<CircleData> circles;

DWORD colorPicker(BYTE idx){
    switch(idx){
        case 0: return RGBA(255, 0, 0);
        case 1: return RGBA(0, 255, 0);
        case 2: return RGBA(0, 0, 255);
        case 3: return RGBA(255, 0, 255);
        case 4: return RGBA(255, 255, 0);
        case 5: return RGBA(0, 255, 255);
    }
    return RGBA(255, 255, 255);
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

struct Point{
    SWORD x;
    SWORD y;
};

struct PointTriangle{
    Point p1;
    Point p2;
    Point p3;
};

bool testDelauney(Glyph& glyph, WORD idx1, WORD idx2, WORD idx3, WORD x, WORD y, PointTriangle* triangles, DWORD count, Window& window){
    Point p1 = {(SWORD)(glyph.points[idx1].x+x), (SWORD)(glyph.points[idx1].y+y)};
    Point p2 = {(SWORD)(glyph.points[idx2].x+x), (SWORD)(glyph.points[idx2].y+y)};
    Point p3 = {(SWORD)(glyph.points[idx3].x+x), (SWORD)(glyph.points[idx3].y+y)};

    bool valid = true;

    SDWORD totalArea = (p2.x-p1.x)*(p3.y-p2.y)-(p2.y-p1.y)*(p3.x-p2.x);
    if(totalArea <= 0) return false;

    float midXP1P2 = (p1.x+p2.x)/2.f;
    float midYP1P2 = (p1.y+p2.y)/2.f;
    float dyP1P2 = (p2.y-p1.y);
    float mP1P2;
    dyP1P2 != 0 ? mP1P2 = -(p2.x-p1.x)/dyP1P2 : mP1P2 = 1e4f;
    float bP1P2 = midYP1P2 - mP1P2*midXP1P2;

    float midXP1P3 = (p1.x+p3.x)/2.f;
    float midYP1P3 = (p1.y+p3.y)/2.f;
    float dyP1P3 = (p3.y-p1.y);
    float mP1P3;
    dyP1P3 != 0 ? mP1P3 = -(p3.x-p1.x)/dyP1P3 : mP1P3 = 1e4f;
    float bP1P3 = midYP1P3 - mP1P3*midXP1P3;

    float centerX = (bP1P3-bP1P2)/(mP1P2-mP1P3);
    float centerY = mP1P2*centerX+bP1P2;
    float radius2 = (centerX-p1.x)*(centerX-p1.x)+(centerY-p1.y)*(centerY-p1.y);
    for(DWORD i=0; i < glyph.numPoints; ++i){
        if(i == idx1 ||i == idx2 ||i == idx3) continue;
        Point p = {(SWORD)(glyph.points[i].x+x), (SWORD)(glyph.points[i].y+y)};
        float distance2 = (centerX-p.x)*(centerX-p.x)+(centerY-p.y)*(centerY-p.y);
        if(distance2 < radius2) return false;
    }
    for(size_t i=0; i < count; ++i){     //Testet ob das Dreieck mit anderen überlappt
        PointTriangle& tri = triangles[i];
        DWORD identical = 0;
        if(triangleOverlap(p1.x, p1.y, p2.x, p2.y, p3.x, p3.y, tri.p1.x, tri.p1.y, tri.p2.x, tri.p2.y, tri.p3.x, tri.p3.y, identical)) return false;
    }
    return true;
}

void triangulateGlyphOld(Font& font, BYTE c, WORD x, WORD y, Window& window){
    Glyph& glyph = font.glyphStorage.glyphs[font.asciiToGlyphMapping[c]];
    WORD startIdx = 0;
    std::vector<PointTriangle> triangles;           //TODO kann man im vorab berechnen
    for(SWORD i=0; i < glyph.numContours; ++i){
        WORD endIdx = glyph.endOfContours[i];
        for(WORD j=startIdx; j <= endIdx; ++j){
            for(WORD k=0; k < glyph.numPoints; ++k){
                if(k == j) continue;
                for(WORD l=0; l < glyph.numPoints; ++l){
                    if(l == k || l == j) continue;
                    if(!testDelauney(glyph, j, k, l, x, y, triangles.data(), triangles.size(), window)) continue;
                    triangles.push_back({{(SWORD)(glyph.points[j].x+x), (SWORD)(glyph.points[j].y+y)}, {(SWORD)(glyph.points[k].x+x), (SWORD)(glyph.points[k].y+y)}, {(SWORD)(glyph.points[l].x+x), (SWORD)(glyph.points[l].y+y)}});
                }
            }
        }
        startIdx = endIdx + 1;
    }
}

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

void addContour(WORD startIdx, WORD endIdx, std::vector<WORD>& indices){
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
    WORD startIdx = 0;
    Glyphpoint& point = glyph.points[glyph.endOfContours[contourIdx-1]+1];
    for(WORD k=0; k < bridges.size(); ++k){
        if(lineSegmentIntersection(bridges[k].x1, bridges[k].y1, bridges[k].x2, bridges[k].y2, point.x, point.y, glyph.points[pointIdx].x, glyph.points[pointIdx].y)) return false;
    }
    for(SWORD j=0; j < glyph.numContours; ++j){
        WORD endIdx = glyph.endOfContours[j];
        for(WORD k=startIdx+1; k <= endIdx; ++k){
            if(lineSegmentIntersection(glyph.points[k].x, glyph.points[k].y, glyph.points[k-1].x, glyph.points[k-1].y, point.x, point.y, glyph.points[pointIdx].x, glyph.points[pointIdx].y)) return false;
        }
        if(lineSegmentIntersection(glyph.points[endIdx].x, glyph.points[endIdx].y, glyph.points[startIdx].x, glyph.points[startIdx].y, point.x, point.y, glyph.points[pointIdx].x, glyph.points[pointIdx].y)) return false;
        startIdx = endIdx + 1;
    }
    bridges.push_back({point.x, point.y, glyph.points[pointIdx].x, glyph.points[pointIdx].y});
    return true;
}

void triangulateGlyph(Font& font, BYTE c, WORD x, WORD y, Window& window){
    Glyph& glyph = font.glyphStorage.glyphs[font.asciiToGlyphMapping[c]];
    std::vector<WORD> indices;
    std::vector<PointTriangle> triangles;
    std::vector<WORD> remainingContours;
    std::vector<Edge> bridges;
    WORD startIdx = 0;
    for(WORD i=1; i < glyph.numContours; ++i) remainingContours.push_back(i);
    for(WORD i=0; i <= glyph.endOfContours[0]; ++i){
        indices.push_back(i);
        if(remainingContours.size() > 0){
            WORD startIdx = 0;
            for(size_t j=0; j < remainingContours.size(); ++j){
                WORD contourIdx = remainingContours[j];
                if(!testEdges(glyph, contourIdx, i, bridges, window, x, y)) continue;
                addContour(glyph.endOfContours[contourIdx-1]+1, glyph.endOfContours[contourIdx], indices);
                remainingContours.erase(remainingContours.begin()+j);
                indices.push_back(i);
            }
        }
    }
    indices.push_back(0);

    while(indices.size() > 3){
        for(WORD j=1; j < indices.size(); ++j){
            WORD n = indices.size();
            WORD current = indices[j];
            WORD prev = indices[(j-1+n)%n];
            WORD next = indices[(j+1)%n];
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
            for(WORD k=0; k < indices.size(); ++k){
                Glyphpoint& point = glyph.points[indices[k]];
                if(&point == &p || &point == &p1 || &point == &p2) continue;
                if(pointInTriangle(p.x, p.y, p1.x, p1.y, p2.x, p2.y, point.x, point.y)){
                    valid = false;
                    break;
                }
            }
            if(!valid) continue;
            triangles.push_back({{(SWORD)(p.x+x), (SWORD)(p.y+y)}, {(SWORD)(p1.x+x), (SWORD)(p1.y+y)}, {(SWORD)(p2.x+x), (SWORD)(p2.y+y)}});
            indices.erase(indices.begin()+j);

            #define VISUALIZETRIANGULATION
            #ifdef VISUALIZETRIANGULATION
            clearWindow(window);
            for(WORD k=0; k < indices.size(); ++k){
                Glyphpoint& point = glyph.points[indices[k]];
                circles.push_back({(WORD)(point.x+x), (WORD)(point.y+y), 3, 0, RGBA(180, 180, 180)});
            }
            for(WORD k=1; k < indices.size(); ++k){
                Glyphpoint& p1 = glyph.points[indices[k]];
                Glyphpoint& p2 = glyph.points[indices[k-1]];
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
            getchar();
            #endif
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
        triangulateGlyph(font, 'C', 160, 100, window);

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
