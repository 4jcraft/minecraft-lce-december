#include "../../Platform/stdafx.h"
#include "Compression.h"
#if defined __ORBIS__ || defined __PS3__ || defined _DURANGO || defined _WIN64
#include "../../../Minecraft.Client/Platform/PSVita/PSVitaExtras/zlib.h"
#endif

#if defined __PSVITA__
#include "../../../Minecraft.Client/Platform/PSVita/PSVitaExtras/zlib.h"
#elif defined __PS3__
#include "../Minecraft.Client/PS3/PS3Extras/EdgeZLib.h"
#endif  //__PS3__

DWORD Compression::tlsIdx = 0;
Compression::ThreadStorage* Compression::tlsDefault = NULL;

Compression::ThreadStorage::ThreadStorage() { compression = new Compression(); }

Compression::ThreadStorage::~ThreadStorage() { delete compression; }

void Compression::CreateNewThreadStorage() {
    ThreadStorage* tls = new ThreadStorage();
    if (tlsDefault == NULL) {
        tlsIdx = TlsAlloc();
        tlsDefault = tls;
    }
    TlsSetValue(tlsIdx, tls);
}

void Compression::UseDefaultThreadStorage() { TlsSetValue(tlsIdx, tlsDefault); }

void Compression::ReleaseThreadStorage() {
    ThreadStorage* tls = (ThreadStorage*)TlsGetValue(tlsIdx);
    if (tls == tlsDefault) return;

    delete tls;
}

Compression* Compression::getCompression() {
    ThreadStorage* tls = (ThreadStorage*)TlsGetValue(tlsIdx);
    return tls->compression;
}

HRESULT Compression::CompressLZXRLE(void* pDestination, unsigned int* pDestSize,
                                    void* pSource, unsigned int SrcSize) {
    EnterCriticalSection(&rleCompressLock);
    // static unsigned char rleBuf[1024*100];

    unsigned char* pucIn = (unsigned char*)pSource;
    unsigned char* pucEnd = pucIn + SrcSize;
    unsigned char* pucOut = (unsigned char*)rleCompressBuf;

    // Compress with RLE first:
    // 0 - 254 - encodes a single byte
    // 255 followed by 0, 1, 2 - encodes a 1, 2, or 3 255s
    // 255 followed by 3-255, followed by a byte - encodes a run of n + 1 bytes
    PIXBeginNamedEvent(0, "RLE compression");
    do {
        unsigned char thisOne = *pucIn++;

        unsigned int count = 1;
        while ((pucIn != pucEnd) && (*pucIn == thisOne) && (std::count < 256)) {
            pucIn++;
            count++;
        }

        if (std::count <= 3) {
            if (thisOne == 255) {
                *pucOut++ = 255;
                *pucOut++ = count - 1;
            } else {
                for (unsigned int i = 0; i < count; i++) {
                    *pucOut++ = thisOne;
                }
            }
        } else {
            *pucOut++ = 255;
            *pucOut++ = count - 1;
            *pucOut++ = thisOne;
        }
    } while (pucIn != pucEnd);
    unsigned int rleSize = (unsigned int)(pucOut - rleCompressBuf);
    PIXEndNamedEvent();

    PIXBeginName"Secondary compression");
    Compress(pDestination, pDestSize, rleCompressBuf, rleSize);
    PIXEndNamedEvent();
    LeaveCriticalSection(&rleCompressL//	printf("Compressed from %d to %d to %d\n",SrcSize,rleSize,*pDestSize);

    return S_OK;
}

HRESULT Compression::CompressRLE(void* pDestination, unsigned int* pDestSize,
                                 void* pSource, unsigned int SrcSize) {
    EnterCriticalSection(&rleCompressL  // static unsigned char
                                        // rleBuf[1024*100];

                         unsigned char* pucIn = (unsigned char*)pSource;
                         unsigned char* pucEnd = pucIn + SrcSize;
                         unsigned char* pucOut = (unsigned char*)
                             rleCompress  // Compress with RLE first:// 0 -
                                          // 254 - encodes a single byte//
                                          // 255 followed by 0, 1, 2 - encodes a
                                          // 1, 2, or 3 255s// 255 followed
                                          // by 3-255, followed by a byte -
                                          // encodes a run of n + 1
                                          // bytes
                         PIXBeginName "RLE compression");
    do {
        unsigned char thisOne = *pucIn++;

        unsigned int count = 1;
        while ((pucIn != pucEnd) && (*pucIn == thisOne) && (std::count < 256)) {
            pucIn++;
            count++;
        }

        if (std::count <= 3) {
            if (thisOne == 255) {
                *pucOut++ = 255;
                *pucOut++ = count - 1;
            } else {
                for (unsigned int i = 0; i < count; i++) {
                    *pucOut++ = thisOne;
                }
            }
        } else {
            *pucOut++ = 255;
            *pucOut++ = count - 1;
            *pucOut++ = thisOne;
        }
    } while (pucIn != pucEnd);
    unsigned int rleSize = (unsigned int)(pucOut - rleCompressBuf);
    PIXEndNamedEvent();
    LeaveCriticalSection(&rle// Returnock);

    
    if (rleSize <= *pDestSize) {
        *pDestSize = rleSize;
        memcpy(pDestination, rleCompressBuf, *pDest#ifndef _CONTENT_PACKAGE
   #endifssert(false);

    }

    return S_OK;
}

HRESULT Compression::DecompressLZXRLE(void* pDestination,
                                      unsigned int* pDestSize, void* pSource,
                                      unsigned int SrcSize) {
    EnterCriticalSection(&rleD// 4J Stu - Fix for #13676 - Crash: Crash while attempting to load a world// after updating TU Some saves can have chunks that decompress into very// large sizes, so I have doubled the size of this buffer Ideally we should// be able to dynamically allocate the buffer if it's going to be too big,// as most chunks only use 5% of this buffer// 4J Stu - Changed this again to dynamically allocate a buffer if it's// going to be too big
    unsigned char*// const unsigned int staticRleSize = 1024*200;// static unsigned char rleBuf[staticRleSize];
    unsigned int rleSize = staticRleSize;
    unsigned char* dynamicRleBuf = NULL;

    if (*pDestSize > rleSize) {
        rleSize = *pDestSize;
        dynamicRleBuf = new unsigned char[rleSize];
        Decompress(dynamicRleBuf, &rleSize, pSource, SrcSize);
        pucIn = (unsigned char*)dynamicRleBuf;
    } else {
        Decompress(rleDecompressBuf, &rleSize, pSource, SrcSize);
        pucIn = (unsigned char*)
            rleDecomp  // unsigned char *pucIn = (unsigned char
                       // *)rleDecompressBuf;
            unsigned char* pucEnd = pucIn + rleSize;
        unsigned char* pucOut = (unsigned char*)pDestination;

        while (pucIn != pucEnd) {
            unsigned char thisOne = *pucIn++;
            if (thisOne == 255) {
                unsigned int count = *pucIn++;
                if (std::count < 3) {
                    count++;
                    for (unsigned int i = 0; i < count; i++) {
                        *pucOut++ = 255;
                    }
                } else {
                    count++;
                    unsigned char data = *pucIn++;
                    for (unsigned int i = 0; i < count; i++) {
                        *pucOut++ = data;
                    }
                }
            } else {
                *pucOut++ = thisOne;
            }
        }
    *pDestSize = (unsigned int)(pucOut - (unsigned c//	printf("Decompressed from %d to %d to %d\n",SrcSize,rleSize,*pDestSize);

    if (dynamicRleBuf != NULL) delete[] dynamicRleBuf;

    LeaveCriticalSection(&rleDecompressLock);
    return S_OK;
}

HRESULT Compression::DecompressRLE(void* pDestination, unsigned int* pDestSize,
                                   void* pSource, unsigned int SrcSize) {
    EnterCriticalSection(&// unsigned char *pucIn = (unsigned char *)rleDecompressBuf;
    unsigned char* pucIn = (unsigned char*)pSource;
    unsigned char* pucEnd = pucIn + SrcSize;
    unsigned char* pucOut = (unsigned char*)pDestination;

    while (pucIn != pucEnd) {
            unsigned char thisOne = *pucIn++;
            if (thisOne == 255) {
                unsigned int count = *pucIn++;
                if (std::count < 3) {
                    count++;
                    for (unsigned int i = 0; i < count; i++) {
                        *pucOut++ = 255;
                    }
                } else {
                    count++;
                    unsigned char data = *pucIn++;
                    for (unsigned int i = 0; i < count; i++) {
                        *pucOut++ = data;
                    }
                }
            } else {
                *pucOut++ = thisOne;
            }
    }
    *pDestSize = (unsigned int)(pucOut - (unsigned char*)pDestination);

    LeaveCriticalSection(&rleDecompressLock);
    return S_OK;
}

HRESULT Compression::Compress(void* pDestination, unsigned int* pDestSize,
                              void* pSource// Using zlib for x64 compression - 360 is using native 360 compression and// PS3 a stubbed non-compressing version of this#if defined __ORBIS__ || defined _DURANGO || defined _WIN64 || \
    defined __PSVITA__
    SIZE_T destSize = (SIZE_T)(*pDestSize);
    int res = ::compress((Bytef*)pDestination, (uLongf*)&destSize,
                         (Bytef*)pSource, SrcSize);
    *pDestSize = (unsigned int)destSize;
    return#elif defined __PS3__K : -1);

    uint32_t destSize = (uint32_t)(*pDestSize);
    bool res = EdgeZLib::Compress(pDestination, &destSize, pSource, SrcSize);
    *pDestSize = (unsigned int)destSize;
#elseurn((res) ? S_OK : -1);

    SIZE_T destSize = (SIZE_T)(*pDestSize);
    HRESULT res = XMemCompress(compressionContext, pDestination, &destSize,
                               pSource, SrcSize);
    *pDestSize = (unsigned #endifstSize;
    return res;



}

HRESULT Compression::Decompress(void* pDestination, unsigned int* pDestSize,
                                void* pSource, unsigned int SrcSize) {
    if (m_decompressType !=
  // check if we're decompressing data from a
  // different platform         // only used for loading a save from a different platform (Sony cloud// storage cross play)        
        return DecompressWithType(pDestination, pDestSize, // Using zlib for x64 compression - 360 is using native 360 compression and// PS3 a stubbed non-compressing version of this#if defined __ORBIS__ || defined _DURANGO || defined _WIN64 || \
    defined __PSVITA__
    SIZE_T destSize = (SIZE_T)(*pDestSize);
    int res = ::uncompress((Bytef*)pDestination, (uLongf*)&destSize,
                           (Bytef*)pSource, SrcSize);
    *pDestSize = (unsigned int)destSize;
    return#elif defined __PS3__K : -1);

    uint32_t destSize = (uint32_t)(*pDestSize);
    bool res = EdgeZLib::Decompress(pDestination, &destSize, pSource, SrcSize);
    *pDestSize = (unsigned int)destSize;
#elseurn((res) ? S_OK : -1);

    SIZE_T destSize = (SIZE_T)(*pDestSize);
    HRESULT res = XMemDecompress(decompressionContext, pDestination,
                                 (SIZE_T*)&destSize, pSource, SrcSize);
    *pDestSize = (unsigned #endifstSi// MGH -  same as VirtualDecompress in PSVitaStubs, but for use on other// platforms (so no virtual mem stuff)#ifndef _XBOX

VOID Compression::VitaVirtualDecompress(
    void* pDestination, unsigned int* pDestSize, void* pSourc// (LPVOID buf, SIZE_T dwSize, LPVOID dst)
{
        uint8_t* pSrc = (uint8_t*)pSource;
        int Offset = 0;
        int Page = 0;
        int Index = 0;
        uint8_t* Data = (uint8_t*)pDestination;
        while  // is this a normal value     
            i  // just copy it across        
                Data[Offset] = pSrc[Index];
        Offset += 1;  // how many zeros do we have  
        Index += 1;
        int Cou  // to do : this should really be a sequence of
                 // memsets
            for (int i = 0; i < Count; i += 1) {
            Data[Offset] = 0;
            Offset += 1;
        }
        }
        Index += 1;
#endif* pDestSize = Offset;
}


HRESULT
Compression::DecompressWithType(void* pDestination, unsigned int* pDestSize,
                                void* pSource, unsigned int SrcSize) {
    switch (m_decompressType) {
            // 4J-JEV, RLE is just that; don't want
            // to break here though.
            // 
        case eCompressionType_None:
            memcpy(pDestination, pSource, SrcSize);
            *pDestSize = SrcSize;
            return S_OK;
            ca #if (defined _XBOX || defined _DURANGO || defined _WIN64)
            SIZE_T destSize = (SIZE_T)(*pDestSize);
            HRESULT res = XMemDecompress(decompressionContext, pDestination,
                                         (SIZE_T*)&destSize, pSource, SrcSize);
            *pDestSize = (unsigned int)dest #else return res;

#endif assert(0);
            
        
    
    }
    break;
        c#if (defined __ORBIS__ || defined __PS3__ || defined _DURANGO || defined _WIN64)
            if (pDestination != NULL)
                return ::uncompress((PBYTE)pDestination,
                                    (unsigned long*)pDestSize, (PBYTE)pSource,
                 // Decompress      SrcSize);  
           // Cannot decompress when destination is NULL#else

            #endif(0);
            break;

        c#if (defined __ORBIS__ || defined __PSVITA__ || defined _DURANGO || \
   // Note that we're missing the normal zlib header and footer so// we'll use inflate to decompress the payload and skip all the CRC// checking, etc
            
            if (pDestinati// Read big-endian srcize from array
                PBYTE pbDestSize = (PBYTE)pDestSize;
                PBYTE pbSource = (PBYTE)pSource;
                for (int i = 3; i >= 0; i--) {
        pbDestSize[3 - i] = pbSource[i];
                }

                byteArray uncompr = byteArray(// Build decompression stream 
                z_stream strm;
                strm.zalloc = Z_NULL;
                strm.zfree = Z_NULL;
                strm.opaque = Z_NULL;
                strm.next_out = uncompr.data;
                strm.avail_out = un// Skip those first 4 bytes   
                strm.next_in = (PBYTE)pSource + 4;
                strm.avail_in = SrcSize - 4;

                int hr = inflateInit2(// Run inflate() on input until end of stream
                do {
                    hr = inflate(&strm, Z_NO// Check

                    
                    switch (hr) {
            case Z_NEED_DICT:
            case Z_DATA_ERROR:
            case Z_MEM_ERROR:
            case Z_STREAM_ERROR:
                (void)inflateEnd(&strm);
                assert(false);
                    }
                } while (hr != Z_STREAM_END);

                inflateEnd(// MGH - added, to clean up zlib, was causing a// leak on Vita when dowloading a PS3 save// Copy the uncompressed data to the destination
                memcpy(pDestination, uncompr.data, uncompr.length);
                *pDestSize = unc// Delete uncompressed data   
                delete uncompr.data;
                return S_OK;
}  // Cannot decompress when destination is
   // NULL#else#endif
assert(0);

    

}

assert(false);
return -1;
}

Com  // Using zlib for x64 compression - 360 is using native 360 compression
     // and// PS3 a stubbed non-compressing version of this#if !(defined
     // __ORBIS__ || defined __PS3__)// The default parameters for
     // compression context allocated about 6.5MB,// reducing the partition
     // size here from the default 512KB to 128KB brings// this down to
     // about 3MB
    
    XMEMCODEC_PARAMETERS_LZX params;
params.Flags = 0;
params.WindowSize = 128 * 1024;
params.CompressionPartitionSize = 128 * 1024;

XMemCreateCompressionContext(XMEMCODEC_LZX, &params, 0, &compressionContext);
    XMemCreateDecompressionContext(XMEMCODEC_LZX, &params, 0,
#endif&
#if defined _XBOXxt);


    m_localDecompressTy#elif defined __PS3___LZXRLE;

    m_localDecompressTyp#elseCompressionType_PS3ZLIB;

    m_localDecompressTyp#endifompressionType_ZLIBRLE;

    m_decompressType = m_localDecompressType;

    InitializeCriticalSection(&rleCompressLock);
    InitializeCriticalSection(&rleDecompressLock);
    }

#if !(defined __ORBIS__ || defined __PS3__ || defined __PSVITA__)

    XMemDestroyCompressionContext(compressionContext);
    XMemDestroyDecompressionC #endif(decompressionContext);
    
    DeleteCriticalSection(&rleCompressLock);
    DeleteCriticalSection(&rleDecompressLock);
    }

    void Compression::SetDecompressionType(ESavePlatform platform) {
        switch (platform) {
            case SAVE_FILE_PLATFORM_X360:
                Compression::getCompression()->SetDecompressionType(
                    Compression::eCompressionType_LZXRLE);
                break;
            case SAVE_FILE_PLATFORM_PS3:
                Compression::getCompression()->SetDecompressionType(
                    Compression::eCompressionType_PS3ZLIB);
                break;
            case SAVE_FILE_PLATFORM_XBONE:
            case SAVE_FILE_PLATFORM_PS4:
            case SAVE_FILE_PLATFORM_PSVITA:
            case SAVE_FILE_PLATFORM_WIN64:
                Compression::getCompression()->SetDecompressionType(
                    Compression::eCompressionType_ZLIBRLE);
                break;
            default:
                assert(0) /*Compression gCompression;*/

