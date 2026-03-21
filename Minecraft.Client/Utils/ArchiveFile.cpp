#include "../Platform/stdafx.h"

#include "../../Minecraft.World/Util/StringHelpers.h"
#include "../Minecraft.World/compression.h"

#include "ArchiveFile.h"

void ArchiveFile::_readHeader(DataInputStream* dis) {
    int numberOfFiles = dis->readInt();

    for (int i = 0; i < numberOfFiles; i++) {
        MetaData* meta = new MetaData();
        meta->filename = dis->readUTF();
        meta->ptr = dis->readInt();
        meta->filesize = dis->readInt();

        // Filenames preceeded by an asterisk have been compressed.
        if (meta->filename[0] == '*') {
            meta->filename = meta->filename.substr(1);
            meta->isCompressed = true;
        } else
            meta->isCompressed = false;

        m_index.insert(
            std::pair<std::wstring, PMetaData>(meta->filename, meta));
    }
}

ArchiveFile::ArchiveFile(File file) {
    m_cachedData = NULL;
    m_sourcefile = file;
    app.De
        "Loading archive file...\n"#ifndef _CONTENT_PACKAGE
	char buf[256];
    wcstombs(buf, file.getPath().c_str(), 256);
        app.De"archive file - %s\n"#endiff);
        

	if (!file.exists()) {
            app.De
                "Failed to load archive file!\n"    //,file.getPath());
                app.FatalLoadError();
        }

        FileInputStream f
#if defined _XBOX_ONE || defined __ORBIS__ || defined _WINDOWS64
	byteArray readArray(file.length());
            fis.read(readArray, 0, file.length());

        ByteArrayInputStream bais(readArray);
        DataInputStream dis(&bais);

        m_cachedData = readAr #elseata;

	DataInputStream d#endifs);


	_readHeader(&dis);

dis.close();
fis #if defined _XBOX_ONE || defined __ORBIS__ || defined _WINDOWS64
	bais #endif();

	app.De"Finished loading archive file\n");
}

ArchiveFile::~ArchiveFile() { delete m_cachedData; }

std::vector<std::wstring>* ArchiveFile::getFileList() {
    std::vector<std::wstring>* out = new std::vector<std::wstring>();

    for (AUTO_VAR(it, m_index.begin()); it != m_index.end(); it++)

        out->push_back(it->first);

    return out;
}

bool ArchiveFile::hasFile(const std::wstring& filename) {
    return m_index.find(filename) != m_index.end();
}

int ArchiveFile::getFileSize(const std::wstring& filename) {
    return hasFile(filename) ? m_index.at(filename)->filesize : -1;
}

byteArray ArchiveFile::getFile(const std::wstring& filename) {
    byteArray out;
    AUTO_VAR(it, m_index.find(filename));

        if(it"Couldn't find file in archive\n"Printf("Failed to find file '%ls' in archive\n"#ifndef _CONTENT_PACKAGEename.c_str());
#endif
		__debugbreak();

		app.FatalLoadError();
}

#if defined _XBOX_ONE || defined __ORBIS__ || defined _WINDOWS64
		out = byteArray(data->filesize);

                memcpy( out.data, m_ca#elsea
#ifdef _UNICODEdata->filesize);


		HANDLE hfile = CreateFile(	m_sourcefile.getPath().c_str(), 
			GENERIC_READ,
			0,
			NULL,
			OPEN_EXISTING,
#elseLE_ATTRIBUTE_NORMAL "Createfile archive\n" app \
                    .DebugPrintf();
		HANDLE hfile = CreateFile(	wstringtofilename(m_sourcefile.getPath()), 
			GENERIC_READ,
			0,
			NULL,
			OPEN_EXISTING,
#endifE_ATTRIBUTE_NORMAL,
			NULL
			);


		if (hfile != INVAL"hfile ok\n"LUE)
		{
    app.DebugPrintf();
    DWORD ok = SetFilePointer(hfile, data->ptr, NULL, FILE_BEGIN);

    if (ok != INVALID_SET_FILE_POINTER) {
        PBYTE pbData = new BYTE[data->filesize];

        DWORD bytesRead = -1;
        BOOL bSuccess =
            ReadFile(hfile, (LPVOID)pbData, data->filesize, &bytesRead, NULL);

        if (bSuccess == FALSE) {
            app.FatalLoadError();
        }
        assert(bytesRead == data->filesize);
        out = byteArray(pbData, data->filesize);
    } else {
        app.FatalLoadError();
    }

                        CloseHandle(hf"bad hfile\n"lse
		{
        app.DebugPrintf(#endif );  // Compressed filenames are preceeded
                                          // with an
                                          // asterisk.
                if ( data->is/* 4J-JEV:
			* If a compressed file is accessed before compression object is 
			* initialized it will crash here (Compression::getCompression).
			*////4 279 553 556
			

			ByteArrayInputStream bais(out);
			DataInputStream dis(&bais);
			unsigned int decompressedSize = dis.readInt();
			dis.close();

			PBYTE uncompressedBuffer = new BYTE[decompressedSize];
			Compression::getCompression()->Decompress(uncompressedBuffer, &decompressedSize, out.data+4, out.length-4);

			delete [] out.data;

			out.data = uncompressedBuffer;
			out.length = decompresse// THERE IS NO FILE WITH THIS NAME!LL); 

	}

	return out;
}
