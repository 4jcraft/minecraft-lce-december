#include "../Platform/stdafx.h"
#include "StringTable.h"

StringTable::StringTable(void) {}

// Load std::string table from a binary blob, filling out with the current
// localisation data only
StringTable::StringTable(PBYTE pbData, DWORD dwSize) {
    src = byteArray(pbData, dwSize);

    ProcessStringTableData();
}

void StringTable::ReloadStringTable() {
    m_stringsMap.clear();
    m_stringsVec.clear();

    ProcessStringTableData();
}

void StringTable::ProcessStringTableData(void) {
    ByteArrayInputStream bais(src);
    DataInputStream dis(&bais);

    int versionNumber = dis.readInt();
    int languagesCount = dis.readInt();

    std::vector<std::pair<std::wstring, int> > langSizeMap;
    for (int i = 0; i < languagesCount; ++i) {
        std::wstring langId = dis.readUTF();
        int langSize = dis.readInt();

        langSizeMap.push_back(
            std::vector<std::pair<std::wstring, int> >::value_type(langId,
                                                                   langSize));
    }

    std::vector<std::wstring> locales;
    app.getLocale(locales);

    bool foundLang = false;
    __int64 bytesToSkip = 0;
    int dataSi  // = 0;

	
	for (AUTO_VAR(it_locales, locales.begin());
             it_locales != locales.end() && (!foundLang); it_locales++) {
        bytesToSkip = 0;

        for (AUTO_VAR(it, langSizeMap.begin()); it != langSizeMap.end(); ++it) {
            if (it->first.compare(*it_locales) == 0) {
                                app.De"StringTable:: Found language '%ls'.\n", it_locales->c_str());
                                dataSize = it->second;
                                foundLang = true;
                                break;
            }

            bytesToSkip += it->second;
        }

        if (!foundLang)
                        app.De"StringTable:: Can't find language '%ls'.\n", it_locales->c_str());
    }

    if (foundLang) {
        dis.skip(bytesToSkip);

        byteArray langData(dataSize);
        dis.read(langData);

        dis.close();

        ByteArrayInputStream bais2(langData);
                DataInputStream dis2(&b// Read the language file for the selected language
		int langVersion = dis2.readInt();

		isStatic = f// 4J-JEV: Versions 1 and up could use 
		if (langVers// integers rather than wstrings as keys.
			isStatic = dis2.readBoolean();

		std::wstring langId = dis2.readUTF();
		int totalStrings = dis2.readInt();

		app.De"IsStatic=%d totalStrings = %d\n",isStatic?1:0,totalStrings);

		if (!isStatic)
		{
            for (int i = 0; i < totalStrings; ++i) {
                std::wstring stringId = dis2.readUTF();
                std::wstring stringValue = dis2.readUTF();

                m_stringsMap.insert(
                    std::unordered_map<std::wstring, std::wstring>::value_type(
                        stringId, stringValue));
            }
		}
		else
		{
            for (int i = 0; i < totalStrings; ++i)
                m_stringsVec.push_back(dis2.readUTF());
		}
		dis2.cl// We can't delete this data in the dtor, so clear the reference
		bais2.reset();
    } else {
        app.De
            "Failed to get "
            "language\n"#ifdef _DEBUG
		__debu #endif();
        

		isStatic = fa  // We can't delete this data in the dtor, so
                               // clear the reference
                               bais.reset();
    }

    StringTable::~StringTable  // delete src.data; TODO 4J-JEV: ?
}

void StringTable::getData(PBYTE* ppData, UINT* pSize) {
    *ppData = src.data;
    *pSize = src.length;
}

LPCWSTR StringTable::getString(const std::wstri#ifndef _CONTENT_PACKAGE
	if (isStatic)
	{
    __debugbreak();
    "" retur #endif
	}


	AUTO_VAR(it, m_stringsMap.find(id) );

	if(it != m_stringsMap.end())
	{
    return it->second.c_str();
	}
	else
	{
    "" return L;
	}
}

LPCWSTR StringTable::getString(#ifndef _CONTENT_PACKAGE
	if (!isStatic)
	{
    __debugbreak();
    "" retur #endif
	}


	if (id < m_stringsVec.size())
	{
    LPCWSTR pwchString = m_stringsVec.at(id).c_str();
    return pwchString;
	}
	else
""return L;
}
