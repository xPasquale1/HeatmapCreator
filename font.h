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

void readCoordinate(std::fstream& file, SWORD* coords, BYTE* flags, BYTE flagBitOffset, SWORD numPoints){
	SWORD prevVal = 0;
	for(SWORD i=0; i < numPoints; ++i){
		if(flagBitSet(flags[i], 1+flagBitOffset)){
			BYTE tmp;
			file.read((char*)&tmp, 1);
			coords[i] = tmp;
			if(!flagBitSet(flags[i], 4+flagBitOffset)) coords[i] *= -1;
		}else{
			if(flagBitSet(flags[i], 4+flagBitOffset)){
				coords[i] = prevVal;
				continue;
			}
			file.read((char*)&coords[i], 2);
			coords[i] = swapEndian(&coords[i]);
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

void createGlyph(Glyph& glyph, SWORD numPoints, SWORD* xCoords, SWORD* yCoords, SWORD xMin, SWORD yMin, SWORD xMax, SWORD yMax, SWORD numContours, WORD* endOfContours){
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

void destroyGlyph(Glyph& glyph){
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

void createGlyphStorage(GlyphStorage& storage, WORD glyphCount){
    storage.glyphCount = glyphCount;
    storage.glyphs = new Glyph[glyphCount];
}

void destroyGlyphStorage(GlyphStorage& storage){
    for(WORD i=0; i < storage.glyphCount; ++i){
        destroyGlyph(storage.glyphs[i]);
    }
    delete[] storage.glyphs;
    storage.glyphs = nullptr;
}

void readSimpleGlyph(std::fstream& file, Glyph& glyph, SWORD numberOfContours){
    SWORD xMin, yMin, xMax, yMax;
	file.read((char*)&xMin, 2);
	xMin = swapEndian(&xMin);
	std::cout << "xMin: " << xMin << std::endl;
	file.read((char*)&yMin, 2);
	yMin = swapEndian(&yMin);
	std::cout << "yMin: " << yMin << std::endl;
	file.read((char*)&xMax, 2);
	xMax = swapEndian(&xMax);
	std::cout << "xMax: " << xMax << std::endl;
	file.read((char*)&yMax, 2);
	yMax = swapEndian(&yMax);
	std::cout << "yMax: " << yMax << std::endl;
	WORD endPtsOfContours[numberOfContours];
	for(SWORD i=0; i < numberOfContours; ++i){
		file.read((char*)&endPtsOfContours[i], 2);
		endPtsOfContours[i] = swapEndian(&endPtsOfContours[i]);
		std::cout << endPtsOfContours[i] << " ";
	} std::cout << std::endl;
	DWORD numPoints = endPtsOfContours[numberOfContours-1] + 1;
	WORD instructionLength;
	file.read((char*)&instructionLength, 2);
	instructionLength = swapEndian(&instructionLength);
	BYTE instructions[instructionLength];
	for(WORD i=0; i < instructionLength; ++i){
		file.read((char*)&instructions[i], 1);
	}
	BYTE flags[numPoints];
	for(DWORD i=0; i < numPoints; ++i){
		file.read((char*)&flags[i], 1);
		if(flagBitSet(flags[i], 3)){
			BYTE flag = flags[i];
            BYTE toSkip;
			file.read((char*)&toSkip, 1);
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

void readCompoundGlyph(std::fstream& file){
    DWORD fileOffset = file.tellg();
    file.seekg(fileOffset+8, std::ios::beg);  //Skippe bounding Box
    WORD flags;
    file.read((char*)&flags, 2);
    flags = swapEndian(&flags);
    WORD offset = 6;
    if(flagBitSet(flags, 0)){
        offset += 2;
    }
    DWORD currentOffset = file.tellg();
    file.seekg(currentOffset+offset, std::ios::beg);  //Skippe den Rest
}

static GlyphStorage glyphStorage;
ErrCode loadTTF(const char* name){
	std::fstream file;
    file.open(name, std::ios::in | std::ios::binary);
    if(!file.is_open()) return FILE_NOT_FOUND;
    file.seekp(0, std::ios::end);
    std::cout << "Dateigröße in Bytes: " << file.tellp() << std::endl;

	Hashmap map;
	createHashmap(map, 100);

    WORD numTables;
    file.seekg(4, std::ios::beg);
    file.read((char*)&numTables, 2);
    numTables = swapEndian(&numTables);
    std::cout << "Tabellen Anzahl: " << numTables << std::endl;
	TableOffset tableData[numTables];
    WORD seekOffset = 12;
    for(WORD i=0; i < numTables; ++i){
        file.seekg(seekOffset, std::ios::beg);
        DWORD tag;
        file.read((char*)&tag, 4);
        file.seekg(seekOffset+8, std::ios::beg);
        DWORD offset;
        file.read((char*)&offset, 4);
        offset = swapEndian(&offset);
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
    file.seekg(50, std::ios::cur);  //Alle möglichen Headerdaten skippen
    SWORD indexToLocFormat;
    file.read((char*)&indexToLocFormat, 2);
    indexToLocFormat = swapEndian(&indexToLocFormat);

	TableOffset* maxp = (TableOffset*)searchHashmap(map, tableStringToCode("maxp"));
	if(maxp == nullptr){
		destroyHashmap(map);
		file.close();
		return GENERIC_ERROR;
	}
	file.seekg(maxp->offset, std::ios::beg);
	DWORD version;
	file.read((char*)&version, 4);
	WORD numGlyphs;
	file.read((char*)&numGlyphs, 2);
	numGlyphs = swapEndian(&numGlyphs);
	std::cout << "Glyphenanzahl: " << numGlyphs << std::endl;
    createGlyphStorage(glyphStorage, numGlyphs);
	WORD maxPoints;
	file.read((char*)&maxPoints, 2);
	maxPoints = swapEndian(&maxPoints);
	std::cout << "Maximale Punkteanzahl: " << maxPoints << std::endl;
	WORD maxContours;
	file.read((char*)&maxContours, 2);
	maxContours = swapEndian(&maxContours);
	std::cout << "Maximale Konturenanzahl: " << maxContours << std::endl;

    TableOffset* loca = (TableOffset*)searchHashmap(map, tableStringToCode("loca"));
    if(loca == nullptr){
        destroyHashmap(map);
        file.close();
        return GENERIC_ERROR;
    }
    file.seekg(loca->offset, std::ios::beg);
    DWORD glyphOffsets[numGlyphs];
    for(WORD i=0; i <= numGlyphs; ++i){     //Ja laut Doku numGlyphs + 1
        if(indexToLocFormat==0){
            WORD offset;
            file.read((char*)&offset, 2);
            offset = swapEndian(&offset);
            glyphOffsets[i] = offset*2;
        }else{
            file.read((char*)&glyphOffsets[i], 4);
            glyphOffsets[i] = swapEndian(&glyphOffsets[i]);
        }
    }

	TableOffset* glyf = (TableOffset*)searchHashmap(map, tableStringToCode("glyf"));
	if(glyf == nullptr){
		destroyHashmap(map);
		file.close();
		return GENERIC_ERROR;
	}
    file.seekg(glyf->offset, std::ios::beg);

    for(WORD i=4; i < numGlyphs; ++i){
        DWORD offsetForGlyph = glyphOffsets[i] + glyf->offset;
        file.seekg(offsetForGlyph, std::ios::beg);
        SWORD numberOfContours;
        file.read((char*)&numberOfContours, 2);
        numberOfContours = swapEndian(&numberOfContours);
        std::cout << "Konturenanzahl: " << numberOfContours << std::endl;
        if(numberOfContours > 0){
            std::cout << "Simpler Glyph" << std::endl;
            readSimpleGlyph(file, glyphStorage.glyphs[i], numberOfContours);
        }else if(numberOfContours == 0){
            file.seekg(8, std::ios::cur);
        }else{
            std::cout << "Hässlicher Glyph" << std::endl;
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
    file.seekg(2, std::ios::cur);   //Skippe Version
    WORD numberSubtables;
    file.read((char*)&numberSubtables, 2);
    numberSubtables = swapEndian(&numberSubtables);
    for(WORD i=0; i < numberSubtables; ++i){
        WORD platformID;
        file.read((char*)&platformID, 2);
        platformID = swapEndian(&platformID);
    }

	destroyHashmap(map);
	file.close();
	return SUCCESS;
}
