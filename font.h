#pragma once

#include <fstream>
#include "util.h"
#include "math.h"

struct TableOffset{
	DWORD tag;
	DWORD offset;
};

WORD swapEndian(WORD* val)noexcept{
    BYTE* out = (BYTE*)val;
    return (out[0]<<8) | out[1];
}

SWORD swapEndian(SWORD* val)noexcept{
    BYTE* out = (BYTE*)val;
    return (WORD)(out[0]<<8) | out[1];
}

DWORD swapEndian(DWORD* val)noexcept{
    BYTE* out = (BYTE*)val;
    return (out[0]<<24) | (out[1]<<16) | (out[2]<<8) | out[3];
}

constexpr DWORD tableStringToCode(const char* name)noexcept{
    return (name[3]<<24) | (name[2]<<16) | (name[1]<<8) | name[0];
}

bool flagBitSet(DWORD val, BYTE pos){
	return (val>>pos)&1;
}

BYTE readUint8(std::fstream& file){
    BYTE ret;
    file.read((char*)&ret, 1);
    return ret;
}

WORD readUint16(std::fstream& file){
    WORD ret;
    file.read((char*)&ret, 2);
    return swapEndian(&ret);
}

SWORD readInt16(std::fstream& file){
    SWORD ret;
    file.read((char*)&ret, 2);
    return swapEndian(&ret);
}

DWORD readUint32(std::fstream& file){
    DWORD ret;
    file.read((char*)&ret, 4);
    return swapEndian(&ret);
}

void readCoordinate(std::fstream& file, SWORD* coords, BYTE* flags, BYTE flagBitOffset, SWORD numPoints)noexcept{
	SWORD prevVal = 0;
	for(SWORD i=0; i < numPoints; ++i){
		if(flagBitSet(flags[i], 1+flagBitOffset)){
			BYTE tmp;
			tmp = readUint8(file);
			coords[i] = tmp;
			if(!flagBitSet(flags[i], 4+flagBitOffset)) coords[i] = 0-coords[i];
		}else{
			if(flagBitSet(flags[i], 4+flagBitOffset)){
				coords[i] = prevVal;
				continue;
			}
			coords[i] = readInt16(file);
		}
		coords[i] = prevVal + coords[i];
		prevVal = coords[i];
	}
}

struct Glyph{
	SWORD numPoints = 0;
	SWORD* xCoords = nullptr;
	SWORD* yCoords = nullptr;
	SWORD xMin = 0;
	SWORD yMin = 0;
	SWORD xMax = 0;
	SWORD yMax = 0;
	SWORD numContours = 0;
	WORD* endOfContours = nullptr;
};

void createGlyph(Glyph& glyph, SWORD numPoints, SWORD* xCoords, SWORD* yCoords, SWORD xMin, SWORD yMin, SWORD xMax, SWORD yMax, SWORD numContours, WORD* endOfContours)noexcept{
	glyph.numPoints = numPoints;
	glyph.xCoords = new SWORD[numPoints];
	glyph.yCoords = new SWORD[numPoints];
	for(SWORD i=0; i < numPoints; ++i){
		glyph.xCoords[i] = xCoords[i];
		glyph.yCoords[i] = yCoords[i];
	}
	glyph.xMin = xMin;
	glyph.yMin = yMin;
	glyph.xMax = xMax;
	glyph.yMax = yMax;
	glyph.numContours = numContours;
	glyph.endOfContours = new WORD[numContours];
	for(WORD i=0; i < numContours; ++i){
		glyph.endOfContours[i] = endOfContours[i];
	}
}

void destroyGlyph(Glyph& glyph)noexcept{
	glyph.numPoints = 0;
	delete[] glyph.xCoords;
	delete[] glyph.yCoords;
	glyph.xCoords = nullptr;
	glyph.yCoords = nullptr;
	glyph.numContours = 0;
	delete[] glyph.endOfContours;
	glyph.endOfContours = nullptr;
}

struct GlyphStorage{
    WORD glyphCount = 0;
    Glyph* glyphs = nullptr;
};

void createGlyphStorage(GlyphStorage& storage, WORD glyphCount)noexcept{
    storage.glyphCount = glyphCount;
    storage.glyphs = new Glyph[glyphCount];
}

void destroyGlyphStorage(GlyphStorage& storage)noexcept{
    for(WORD i=0; i < storage.glyphCount; ++i){
        destroyGlyph(storage.glyphs[i]);
    }
    delete[] storage.glyphs;
    storage.glyphs = nullptr;
}

void readSimpleGlyph(std::fstream& file, Glyph& glyph, SWORD numberOfContours)noexcept{
    SWORD xMin, yMin, xMax, yMax;
	xMin = readInt16(file);
	// std::cout << "xMin: " << xMin << std::endl;
	yMin = readInt16(file);
	// std::cout << "yMin: " << yMin << std::endl;
	xMax = readInt16(file);
	// std::cout << "xMax: " << xMax << std::endl;
	yMax = readInt16(file);
	// std::cout << "yMax: " << yMax << std::endl;
	WORD endPtsOfContours[numberOfContours];
	for(SWORD i=0; i < numberOfContours; ++i){
		endPtsOfContours[i] = readUint16(file);
		// std::cout << endPtsOfContours[i] << " ";
	}
    // std::cout << std::endl;
	DWORD numPoints = endPtsOfContours[numberOfContours-1] + 1;
	WORD instructionLength = readUint16(file);
	BYTE instructions[instructionLength];
	for(WORD i=0; i < instructionLength; ++i){
		instructions[i] = readUint8(file);
	}
	BYTE flags[numPoints];
	for(DWORD i=0; i < numPoints; ++i){
		flags[i] = readUint8(file);
		if(flagBitSet(flags[i], 3)){
			BYTE flag = flags[i];
            BYTE toSkip = readUint8(file);
			for(BYTE j=0; j < toSkip; ++j){
				flags[++i] = flag;
			}
		}
	}
	SWORD xCoords[numPoints]{0};
	SWORD yCoords[numPoints]{0};
	readCoordinate(file, xCoords, flags, 0, numPoints);
	readCoordinate(file, yCoords, flags, 1, numPoints);
	// for(SWORD i=0; i < numPoints; ++i){
	// 	std::cout << "(" << xCoords[i] << ", " << yCoords[i] << ")" << std::endl;
	// }
	createGlyph(glyph, numPoints, xCoords, yCoords, xMin, yMin, xMax, yMax, numberOfContours, endPtsOfContours);
}

void readCompoundGlyph(std::fstream& file)noexcept{
    DWORD fileOffset = file.tellg();
    file.seekg(fileOffset+8, std::ios::beg);  //Skippe bounding Box
    WORD flags = readUint16(file);
    WORD offset = 6;
    if(flagBitSet(flags, 0)){
        offset += 2;
    }
    DWORD currentOffset = file.tellg();
    file.seekg(currentOffset+offset, std::ios::beg);  //Skippe den Rest
}

struct HorMetric{
    WORD advanceWidth;
    SWORD leftSideBearing;
};

//TODO sollte eine Mapping-Tabelle für alle Unicode Zeichen haben, kann man vllt mit einer Hashmap umsetzen, da es ja nicht alle Unicode-Zeichen geben muss
struct Font{
    WORD asciiToGlyphMapping[256];
    WORD unitsPerEm;		//TODO eigentlich wird das ja nie benutzt
	float scalingFactor;
    SWORD xMin;
    SWORD yMin;
    SWORD xMax;
    SWORD yMax;
    GlyphStorage glyphStorage;
    WORD horMetricsCount = 0;
    HorMetric* horMetrics = nullptr;
	WORD verticalSpacing = 0;
};

void destroyFont(Font& font){
    destroyGlyphStorage(font.glyphStorage);
    delete[] font.horMetrics;
    font.horMetricsCount = 0;
}

ErrCode loadTTF(Font& font, const char* name)noexcept{
	std::fstream file;
    file.open(name, std::ios::in | std::ios::binary);
    if(!file.is_open()) return FILE_NOT_FOUND;
    file.seekp(0, std::ios::end);
    std::cout << "Dateigröße in Bytes: " << file.tellp() << std::endl;

	Hashmap map;
	createHashmap(map, 100);

    file.seekg(4, std::ios::beg);
    WORD numTables = readUint16(file);
    std::cout << "Tabellen Anzahl: " << numTables << std::endl;
	TableOffset tableData[numTables];
    WORD seekOffset = 12;
    for(WORD i=0; i < numTables; ++i){
        file.seekg(seekOffset, std::ios::beg);
        DWORD tag;
        file.read((char*)&tag, 4);
        file.seekg(seekOffset+8, std::ios::beg);
        DWORD offset = readUint32(file);
		tableData[i].offset = offset;
		tableData[i].tag = tag;
		insertHashmap(map, tag, &tableData[i]);
        seekOffset += 16;
    }

    TableOffset* head = (TableOffset*)searchHashmap(map, tableStringToCode("head"));
    if(head == nullptr){
        destroyHashmap(map);
        file.close();
        return GENERIC_ERROR;
    }
    file.seekg(head->offset, std::ios::beg);
    file.seekg(18, std::ios::cur);  //Alle möglichen Headerdaten skippen
    font.unitsPerEm = readUint16(file);
    file.seekg(8, std::ios::cur);  //Mehr Headerdaten skippen
    font.xMin = readInt16(file);
    font.yMin = readInt16(file);
    font.xMax = readInt16(file);
    font.yMax = readInt16(file);
    file.seekg(14, std::ios::cur);  //Noch mehr Headerdaten skippen
    SWORD indexToLocFormat = readInt16(file);

	TableOffset* maxp = (TableOffset*)searchHashmap(map, tableStringToCode("maxp"));
	if(maxp == nullptr){
		destroyHashmap(map);
		file.close();
		return GENERIC_ERROR;
	}
	file.seekg(maxp->offset, std::ios::beg);
	DWORD version = readUint32(file);
	WORD numGlyphs = readUint16(file);
	std::cout << "Glyphenanzahl: " << numGlyphs << std::endl;
    
    createGlyphStorage(font.glyphStorage, numGlyphs);
	WORD maxPoints = readUint16(file);
	std::cout << "Maximale Punkteanzahl: " << maxPoints << std::endl;
	WORD maxContours = readUint16(file);
	std::cout << "Maximale Konturenanzahl: " << maxContours << std::endl;

    TableOffset* loca = (TableOffset*)searchHashmap(map, tableStringToCode("loca"));
    if(loca == nullptr){
        destroyHashmap(map);
        file.close();
        return GENERIC_ERROR;
    }
    file.seekg(loca->offset, std::ios::beg);
    DWORD glyphOffsets[numGlyphs];
    bool emptyGlyphs[numGlyphs]{false};
    for(WORD i=0; i <= numGlyphs; ++i){     //Ja laut Doku numGlyphs + 1
        if(indexToLocFormat==0){
            WORD offset = readUint16(file);
            glyphOffsets[i] = offset*2;
        }else{
            glyphOffsets[i] = readUint32(file);
        }
        if(i < 1) continue;
        WORD length = glyphOffsets[i] - glyphOffsets[i-1];
        if(length == 0) emptyGlyphs[i-1] = true;
    }

	TableOffset* glyf = (TableOffset*)searchHashmap(map, tableStringToCode("glyf"));
	if(glyf == nullptr){
		destroyHashmap(map);
		file.close();
		return GENERIC_ERROR;
	}

    for(WORD i=0; i < numGlyphs; ++i){
        DWORD offsetForGlyph = glyphOffsets[i] + glyf->offset;
        file.seekg(offsetForGlyph, std::ios::beg);
        SWORD numberOfContours = readInt16(file);
        if(numberOfContours > 0 && emptyGlyphs[i] == false){
            readSimpleGlyph(file, font.glyphStorage.glyphs[i], numberOfContours);
        }else if(numberOfContours == 0){
        }else{
            readCompoundGlyph(file);
        }
    }

    TableOffset* cmap =(TableOffset*)searchHashmap(map, tableStringToCode("cmap"));
    if(cmap == nullptr){
        destroyHashmap(map);
        file.close();
        return GENERIC_ERROR;
    }
    file.seekg(cmap->offset, std::ios::beg);
    file.seekg(2, std::ios::cur);				//Skippe Version
    WORD numberSubtables = readUint16(file);
    for(WORD i=0; i < numberSubtables; ++i){
        WORD platformID = readUint16(file);
        WORD platformSpecificID = readUint16(file);
        DWORD offset = readUint32(file);
        //TODO Sucht aktuell nur nach einer Unicode Tabelle die angeblich am meisten verwendet wird
        if(platformID == 0 && platformSpecificID == 3){
            file.seekg(cmap->offset+offset, std::ios::beg);
            WORD format = readUint16(file);
            //TODO auch hier wieder nur ein Format, nämlich 4, da es das meist genutzte ist
            if(format == 4){
                WORD length = readUint16(file);
                WORD language = readUint16(file);
                WORD segCountX2 = readUint16(file);
                WORD searchRange = readUint16(file);
                WORD entrySelector = readUint16(file);
                WORD rangeShift = readUint16(file);
                WORD* endCode = new WORD[segCountX2/2];
                for(WORD j=0; j < segCountX2/2; ++j){
                    endCode[j] = readUint16(file);
                }
                file.seekg(2, std::ios::cur);				//Skippe padding
                WORD* startCode = new WORD[segCountX2/2];
                for(WORD j=0; j < segCountX2/2; ++j){
                    startCode[j] = readUint16(file);
                }
                WORD* idDelta = new WORD[segCountX2/2];
                for(WORD j=0; j < segCountX2/2; ++j){
                    idDelta[j] = readUint16(file);
                }
                WORD* idRangeOffset = new WORD[segCountX2/2];
                for(WORD j=0; j < segCountX2/2; ++j){
                    idRangeOffset[j] = readUint16(file);
                }
                WORD restBytes = length - (16+4*segCountX2);
                WORD* glyphIndexArray = new WORD[restBytes];
                for(WORD j=0; j < restBytes; ++j){
                    glyphIndexArray[j] = readUint16(file);
                }
                for(WORD j=0; j < 256; ++j){
                    for(WORD k=0; k < segCountX2/2; ++k){
                        if(endCode[k] >= j){				//Missing Char Symbol
                            if(startCode[k] > j){
                                font.asciiToGlyphMapping[j] = 0;
                                break;
                            }
                            if(idRangeOffset[k] == 0){
                                font.asciiToGlyphMapping[j] = idDelta[k] + j;
                                break;
                            }
                            WORD offset = (idRangeOffset[k]-segCountX2+k*2)/2;
                            font.asciiToGlyphMapping[j] = glyphIndexArray[offset + (j-startCode[k])];
                            break;
                        }
                    }
                }
                delete[] startCode;
                delete[] idDelta;
                delete[] idRangeOffset;
                delete[] glyphIndexArray;
                break;
            }
        }
    }

    TableOffset* hhea = (TableOffset*)searchHashmap(map, tableStringToCode("hhea"));
    if(cmap == nullptr){
        destroyHashmap(map);
        file.close();
        return GENERIC_ERROR;
    }
    file.seekg(hhea->offset, std::ios::beg);
    file.seekg(34, std::ios::cur);					//Skippe mal wieder ne Menge Daten
    WORD numOfLongHorMetrics = readUint16(file);

    TableOffset* hmtx = (TableOffset*)searchHashmap(map, tableStringToCode("hmtx"));
    if(cmap == nullptr){
        destroyHashmap(map);
        file.close();
        return GENERIC_ERROR;
    }
    file.seekg(hmtx->offset, std::ios::beg);
    font.horMetricsCount = numOfLongHorMetrics;
    font.horMetrics = new HorMetric[numOfLongHorMetrics];
    for(WORD i=0; i < numOfLongHorMetrics; ++i){
        font.horMetrics[i].advanceWidth = readUint16(file);
        font.horMetrics[i].leftSideBearing = readInt16(file);
    }

	//Berechne yMin und yMax für die relevanten Glyphen
    SWORD yMin = font.glyphStorage.glyphs[font.asciiToGlyphMapping[0]].yMin;
    SWORD yMax = font.glyphStorage.glyphs[font.asciiToGlyphMapping[0]].yMax;
    for(WORD i=1; i < 256; ++i){
        SWORD min = font.glyphStorage.glyphs[font.asciiToGlyphMapping[i]].yMin;
        SWORD max = font.glyphStorage.glyphs[font.asciiToGlyphMapping[i]].yMax;
        if(min < yMin) yMin = min;
        if(max > yMax) yMax = max;
    }
	font.yMax = yMax;
	font.yMin = yMin;
	//Skaliere die Glyphen auf eine 32 Pixelgröße und drehe diese um
	font.scalingFactor = (float)(font.yMax-font.yMin)/32.f;
	for(WORD i=0; i < font.horMetricsCount; ++i) font.horMetrics[i].advanceWidth /= font.scalingFactor;
	font.verticalSpacing = (yMax-yMin)/font.scalingFactor;
	for(WORD i=0; i < font.glyphStorage.glyphCount; ++i){
        Glyph& glyph = font.glyphStorage.glyphs[i];
        glyph.yMin = (0-glyph.yMin)+font.yMax;
        glyph.yMax = (0-glyph.yMax)+font.yMax;
        glyph.yMin /= font.scalingFactor;
        glyph.yMax /= font.scalingFactor;
        glyph.xMin /= font.scalingFactor;
        glyph.xMax /= font.scalingFactor;
        for(WORD j=0; j < glyph.numPoints; ++j){
            glyph.yCoords[j] = (0-glyph.yCoords[j])+font.yMax;
            glyph.xCoords[j] /= font.scalingFactor;
            glyph.yCoords[j] /= font.scalingFactor;
        }
    }

	destroyHashmap(map);
	file.close();
	return SUCCESS;
}

//TODO das sollte alles auf der GPU berechnet werden, auch das Zeichen etc.
ErrCode setFontSize(Font& font, WORD pixelSize)noexcept{
	float newScalingFactor = (float)(font.yMax-font.yMin)/pixelSize;
	float scalingDiff = font.scalingFactor/newScalingFactor;
	font.scalingFactor = newScalingFactor;
	for(WORD i=0; i < font.horMetricsCount; ++i) font.horMetrics[i].advanceWidth *= scalingDiff;
	font.verticalSpacing *= scalingDiff;
	
	for(WORD i=0; i < font.glyphStorage.glyphCount; ++i){
        Glyph& glyph = font.glyphStorage.glyphs[i];
        glyph.yMin /= font.scalingFactor;
        glyph.yMax /= font.scalingFactor;
        glyph.xMin /= font.scalingFactor;
        glyph.xMax /= font.scalingFactor;
        for(WORD j=0; j < glyph.numPoints; ++j){
            glyph.xCoords[j] /= font.scalingFactor;
            glyph.yCoords[j] /= font.scalingFactor;
        }
    }
	return SUCCESS;
}
