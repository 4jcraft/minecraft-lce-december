#pragma once
#include <ostream>
#include "../Streams/InputOutputStream.h"

class Tag {
public:
    static const uint8_t TAG_End = 0;
    static const uint8_t TAG_Byte = 1;
    static const uint8_t TAG_Short = 2;
    static const uint8_t TAG_Int = 3;
    static const uint8_t TAG_Long = 4;
    static const uint8_t TAG_Float = 5;
    static const uint8_t TAG_Double = 6;
    static const uint8_t TAG_Byte_Array = 7;
    static const uint8_t TAG_String = 8;
    static const uint8_t TAG_List = 9;
    static const uint8_t TAG_Compound = 10;
    static const uint8_t TAG_Int_Array = 11;
    static const int MAX_DEPTH = 512;

private:
    std::wstring name;

protected:
    Tag(const std::wstring& name);

public:
    virtual void write(DataOutput* dos) = 0;
    virtual void load(DataInput* dis, int tagDepth) = 0;
    virtual std::wstring toString() = 0;
    virtual uint8_t getId() = 0;
    void print(std::ostream out);
    void print(char* prefix, std::wostream out);
    std::wstring getName();
    Tag* setName(const std::wstring& name);
    static Tag* readNamedTag(DataInput* dis);
    static Tag* readNamedTag(DataInput* dis, int tagDepth);
    static void writeNamedTag(Tag* tag, DataOutput* dos);
    static Tag* newTag(uint8_t type, const std::wstring& name);
    static wchar_t* getTagName(uint8_t type);
    virtual ~Tag() {}
    virtual bool equals(Tag* obj);  // 4J Brought forward from 1.2
    virtual Tag* copy() = 0;   // 4J Brought foward from 1.2
};
