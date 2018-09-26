#ifndef __DDSHEAD__HPP__
#define __DDSHEAD__HPP__

#define DDS_SIGNATURE "DDS \0"
#define PVRT_SIGNATURE "PVRT\0"
#define GBIX_SIGNATURE "GBIX\0"

#define DXT1_SIGN 0x31545844
#define DXT2_SIGN 0x32545844
#define DXT3_SIGN 0x33545844
#define DXT4_SIGN 0x34545844
#define DXT5_SIGN 0x35545844

#define DWORD unsigned int

// GBIX header
struct GBIX_HEADER {
	DWORD dwReserved;
	DWORD dwGlobalIndex;
};

// PVR header
struct PVRT_HEADER {
	DWORD dwDataSize;
	DWORD dwUnknow;
	short int Width;
	short int Height;
};

// DDS header
struct DDSURFACEDESC2
{
    DWORD dwSize;
    DWORD dwFlags;
    DWORD dwHeight;
    DWORD dwWidth;
    DWORD dwPitchOrLinearSize;
    DWORD dwDepth;
    DWORD dwMipMapCount;
    DWORD dwReserved1[11];
    
    struct {
		DWORD dwSize;
		DWORD dwFlags;
		DWORD dwFourCC;
		DWORD dwRGBBitCount;
		DWORD dwRBitMask;
		DWORD dwGBitMask;
		DWORD dwBBitMask;
		DWORD dwRGBAlphaBitMask;
    } ddpfPixelFormat;
    
    struct {
		DWORD dwCaps1;
		DWORD dwCaps2;
		DWORD Reserved[2];
    } ddsCaps;
    
    DWORD dwReserved2;
};

#endif // __DDSHEAD__HPP__
