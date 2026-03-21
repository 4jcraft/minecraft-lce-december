#pragma once
#include <ostream>
#include "../Streams/InputOutputStream.h"

class Tag {
public:
    static const byte TAG_End = 0;
    static const byte TAG_Byte = 1;
    static const byte TAG_Short = 2;
    static const byte TAG_Int = 3;
    static const byte TAG_Long = 4;
    static const byte TAG_Float = 5;
    static const byte TAG_Double = 6;
    static const byte TAG_Byte_Array = 7;
    static const byte TAG_String = 8;
    static const byte TAG_List = 9;
    static const byte TAG_Compound = 10;
    static const byte TAG_Int_Array = 11;
    static const int MAX_DEPTH = 512;

private:
    std::wstring name;

protected:
    Tag(const std::wstring& name);

public:
    virtual void write(DataOutput* dos) = 0;
    virtual void load(DataInput* dis, int tagDepth) = 0;
    virtual std::wstring toString() = 0;
    virtual byte getId() = 0;
    void print(std::ostream out);
    void print(char* prefix, std::wostream out);
    std::wstring getName();
    Tag* setName(const std::wstring& name);
    static Tag* readNamedTag(DataInput* dis);
    static Tag* readNamedTag(DataInput* dis, int tagDepth);
    static void writeNamedTag(Tag* tag, DataOutput* dos);
    static Tag* newTag(byte type, const std::wstring& name);
    static wchar_t* getTagName(byte type);
    virtual ~Tag() {}
    virtual bool equals(Tag* obj);  // 4J Brought forward from 1.2
    virtual Tag* copy() = 0;        // 4J Brought foward from 1.2
};
