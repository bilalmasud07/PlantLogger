// stdafx.h : include file for standard system include files,
// or project specific include files that are used frequently, but
// are changed infrequently
//

#pragma once

#include "targetver.h"

#include <stdio.h>
#include <tchar.h>



// TODO: reference additional headers your program requires here
#include <iostream>

#if defined(UNICODE) || defined(_UNICODE)
#define _tcout std::wcout
#define _tcin std::wcin
#else
#define _tcout std::cout
#define _tcout std::cin
#endif

typedef std::basic_string<TCHAR> _tstring;