#pragma once

#include <iostream>
#include <vector>
#include <math.h>

typedef unsigned char BYTE;			//8  Bit
typedef signed char SBYTE;			//8  Bit signed
typedef unsigned short WORD;		//16 Bit
typedef signed short SWORD;			//16 Bit signed
typedef unsigned long DWORD;		//32 Bit
typedef signed long SDWORD;			//32 Bit signed
typedef unsigned long long QWORD;	//64 Bit
typedef signed long long SQWORD;	//64 Bit signed

//TODO sollte das error zeug nicht auch in window.h?
//Error-Codes
enum ErrCode{
	//Fenster
	SUCCESS = 0,
	GENERIC_ERROR,
	APP_INIT,
	BAD_ALLOC,
	CREATE_WINDOW,
	TEXTURE_NOT_FOUND,
	MODEL_NOT_FOUND,
	MODEL_BAD_FORMAT,
	FILE_NOT_FOUND,
	WINDOW_NOT_FOUND,
	INIT_RENDER_TARGET,
	//USB
	INVALID_USB_HANDLE,
	COMMSTATE_ERROR,
	TIMEOUT_SET_ERROR,
	//Intern
	OPEN_FILE
};
enum ErrCodeFlags{
	ERR_NO_FLAG = 0,
	ERR_NO_OUTPUT = 1,
	ERR_ON_FATAL = 2
};
//TODO ERR_ON_FATAL ausgeben können wenn der nutzer es so möchte
inline ErrCode ErrCheck(ErrCode code, const char* msg="\0", ErrCodeFlags flags=ERR_NO_FLAG){
	switch(code){
	case BAD_ALLOC:
		if(!(flags&ERR_NO_OUTPUT)) std::cerr << "[BAD_ALLOC ERROR] " << msg << std::endl;
		return code;
	case GENERIC_ERROR:
		if(!(flags&ERR_NO_OUTPUT)) std::cerr << "[GENERIC_ERROR ERROR] " << msg << std::endl;
		return code;
	case CREATE_WINDOW:
		if(!(flags&ERR_NO_OUTPUT)) std::cerr << "[CREATE_WINDOW ERROR] " << msg << std::endl;
		return code;
	case TEXTURE_NOT_FOUND:
		if(!(flags&ERR_NO_OUTPUT)) std::cerr << "[TEXTURE_NOT_FOUND ERROR] " << msg << std::endl;
		return code;
	case MODEL_NOT_FOUND:
		if(!(flags&ERR_NO_OUTPUT)) std::cerr << "[MODEL_NOT_FOUND ERROR] " << msg << std::endl;
		return code;
	case MODEL_BAD_FORMAT:
		if(!(flags&ERR_NO_OUTPUT)) std::cerr << "[MODEL_BAD_FORMAT ERROR] " << msg << std::endl;
		return code;
	case FILE_NOT_FOUND:
		if(!(flags&ERR_NO_OUTPUT)) std::cerr << "[FILE_NOT_FOUND ERROR] " << msg << std::endl;
		return code;
	case WINDOW_NOT_FOUND:
		if(!(flags&ERR_NO_OUTPUT)) std::cerr << "[WINDOW_NOT_FOUND ERROR] " << msg << std::endl;
		return code;
	case APP_INIT:
		if(!(flags&ERR_NO_OUTPUT)) std::cerr << "[APP_INIT ERROR] " << msg << std::endl;
		return code;
	case INIT_RENDER_TARGET:
		if(!(flags&ERR_NO_OUTPUT)) std::cerr << "[INIT_RENDER_TARGET ERROR] " << msg << std::endl;
		return code;
	case INVALID_USB_HANDLE:
		if(!(flags&ERR_NO_OUTPUT)) std::cerr << "[INVALID_USB_HANDLE ERROR] " << msg << std::endl;
		return code;
	case COMMSTATE_ERROR:
		if(!(flags&ERR_NO_OUTPUT)) std::cerr << "[COMMSTATE_ERROR ERROR] " << msg << std::endl;
		return code;
	case TIMEOUT_SET_ERROR:
		if(!(flags&ERR_NO_OUTPUT)) std::cerr << "[TIMEOUT_SET_ERROR ERROR] " << msg << std::endl;
		return code;
	case OPEN_FILE:
		if(!(flags&ERR_NO_OUTPUT)) std::cerr << "[OPEN_FILE ERROR] " << msg << std::endl;
		return code;
	default: return SUCCESS;
	}
	return SUCCESS;
}

enum MOUSEBUTTON{
	MOUSE_LMB = 1,
	MOUSE_RMB = 2
};
struct Mouse{
	WORD x;
	WORD y;
	char button = 0;	//Bits: LMB, RMB, Rest ungenutzt
}; static Mouse mouse;

inline constexpr bool getButton(Mouse& mouse, MOUSEBUTTON button){return (mouse.button & button);}
inline constexpr void setButton(Mouse& mouse, MOUSEBUTTON button){mouse.button |= button;}
inline constexpr void resetButton(Mouse& mouse, MOUSEBUTTON button){mouse.button &= ~button;}

//TODO always inline?, compiler weiß es bestimmt besser
inline constexpr __attribute__((always_inline)) const char* stringLookUp2(long value){
	return &"001020304050607080900111213141516171819102122232425262728292"
			"031323334353637383930414243444546474849405152535455565758595"
			"061626364656667686960717273747576777879708182838485868788898"
			"09192939495969798999"[value<<1];
}
//std::to_string ist langsam, das ist simpel und schnell
static char _dec_to_str_out[12] = "00000000000";
inline const char* longToString(long value){
	char* ptr = _dec_to_str_out + 11;
	*ptr = '0';
	char c = 0;
	if(value <= 0){
		value < 0 ? c = '-' : c = '0';
		value = 0-value;
	}
	while(value >= 100){
		const char* tmp = stringLookUp2(value%100);
		ptr[0] = tmp[0];
		ptr[-1] = tmp[1];
		ptr -= 2;
		value /= 100;
	}
	while(value){
		*ptr-- = '0'+value%10;
		value /= 10;
	}
	if(c) *ptr-- = c;
	return ptr+1;
}

//value hat decimals Nachkommestellen
inline std::string intToString(int value, BYTE decimals=2){
	std::string out = longToString(value);
	if(out.size() < ((size_t)decimals+1) && out[0] != '-') out.insert(0, (decimals+1)-out.size(), '0');
	else if(out.size() < ((size_t)decimals+2) && out[0] == '-') out.insert(1, (decimals+1)-(out.size()-1), '0');
	if(decimals) out.insert(out.size()-decimals, 1, '.');
	return out;
}

inline std::string floatToString(float value, BYTE decimals=2){
	WORD precision = pow(10, decimals);
	long val = value * precision;
	return intToString(val, decimals);
}

enum KEYBOARDBUTTON : unsigned long long{
	KEY_0 = 0ULL,
	KEY_1 = 1ULL << 0,
	KEY_3 = 1ULL << 1,
	KEY_4 = 1ULL << 2,
	KEY_5 = 1ULL << 3,
	KEY_6 = 1ULL << 4,
	KEY_7 = 1ULL << 5,
	KEY_8 = 1ULL << 6,
	KEY_9 = 1ULL << 7,
	KEY_A = 1ULL << 8,
	KEY_B = 1ULL << 9,
	KEY_C = 1ULL << 10,
	KEY_D = 1ULL << 11,
	KEY_E = 1ULL << 12,
	KEY_F = 1ULL << 13,
	KEY_G = 1ULL << 14,
	KEY_H = 1ULL << 15,
	KEY_I = 1ULL << 16,
	KEY_J = 1ULL << 17,
	KEY_K = 1ULL << 18,
	KEY_L = 1ULL << 19,
	KEY_M = 1ULL << 20,
	KEY_N = 1ULL << 21,
	KEY_O = 1ULL << 22,
	KEY_P = 1ULL << 23,
	KEY_Q = 1ULL << 24,
	KEY_R = 1ULL << 25,
	KEY_S = 1ULL << 26,
	KEY_T = 1ULL << 27,
	KEY_U = 1ULL << 28,
	KEY_V = 1ULL << 29,
	KEY_W = 1ULL << 30,
	KEY_X = 1ULL << 31,
	KEY_Y = 1ULL << 32,
	KEY_Z = 1ULL << 33,
	KEY_SHIFT = 1ULL << 34,
	KEY_SPACE = 1ULL << 35,
	KEY_CTRL = 1ULL << 36,
	KEY_ALT = 1ULL << 37,
	KEY_ESC = 1ULL << 38,
	KEY_TAB = 1ULL << 39,
	KEY_ENTER = 1ULL << 40,
	KEY_BACK = 1ULL << 41
};
struct Keyboard{
	unsigned long long buttons;	//Bits siehe enum oben
}; static Keyboard keyboard;

inline constexpr bool getButton(Keyboard& keyboard, KEYBOARDBUTTON button){return keyboard.buttons & button;}
inline constexpr void setButton(Keyboard& keyboard, KEYBOARDBUTTON button){keyboard.buttons |= button;}
inline constexpr void resetButton(Keyboard& keyboard, KEYBOARDBUTTON button){keyboard.buttons &= ~button;}

//TODO gescheiter Timer
//Zeitunterschied in Millisekunden
// inline constexpr long long systemTimeDiff(SYSTEMTIME& start, SYSTEMTIME& end){
// 	return (end.wYear-start.wYear)*31536000000+(end.wDay-start.wDay)*86400000+(end.wHour-start.wHour)*3600000+(end.wMinute-start.wMinute)*60000+(end.wSecond-start.wSecond)*1000+end.wMilliseconds-start.wMilliseconds;
// }

#include <chrono>
#define PERFORMANCE_ANALYZER
#define PERFORMANCE_ANALYZER_DATA_POINTS 3
//TODO chrono... ew, win api hat gute schnittstelle mit filetime
struct PerfAnalyzer{
	//Indexe: 0 rasterizer, 1 drawing, 2 ungenutzt
	float data[PERFORMANCE_ANALYZER_DATA_POINTS*8] = {};
	BYTE counter[PERFORMANCE_ANALYZER_DATA_POINTS-1] = {};
	DWORD totalTriangles = 0;
	DWORD drawnTriangles = 0;
	DWORD pixelsDrawn = 0;
	DWORD pixelsCulled = 0;		//Wegen Depthbuffer nicht gezeichnete Pixel
	std::chrono::high_resolution_clock::time_point tp[2];
}; static PerfAnalyzer _perfAnalyzer;

void startTimer(PerfAnalyzer& pa, BYTE idx){pa.tp[idx] = std::chrono::high_resolution_clock::now();}
float stopTimer(PerfAnalyzer& pa, BYTE idx){return std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::high_resolution_clock::now()-pa.tp[idx]).count();}
void reset(PerfAnalyzer& pa){
	pa.totalTriangles = 0;
	pa.drawnTriangles = 0;
	pa.pixelsDrawn = 0;
	pa.pixelsCulled = 0;
}
void recordData(PerfAnalyzer& pa, BYTE idx){
	float ms = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::high_resolution_clock::now()-pa.tp[idx]).count();
	pa.data[pa.counter[idx]/32+idx*8] = ms;
	pa.counter[idx] += 32;
}
void recordDataNoInc(PerfAnalyzer& pa, BYTE idx){
	float ms = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::high_resolution_clock::now()-pa.tp[idx]).count();
	pa.data[pa.counter[idx]/32+idx*8] += ms;
}
float getAvgData(PerfAnalyzer& pa, BYTE idx){
	float out = 0;
	for(BYTE i=0; i < 8; ++i){
		out += pa.data[i+8*idx];
	}
	return out/8.;
}

std::vector<void*> allocs;

struct HashmapData{
	HashmapData* next = nullptr;
	DWORD key;
	void* data = nullptr;
	void* operator new(size_t size){
		void* ptr = ::operator new(size);
		allocs.push_back(ptr);
		return ptr;
	}
	void operator delete(void* ptr){
		for(std::vector<void*>::iterator iter = allocs.begin(); iter != allocs.end(); iter++){
			if((*iter) == ptr){
				allocs.erase(iter);
				break;
			}
		}
		::operator delete(ptr);
	}
};

//Bildet eine DWORD key auf einen generischen void* ab
struct Hashmap{
	DWORD tableSize = 0;
	DWORD size = 0;
	HashmapData** tableBuffer = nullptr;
};

ErrCode createHashmap(Hashmap& map, DWORD tableSize=800)noexcept{
	map.tableSize = tableSize;
	map.tableBuffer = new(std::nothrow) HashmapData*[tableSize]{nullptr};
	if(!map.tableBuffer) return BAD_ALLOC;
	return SUCCESS;
}

//Löscht die Hashmap und deallokiert alle intern allokierten Ressourcen
void destroyHashmap(Hashmap& map)noexcept{
	for(DWORD i=0; i < map.tableSize; ++i){
		HashmapData* current = map.tableBuffer[i];
		while(current != nullptr){
			HashmapData* toDelete = current;
			current = current->next;
			delete toDelete;
		}
	}
	delete[] map.tableBuffer;
	map.size = 0;
}

//Pointer für den Schlüssel key falls gefunden, sonst nullptr
void* searchHashmap(Hashmap& map, DWORD key)noexcept{
	DWORD idx = key % map.tableSize;
	HashmapData* current = map.tableBuffer[idx];
	while(current != nullptr){
		if(current->key == key) return current->data;
		current = current->next;
	}
	return nullptr;
}

//Fügt das Schlüssel-Wert-Paar ein oder updated dieses, falls dieses schon vorhanden sind
void insertHashmap(Hashmap& map, DWORD key, void* data)noexcept{
	DWORD idx = key % map.tableSize;
	HashmapData** current = &map.tableBuffer[idx];
	HashmapData* prev = *current;
	while(*current != nullptr){
		if((*current)->key == key){
			(*current)->data = data;
			return;
		}
		prev = *current;
		current = &((*current)->next);
	}
	*current = new HashmapData;
	(*current)->data = data;
	(*current)->key = key;
	map.size++;
	if(prev) prev->next = *current;
}

//Löscht den Pointer aus der Hashmap und gibt den Zeiger auf die Daten zurück, nullptr falls nicht gefunden
void* removeHashmap(Hashmap& map, DWORD key)noexcept{
	DWORD idx = key % map.tableSize;
	HashmapData** current = &map.tableBuffer[idx];
	while(*current != nullptr){
		if((*current)->key == key){
			HashmapData* toDelete = (*current);
			*current = (*current)->next;
			void* data = toDelete->data;
			delete toDelete;
			map.size--;
			return data;
		}
		current = &(*current)->next;
	}
	return nullptr;
}

void clearHashmap(Hashmap& map)noexcept{
	for(DWORD i=0; i < map.tableSize; ++i){
		HashmapData* current = map.tableBuffer[i];
		while(current != nullptr){
			HashmapData* toDelete = current;
			current = current->next;
			delete toDelete;
		}
		map.tableBuffer[i] = nullptr;
	}
	map.size = 0;
}

struct HashmapIterator{
	void* data = nullptr;				//Der Pointer in der Hashmap
	DWORD key;							//Der Key für den Pointer
	bool valid = false;					//True falls der Iterator valide ist, false sonst
	HashmapData* current = nullptr;		//Intern
	DWORD bucketIndex = 0;				//Der aktuelle "Bucket" Index
};

//Lässt den Iterator durch die Hashmap iterieren, falls der Iterator valid ist ist iterator.valid == true, sonst ist der Iterator invalide
HashmapIterator& iterateHashmap(Hashmap map, HashmapIterator& iterator)noexcept{
	while(1){
		if(iterator.bucketIndex >= map.tableSize){
			iterator.valid = false;
			break;
		}
		while(iterator.current != nullptr){
			iterator.data = iterator.current->data;
			iterator.key = iterator.current->key;
			iterator.current = iterator.current->next;
			iterator.valid = true;
			return iterator;
		}
		iterator.current = map.tableBuffer[iterator.bucketIndex];
		iterator.bucketIndex++;
	}
	return iterator;
}

void resetHashmapIterator(HashmapIterator& iterator){
	iterator.bucketIndex = 0;
	iterator.current = nullptr;
	iterator.data = nullptr;
}

//Befüllt das Array buffer mit allen Pointern die in der Hashmap sind
void getAllElementsHashmap(Hashmap& map, void** buffer)noexcept{
	DWORD idx = 0;
	for(DWORD i=0; i < map.tableSize; ++i){
		HashmapData* current = map.tableBuffer[i];
		while(current != nullptr){
			buffer[idx++] = current->data;
			current = current->next;
		}
	}
}

//Wie viele Pointer sich in der Hashmap befinden
DWORD sizeHashmap(Hashmap& map)noexcept{return map.size;}
