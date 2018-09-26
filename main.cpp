/*
	PVRX2PNG by [big_fury]SiZiOUS
	http://sbibuilder.shorturl.com/
	
	(C) Copyleft 2002, 2009.
	This utility is under GPL licence.
	Please take a look to the README.txt file to get more infos.
	
	This program was written in order to convert PVR-XBOX "PVRX" files from the 
	game Shenmue II on XBOX to the PNG format. 
	In fact theses files are normal DDS encapsuled with PVR (PowerVR) texture 
	header (header that comes from Dreamcast).
	
	This project uses 3 libraries:
		libpng		: http://www.libpng.org/pub/png/libpng.html
		libz			: http://www.zlib.net/
		libsquish	: http://code.google.com/p/libsquish/
	Libraries are included in the ./deps/ directory.
	
	This program is based on squishpng sample by Simon Brown (included with
	libsquish).
	
	Some portions are coming from :
		nut2png by sabishirop	: http://imas.890m.com/		
		aorta by Gregory Smith	: http://aorta.sourceforge.net/
	
	Original copyright notice is as follows.
	Thanks to these authors for their great work.
	---------------------------------------------------------------------------
*/
/*
	nut2png: Convert Numco Texture file(.nut) to png image.	
	Copyright (c) 2008 sabishirop <sabishirop@gmail.com>
*/
/*
	Aorta is a utility for creating high quality DDS textures.

	Aorta is licensed under the GNU Public License. See the file COPYING for
	details.

	Aorta is copyright 2006-2008 by Gregory Smith

	Special thanks go to:
	Simon Brown for his squish library
	Ignacio Castano for open source nvidia texture tools
*/
/* 
	---------------------------------------------------------------------------

	Copyright (c) 2006 Simon Brown                          si@sjbrown.co.uk

	Permission is hereby granted, free of charge, to any person obtaining
	a copy of this software and associated documentation files (the 
	"Software"), to	deal in the Software without restriction, including
	without limitation the rights to use, copy, modify, merge, publish,
	distribute, sublicense, and/or sell copies of the Software, and to 
	permit persons to whom the Software is furnished to do so, subject to 
	the following conditions:

	The above copyright notice and this permission notice shall be included
	in all copies or substantial portions of the Software.

	THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS
	OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF 
	MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
	IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY 
	CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, 
	TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE 
	SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
	
   ----------------------------------------------------------------------------
*/
   
/*! @file

	@brief	Example program that converts between the PNG and DXT formats.
	
	This program requires libpng for PNG input and output, and is designed
	to show how to prepare data for the squish library when it is not simply
	a contiguous block of memory.
*/

#define APP_VERSION "0.1"

#include "filehead.hpp"

#include <iostream>
#include <string>
#include <sstream>
#include <ctime>
#include <cmath>
#include <squish.h>
#include <png.h>

#ifdef _MSC_VER
#pragma warning( disable: 4511 4512 )
#endif // def _MSC_VER

using namespace squish;

// Specify if the uses want to force decompression algorithm (as in nut2png)
typedef enum {
  UNCOMPRESSED,
  DXT1,
  DXT3,
  DXT5,
  UNDEFINED
} compression_type;

// determinates file type.
// PVR-X is typically: [GBIX header] ... [PVRT header] ... [DDS header] ... [DDS data]
typedef enum {
	UNDEF,
	DDS,
	PVRT,
	GBIX
} file_type;

//! Simple exception class.
class Error : public std::exception
{
public:
	Error( std::string const& excuse ) : m_excuse( excuse ) {}
	~Error() throw() {}
	
	virtual char const* what() const throw() { return m_excuse.c_str(); }
	
private:
	std::string m_excuse;
};

//! Base class to make derived classes non-copyable
class NonCopyable
{
public:
	NonCopyable() {}
	
private:
	NonCopyable( NonCopyable const& );
	NonCopyable& operator=( NonCopyable const& );
};

//! Memory object.
class Mem : NonCopyable
{
public:
	explicit Mem( int size ) : m_p( new u8[size] ) {}
	
/*	explicit Mem( int size ) {		
		std::cout << "size: " << size << std::endl;		
		m_p = new u8[size];
	}
*/
	
	~Mem() { delete[] m_p; }
	
	u8* Get() const { return m_p; }
	
private:
	u8* m_p;
};

//! File object.
class File : NonCopyable
{
public:
	explicit File( FILE* fp ) : m_fp( fp ) {}
	~File() { if( m_fp ) fclose( m_fp ); }
	
	bool IsValid() const { return m_fp != 0; }
	FILE* Get() const { return m_fp; }

private:
	FILE* m_fp;
};

//! PNG read object.
class PngReadStruct : NonCopyable
{
public:
	PngReadStruct()
	  : m_png( 0 ), 
		m_info( 0 ), 
		m_end( 0 )
	{
		m_png = png_create_read_struct( PNG_LIBPNG_VER_STRING, 0, 0, 0 );
		if( !m_png )
			throw Error( "failed to create png read struct" );	
			
		m_info = png_create_info_struct( m_png );
		m_end = png_create_info_struct( m_png );
		if( !m_info || !m_end )
		{
			png_infopp info = m_info ? &m_info : 0;
			png_infopp end = m_end ? &m_end : 0;
			png_destroy_read_struct( &m_png, info, end );
			throw Error( "failed to create png info structs" );
		}
	}
	
	~PngReadStruct() 
	{ 
		png_destroy_read_struct( &m_png, &m_info, &m_end );
	}

	png_structp GetPng() const { return m_png; }
	png_infop GetInfo() const { return m_info; }

private:
	png_structp m_png;
	png_infop m_info, m_end;
};

//! PNG write object.
class PngWriteStruct : NonCopyable
{
public:
	PngWriteStruct()
	  : m_png( 0 ), 
		m_info( 0 )
	{
		m_png = png_create_write_struct( PNG_LIBPNG_VER_STRING, 0, 0, 0 );
		if( !m_png )
			throw Error( "failed to create png read struct" );	
			
		m_info = png_create_info_struct( m_png );
		if( !m_info )
		{
			png_infopp info = m_info ? &m_info : 0;
			png_destroy_write_struct( &m_png, info );
			throw Error( "failed to create png info structs" );
		}
	}
	
	~PngWriteStruct()
	{
		png_destroy_write_struct( &m_png, &m_info );
	}
	
	png_structp GetPng() const { return m_png; }
	png_infop GetInfo() const { return m_info; }

private:
	png_structp m_png;
	png_infop m_info;
};

//! PNG rows object.
class PngRows : NonCopyable
{
public:
	PngRows( int width, int height, int stride ) : m_width( width ), m_height( height )
	{
		m_rows = ( png_bytep* )malloc( m_height*sizeof( png_bytep ) );
		for( int i = 0; i < m_height; ++i )
			m_rows[i] = ( png_bytep )malloc( m_width*stride );
	}
	
	~PngRows() 
	{
		for( int i = 0; i < m_height; ++i )
			free( m_rows[i] );
		free( m_rows );
	}
	
	png_bytep* Get() const { return m_rows; }
	
private:
	png_bytep* m_rows;
	int m_width, m_height;
};

//! PNG image.
class PngImage
{
public:
	explicit PngImage( std::string const& fileName );

	int GetWidth() const { return m_width; }
	int GetHeight() const { return m_height; }
	int GetStride() const { return m_stride; }
	bool IsColour() const { return m_colour; }
	bool IsAlpha() const { return m_alpha; }
	
	u8 const* GetRow( int row ) const { return ( u8* )m_rows[row]; }

private:
	PngReadStruct m_png;

	int m_width;
	int m_height;
	int m_stride;
	bool m_colour;
	bool m_alpha;
	
	png_bytep* m_rows;
};

PngImage::PngImage( std::string const& fileName )
{
	// open the source file
	File file( fopen( fileName.c_str(), "rb" ) );
	if( !file.IsValid() )
	{
		std::ostringstream oss;
		oss << "failed to open \"" << fileName << "\" for reading";
		throw Error( oss.str() );
	}
	
	// check the signature bytes
	png_byte header[8];
	fread( header, 1, 8, file.Get() );
	if( png_sig_cmp( header, 0, 8 ) )
	{
		std::ostringstream oss;
		oss << "\"" << fileName << "\" does not look like a png file";
		throw Error( oss.str() );
	}
	
	// read the image into memory
	png_init_io( m_png.GetPng(), file.Get() );
	png_set_sig_bytes( m_png.GetPng(), 8 );
	png_read_png( m_png.GetPng(), m_png.GetInfo(), PNG_TRANSFORM_EXPAND, 0 );

	// get the image info
	png_uint_32 width;
	png_uint_32 height;
	int bitDepth;
	int colourType;
	png_get_IHDR( m_png.GetPng(), m_png.GetInfo(), &width, &height, &bitDepth, &colourType, 0, 0, 0 );
	
	// check the image is 8 bit
	if( bitDepth != 8 )
	{
		std::ostringstream oss;
		oss << "cannot process " << bitDepth << "-bit image (bit depth must be 8)";
		throw Error( oss.str() );
	}
	
	// save the info
	m_width = width;
	m_height = height;
	m_colour = ( ( colourType & PNG_COLOR_MASK_COLOR ) != 0 );
	m_alpha = ( ( colourType & PNG_COLOR_MASK_ALPHA ) != 0 );
	m_stride = ( m_colour ? 3 : 1 ) + ( m_alpha ? 1 : 0 );

	// get the image rows
	m_rows = png_get_rows( m_png.GetPng(), m_png.GetInfo() );
	if( !m_rows )
		throw Error( "failed to get image rows" );
}

// This function is a tool to change the file extension.
std::string changeFileExt( std::string const& fileName, std::string newExt ) {
	std::string s;
	s = fileName;
	
	// keep only the filename
	size_t pos = s.find_last_of( '\\' );
	if (pos != std::string::npos)
		s = s.substr( pos + 1, s.size() - pos );

	// replace the extension
	size_t ext_pos = s.find_last_of( '.' );
	if (ext_pos != std::string::npos)
		s.replace( ext_pos, s.size() - 1, newExt );
		
	std::ostringstream oss;
	oss << s;
	return oss.str();
}

// Launch the decompression job.
static void Decompress( std::string const& sourceFileName, std::string const& targetFileName, compression_type method )
{
	// to pass to the libsquish Decode function
	int flags = kDxt1;
	bool uncompressed = false;
	
	// open the source file
	File sourceFile( fopen( sourceFileName.c_str(), "rb" ) );
	if( !sourceFile.IsValid() )
	{
		std::ostringstream oss;
		oss << "failed to open \"" << sourceFileName << "\" for reading";
		throw Error( oss.str() );
	}
	
    // checking signature
	file_type ftype = UNDEF;
    char buf[4];
	std::ostringstream oss;
	std::string fileFormat = "(Undef)";
	
    if(fread(buf, sizeof(buf), 1, sourceFile.Get()) > 0) {		
					
		if (memcmp(buf, DDS_SIGNATURE, sizeof(buf)) == 0) {
			ftype = DDS;
			fileFormat = "DDS";
		} else if (memcmp(buf, PVRT_SIGNATURE, sizeof(buf)) == 0) {
			ftype = PVRT;
			fileFormat = "PVR-X";
		} else if (memcmp(buf, GBIX_SIGNATURE, sizeof(buf)) == 0) {
			ftype = GBIX;
			fileFormat = "GBIX / PVR-X";
		}
		
		// Undefined...
		if (ftype == UNDEF) {
			oss << "Sorry, invalid signature (found: \"" << buf << "\" instead of \"" << DDS_SIGNATURE << "\")";
			throw Error( oss.str() );
		}
		
    } else {
		oss << "Sorry, unable to read file signature";
		throw Error( oss.str() );
	}
	
#ifdef DEBUG
	std::cout << "FILE FORMAT = " << fileFormat << std::endl;
#endif

	// parsing file signature	
	GBIX_HEADER gbix;
	PVRT_HEADER pvr;
	
	// reading GBIX header
	int globalIndex = -1;
	if (ftype == GBIX) {
		fread( &gbix, sizeof(gbix), 1, sourceFile.Get() );
		globalIndex = gbix.dwGlobalIndex;
	}
	
	// reading PVR header
	if (ftype == GBIX || ftype == PVRT) {		
		// pass the "PVRT" header...
		if (ftype == GBIX)
			fread( &buf, sizeof(buf), 1, sourceFile.Get() );
			
		fread( &pvr, sizeof(pvr), 1, sourceFile.Get() );
	}
	
	// pass the "DDS " header
	if (ftype != DDS) {
		fread( &buf, sizeof(buf), 1, sourceFile.Get() );
		// if the DDS signature is not found, then it's a REAL GBIX / PVR file!
		if (memcmp(buf, DDS_SIGNATURE, sizeof(buf)) != 0) {
			oss << "Sorry, this file is a real Dreamcast PowerVR (PVR) file !";
			throw Error( oss.str() );
		}
	}
			
	// reading DDS header
	DDSURFACEDESC2 header;
	fread( &header, sizeof(header), 1, sourceFile.Get() );
	
	// getting the compression algorithm.
	std::ostringstream pixelFormat;
	
	// let the proggy determines the compression algorithm
	if (method == UNDEFINED) {
	
		// DXT2 and DXT4 is not supported... may be?
		if (
			header.ddpfPixelFormat.dwFourCC == DXT2_SIGN ||
			header.ddpfPixelFormat.dwFourCC == DXT4_SIGN
		) {
			std::ostringstream oss;
			oss << "sorry, this DDS compression is not supported";
			throw Error( oss.str() );
		}
		
		// OK we know how to do with theses format
		if (header.ddpfPixelFormat.dwFourCC == DXT1_SIGN) {
			pixelFormat << "DXT1";
			flags = kDxt1;
		} else if (header.ddpfPixelFormat.dwFourCC == DXT3_SIGN) {
			pixelFormat << "DXT3";
			flags = kDxt3;
		} else if (header.ddpfPixelFormat.dwFourCC == DXT5_SIGN) {
			pixelFormat << "DXT5";		
			flags = kDxt5;
		} else
			uncompressed = true;
		
	} else {
	
		// the compression algorithm is forced by the user
		switch( method ) {
			case UNCOMPRESSED:
				pixelFormat << "Uncompressed";
				uncompressed = true;
				break;
				
			case DXT1:
				pixelFormat << "DXT1";
				flags = kDxt1;
				break;
				
			case DXT3:
				pixelFormat << "DXT3";
				flags = kDxt3;
				break;
				
			case DXT5:
				pixelFormat << "DXT5";				
				flags = kDxt5;
				
			default:
				break;
		}
	}
	
	// get the width and height
	int width, height;
	width = header.dwWidth;
	height = header.dwHeight;		
	
	// work out the data size
	int bytesPerBlock = 0;
	if (uncompressed)
		bytesPerBlock = 64; // from nut2png
	else	
		bytesPerBlock = ( ( flags & kDxt1 ) != 0 ) ? 8 : 16;
		
	int sourceDataSize = bytesPerBlock * width * height / 16;	
	Mem sourceData( sourceDataSize );
	
	// display infos before proceeding
	std::cout 
		<< "Input File   : " << sourceFileName << std::endl
		<< "Output File  : " << targetFileName << std::endl
		<< "File Format  : " << fileFormat << std::endl
	;
	
	if (ftype == GBIX)
		std::cout 
		<< "Global Index : " << globalIndex << std::endl;
		
	std::cout
		<< "Image Size   : " << width << "x" << height << std::endl
		<< "Data Size    : " << sourceDataSize << std::endl
		<< "Pixel Format : " << pixelFormat.str() << std::endl << std::endl
		<< "Converting..."
	;
	
	// read the source data
	fread( sourceData.Get(), 1, sourceDataSize, sourceFile.Get() );
		
	// create the target rows
	PngRows targetRows( width, height, 4 );
			
	// loop over blocks and compress them
	u8 const* sourceBlock = sourceData.Get();
	
	// handling uncompressed DDS
	if (uncompressed) {
	
		// from nut2png program
	    for( int y = 0; y < height; y++ ) {
			for( int x = 0; x < width; x++ ){
			    u8* row = ( u8* )targetRows.Get()[y] + x*4;
			    //ARGB->RGBA
			    *row++ = sourceBlock[(y * width + x) * 4 + 1];
			    *row++ = sourceBlock[(y * width + x) * 4 + 2];
			    *row++ = sourceBlock[(y * width + x) * 4 + 3];
			    *row++ = sourceBlock[(y * width + x) * 4 + 0];
			}
	    }
		
	} else {
	
		// handling compressed DDS
		for( int y = 0; y < height; y += 4 )
		{
			// process a row of blocks
			for( int x = 0; x < width; x += 4 )
			{
				// decompress back
				u8 targetRgba[16*4];
				
				Decompress( targetRgba, sourceBlock, flags );
				
				// write the data into the target rows
				for( int py = 0, i = 0; py < 4; ++py )
				{
					u8* row = ( u8* )targetRows.Get()[y + py] + x*4;
					for( int px = 0; px < 4; ++px, ++i )
					{	
						for( int j = 0; j < 4; ++j )
							*row++ = targetRgba[4*i + j];
					}
				}
				
				// advance
				sourceBlock += bytesPerBlock;
			}
		}
	}
	
	// create the target PNG
	PngWriteStruct targetPng;

	// set up the image
	png_set_IHDR(
		targetPng.GetPng(), targetPng.GetInfo(), width, height,
		8, PNG_COLOR_TYPE_RGBA, PNG_INTERLACE_NONE,
		PNG_COMPRESSION_TYPE_DEFAULT, PNG_FILTER_TYPE_DEFAULT 
	);
	   
	// open the target file
	File targetFile( fopen( targetFileName.c_str(), "wb" ) );
	if( !targetFile.IsValid() )
	{
		std::ostringstream oss;
		oss << "failed to open \"" << targetFileName << "\" for writing";
		throw Error( oss.str() );
	}
	
	// write the image
	png_set_rows( targetPng.GetPng(), targetPng.GetInfo(), targetRows.Get() );
	png_init_io( targetPng.GetPng(), targetFile.Get() );
	png_write_png( targetPng.GetPng(), targetPng.GetInfo(), PNG_TRANSFORM_IDENTITY, 0 );
	
	std::cout << "Done.";
}

// show the help
void printHelp( char* prgname_c ) {
	std::string prgname;
	prgname.assign( changeFileExt(prgname_c, "") );
	
//            --------------------------------------------------------------------------------

	std::cout 
		<< "This converter was made to convert PVR-X texture files to the PNG format. It " << std::endl
		<< "was originally made for the XBox version of the game Shenmue II." << std::endl
		<< std::endl
		<< "Usage:" << std::endl
		<< "  " << prgname << " [-0135h] <source.pvr> [target.png]" << std::endl << std::endl
		<< "Options:" << std::endl
		<< "  -h   : This help." << std::endl
		<< "  -0135: Forces the use of the following decompression algorithm: " << std::endl
		<< "           -0: Uncompressed" << std::endl
		<< "           -1: DXT1" << std::endl 
		<< "           -3: DXT3" << std::endl
		<< "           -5: DXT5" << std::endl
		<< "Examples:" << std::endl
		<< "  " << prgname << " big_fury.pvr big_fury.png" << std::endl
		<< "    This will decode \"big_fury.pvr\" and write the output to \"big_fury.png\"" << std::endl << std::endl
		<< "  " << prgname << " -1 sizious.pvr" << std::endl
		<< "    This will decode \"sizious.pvr\" but force the use of the DXT1 algorithm." << std::endl
		;
}

// show the banner
void printBanner( void ) {
	std::cout
		<< "PVRX2PNG - Version " << APP_VERSION << " (" << __DATE__ << ") - (C)reated by [big_fury]SiZiOUS" << std::endl
		<< "http://sbibuilder.shorturl.com/" << std::endl 
		<< std::endl
	;
	
#ifdef DEBUG
	std::cout
		<< "*** DEBUG BUILD ***" << std::endl
		<< std::endl
	;
#endif
}

// entry point
int main( int argc, char* argv[] ) {
	printBanner();
	
	try
	{
		// parse the command-line
		std::string sourceFileName;
		std::string targetFileName;
		compression_type method = UNDEFINED;
		bool help = false;
		bool arguments = true;
							
		for( int i = 1; i < argc; ++i )
		{
			// check for options
			char const* word = argv[i];
			
			if( arguments && word[0] == '-' )
			{
				for( int j = 1; word[j] != '\0'; ++j )
				{
					switch( word[j] )
					{
					case 'h': help = true; break;
					case '0': method = UNCOMPRESSED; break;
					case '1': method = DXT1; break;
					case '3': method = DXT3; break;
					case '5': method = DXT5; break;
					case '-': arguments = false; break;
					default:
						std::cerr << "Error: Unknown option \"" << word[j] << "\"." << std::endl;
						return -1;
					}
				}
			}
			else
			{
				if( sourceFileName.empty() )
					sourceFileName.assign( word );
				else if( targetFileName.empty() )
					targetFileName.assign( word );
				else
				{
					std::cerr << "Error: Unexpected argument \"" << word << "\"." << std::endl;
				}
			}
		}
		
		// check arguments
		if( help || sourceFileName.empty() ) {
			printHelp( argv[0] );
			return 0;
		}
						
/*		if( sourceFileName.empty() )
		{
			std::cerr << "Error: No source file given." << std::endl;
			return -1;
		}
*/
		
		if( targetFileName.empty() ) {
			targetFileName.assign( changeFileExt(sourceFileName, ".png") );
		}
		
		// do the work
		Decompress( sourceFileName, targetFileName, method );
		std::cout << std::endl;		
	}
	catch( std::exception& excuse ) {
		// complain
		std::cerr << "Error: " << excuse.what() << std::endl;
		return -1;
	}
	
	// done
	return 0;
}
