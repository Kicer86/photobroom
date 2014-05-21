Linux:
        1. gcc.
                At least 4.8.0 version is required (due to c++11).
                Qt 5.0 is minimum.
                Gcc has support for OpenMP which will be used for various tasks.
                
        2. clang.
                

Microsoft Windows build:
	1. Visual studio.
		Visual Studio 2013 at least is required with Visual C++ Compiler Nov 2013 CTP (CTP_Nov2013) (due to c++11).
		Qt 5.3 (first one with support for Visual Studio 2013).
		It looks like Qt has some issue - in QCompilerDetection.h: line:
		#    if _MSC_FULL_VER >= 180030324 // VC 12 SP 2 RC
		needs to be replaced with:
		#    if _MSC_FULL_VER >= 180021050 // VC 12 SP 2 RC
