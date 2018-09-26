            ______              ___ ___ ______ ______              
           |   __ \.--.--.----.|   |   |__    |   __ \.-----.-----.
           |    __/|  |  |   _||-     -|    __|    __/|     |  _  |
           |___|    \___/|__|  |___|___|______|___|   |__|__|___  |
                                                    PvrX2Png|_____|
         
  Welcome to PvrX2Png. This tool allow you to convert PVR-X texture images to
  the PNG format. PVR-X files are textures for some games on the Microsoft 
  XBox console plateform (the old one, not the 360° stuff).
  
  This tool supports PVR-X in three formats:
    - GBIX / PVR-X (PVR-X with GBIX header)
    - PVR-X (PVR-X without GBIX header)
    - DDS (PVR-X without any PVR format)
	
  See the rest of this document to learn more about this tool.
      ___                  _      __  _         
     / _ \___ ___ ________(_)__  / /_(_)__  ___ 
___ / // / -_|_-</ __/ __/ / _ \/ __/ / _ \/ _ \_______________________________
   /____/\__/___/\__/_/ /_/ .__/\__/_/\___/_//_/
                         /_/ I. DESCRiPTiON        
						 
  In reality, the PVR-X format doesn't exist... Huh? Wtf ? Let me explain this
  to you:
  
  The PVR-X format is used in some games on the Microsoft XBox console. When
  I said "some games", in fact, I'm thinking of one of them, in particular:
  Shenmue II.
  
  In this game, which is a port from the Sega Dreamcast plateform, you can 
  find some texture files with the .PVR extension. The .PVR extension 
  identifies a PowerVR texture, which is the official Sega Dreamcast texture 
  format (the NEC PowerVR is the GPU of the Sega Dreamcast).
  
  But when you tries to convert them in the PNG format using Pvr2Png 1.01 by
  Gzav (which can easilly found in MegaGames for example), it doesn't work. 
  Why ?
  
  Because theses files, with the .PVR extension, seems like the "original"
  Dreamcast PowerVR format, but they are NOT! Yes, they have the PVRT (and/or 
  the optional GBIX) header, but the texture data aren't the PVR format: It's 
  the real Microsoft XBox format, you know it: The DDS (for Direct Draw 
  Surface, the same as DirectX). Yeah, Shenmue II was a Dreamcast conversion,
  so what did you expect ?
  
  I called this special .PVR texture format the "PVR-X" for "PVR-XBOX".
  
  So this proggy was made to decode them, in order to have a command line
  converter like Pvr2Png by Gzav. These converters (this one and Pvr2Png) are 
  used in the Shenmue Translation Pack suite, which can found on this URL:
  
	             http://shenmuesubs.sourceforge.net/
  
  You know all the story.
    __  __                 
   / / / /__ ___ ____ ____ 
__/ /_/ (_-</ _ `/ _ `/ -_)____________________________________________________
  \____/___/\_,_/\_, /\__/ 
      II. USAGE /___/      

  The usage of this tool is pretty simple:
  
  1. Get a PVR-X texture from somewhere (randomly in the Shenmue II game).
  
  2. Type the following:
     pvrx2png my_tex.pvr
	 
  3. This will generate a my_tex.png file which contains your decoded texture.
  
  The detailled usage is the following:
    pvrx2png [-0135h] <source.pvr> [target.png]

  Options:
    -h   : This help.
    -0135: Forces the use of the following decompression algorithm:
             -0: Uncompressed
             -1: DXT1
             -3: DXT3
             -5: DXT5
  Examples:
    pvrx2png big_fury.pvr big_fury.png
      This will decode "big_fury.pvr" and write the output to "big_fury.png"

    pvrx2png -1 sizious.pvr
      This will decode "sizious.pvr" to "sizious.png" but force the use of 
	  the DXT1 algorithm (instead of detecting it).
	  	 
  Pretty simple, huh ?
     _____                _ ___          
    / ___/__  __ _  ___  (_) (_)__  ___ _
___/ /__/ _ \/  ' \/ _ \/ / / / _ \/ _ `/______________________________________
   \___/\___/_/_/_/ .__/_/_/_/_//_/\_, /                                         
  III. COMPiLiNG /_/              /___/  

  To compile this awesome proggy, you will need:
    - MinGW compiler (I'm using the v5.1.4)
    - MSYS installed (for the RM command, optionnal, I'm using the v1.0.11).
    - MinGW Make tool (mingw32-make)
    - UPX (Ultimate Packer for eXecutables, I'm using the v3.02).
    - Some external libraries (see below)
    - If you want to do some changes, a cool text editor (I'm using the nice 
      Notepad++ v5.4.5)
    - A working brain.
  
  Recipe to compile: 
  
  1. Unzip this source package in a folder (yeah, that's already done, it 
     isn't?)
	 
  2. Double-click on the "build.cmd" script.
  
  3. Please wait a moment and look at the "bin/" folder. 
  
  4. Serve cold with ice.
  
  This project uses three libraries in order to compile, which are:
    - libz (used by libpng)
    - libpng (for PNG writing)
    - libsquish (for DDS decoding)
 
  These libraries are in the "./deps/" folder. You don't have to download them
  because they are already there. If you want to update them, you can, by 
  following this procedure:
  
  1. Go to the official library website and get it.
  
  2. Unzip the content of the library in the appropriate "./deps/<YOUR_LIB>/"
     folder, overwrite everything.
	 
  3. Then rebuild.
  
  Currently, libraries versions are:
    - libpng-1.2.39
    - libsquish-1.10
    - libz-1.2.3
  
  That's all folks.
     _____           ___ __    
    / ___/______ ___/ (_) /____
___/ /__/ __/ -_) _  / / __(_-<________________________________________________
   \___/_/  \__/\_,_/_/\__/___/ IV. CREDiTS
                         
Main code : 
  [big_fury]SiZiOUS (http://sbibuilder.shorturl.com/)

Thanks flying to the authors of these libraries :
  - libpng         : http://www.libpng.org/pub/png/libpng.html
  - libz           : http://www.zlib.net/
  - libsquish      : http://code.google.com/p/libsquish/

Thanks (also) to:
  - Simon Brown for the libsquish library and the squishpng sample.
  - sabishirop (http://imas.890m.com/) for his/her nut2png tool. I took some 
    code / idea from it.
  - Gregory Smith (http://aorta.sourceforge.net/) for his "aorta" tool which 
    helped me to understand the DDS format, and for some code.
_____________________________________________________________________/ EOF /___
