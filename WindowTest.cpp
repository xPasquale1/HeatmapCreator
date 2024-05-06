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

void triangulateGlyph(Font& font, BYTE c, WORD x, WORD y, Window& window){
    Glyph& glyph = font.glyphStorage.glyphs[font.asciiToGlyphMapping[c]];
    WORD startIdx = 0;
    std::vector<PointTriangle> triangles;    //Hält alle Datenpunkt Dreiecke TODO kann man im vorab berechnen
    for(SWORD i=0; i < glyph.numContours; ++i){
        WORD endIdx = glyph.endOfContours[i];
        for(WORD j=startIdx; j <= endIdx; ++j){
            Point p1 = {(SWORD)(glyph.points[j].x+x), (SWORD)(glyph.points[j].y+y)};
            for(WORD k=0; k < glyph.numPoints; ++k){
                if(k == j) continue;
                Point p2 = {(SWORD)(glyph.points[k].x+x), (SWORD)(glyph.points[k].y+y)};
                for(WORD l=0; l < glyph.numPoints; ++l){
                    if(l == k || l == j) continue;
                    Point p3 = {(SWORD)(glyph.points[l].x+x), (SWORD)(glyph.points[l].y+y)};

                    clearWindow(window);
                    bool valid = true;

                    SDWORD totalArea = (p2.x-p1.x)*(p3.y-p2.y)-(p2.y-p1.y)*(p3.x-p2.x);
                    if(totalArea <= 0) continue;

                    float midXP1P2 = (p1.x+p2.x)/2.f;
                    float midYP1P2 = (p1.y+p2.y)/2.f;
                    float dyP1P2 = (p2.y-p1.y);
                    float mP1P2;
                    dyP1P2 != 0 ? mP1P2 = -(p2.x-p1.x)/dyP1P2 : mP1P2 = 1e8f;
                    float bP1P2 = midYP1P2 - mP1P2*midXP1P2;

                    float midXP1P3 = (p1.x+p3.x)/2.f;
                    float midYP1P3 = (p1.y+p3.y)/2.f;
                    float dyP1P3 = (p3.y-p1.y);
                    float mP1P3;
                    dyP1P3 != 0 ? mP1P3 = -(p3.x-p1.x)/dyP1P3 : mP1P3 = 1e8f;
                    float bP1P3 = midYP1P3 - mP1P3*midXP1P3;

                    float centerX = (bP1P3-bP1P2)/(mP1P2-mP1P3);
                    float centerY = mP1P2*centerX+bP1P2;
                    float radius2 = (centerX-p1.x)*(centerX-p1.x)+(centerY-p1.y)*(centerY-p1.y);
                    for(DWORD m=0; m < glyph.numPoints; ++m){
                        if(m == l || m == k || m == j) continue;
                        Point p = {(SWORD)(glyph.points[m].x+x), (SWORD)(glyph.points[m].y+y)};
                        float distance2 = (centerX-p.x)*(centerX-p.x)+(centerY-p.y)*(centerY-p.y);
                        if(distance2 < radius2){
                            valid = false;
                            break;
                        }
                    }

                    clearWindow(window);
                    circles.push_back({(WORD)centerX, (WORD)centerY, sqrt(radius2), sqrt(radius2)-1, RGBA(255, 255, 255)});
                    for(DWORD m=0; m < glyph.numPoints; ++m){
                        circles.push_back({(WORD)(glyph.points[m].x+x), (WORD)(glyph.points[m].y+y), 3, 0, RGBA(255, 255, 255)});
                    }
                    for(size_t m=0; m < triangles.size(); ++m){
                        PointTriangle& tri = triangles[m];
                        lines.push_back({(WORD)(tri.p1.x), (WORD)(tri.p1.y), (WORD)(tri.p2.x), (WORD)(tri.p2.y), 1, RGBA(0, 255, 0)});
                        lines.push_back({(WORD)(tri.p1.x), (WORD)(tri.p1.y), (WORD)(tri.p3.x), (WORD)(tri.p3.y), 1, RGBA(0, 255, 0)});
                        lines.push_back({(WORD)(tri.p2.x), (WORD)(tri.p2.y), (WORD)(tri.p3.x), (WORD)(tri.p3.y), 1, RGBA(0, 255, 0)});
                    }
                    lines.push_back({(WORD)(p1.x), (WORD)(p1.y), (WORD)(p2.x), (WORD)(p2.y), 1, RGBA(255, 0, 0)});
                    lines.push_back({(WORD)(p1.x), (WORD)(p1.y), (WORD)(p3.x), (WORD)(p3.y), 1, RGBA(255, 0, 0)});
                    lines.push_back({(WORD)(p2.x), (WORD)(p2.y), (WORD)(p3.x), (WORD)(p3.y), 1, RGBA(255, 0, 0)});
                    renderCircles(window, circles.data(), circles.size());
                    renderLines(window, lines.data(), lines.size());
                    circles.clear();
                    lines.clear();
                    drawWindow(window);
                    getchar();

                    if(!valid) continue;
                    for(size_t m=0; m < triangles.size(); ++m){     //Testet ob das Dreieck mit anderen überlappt
                        PointTriangle& tri = triangles[m];
                        DWORD identical = 0;
                        if(triangleOverlap(p1.x, p1.y, p2.x, p2.y, p3.x, p3.y, tri.p1.x, tri.p1.y, tri.p2.x, tri.p2.y, tri.p3.x, tri.p3.y, identical)){
                            valid = false;
                            break;
                        }
                        if(identical >= 2){
                            valid = false;
                            break;
                        }
                    }
                    if(!valid) continue;

                    triangles.push_back({p1, p2, p3});
                }
            }
        }
        startIdx = endIdx + 1;
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
        triangulateGlyph(font, 'B', 160, 100, window);

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
