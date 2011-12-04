#pragma warning(disable : 4511) // copy constructor could not be generated
#pragma warning(disable : 4512) // assignment operator could not be generated
#pragma warning(disable : 4127) // conditional expression is constant
#pragma warning(disable : 4561) //c:\develop\Divinity2\Libraries\Boost_1_33_1\boost/type_traits/detail/is_mem_fun_pointer_tester.hpp(85) : 
								// warning C4561: '__fastcall' incompatible with the '/clr' option: converting to '__stdcall'
								// -- this occurs on line 85 - 2544
#pragma warning(disable : 4793) // '__asm' : causes native code generation for function 'void NiSinCos(float,float &,float &)' and more
								// 'vararg' : causes native code generation for function 'int NiSprintf(char *,size_t,const char *,...)'
#pragma warning(disable : 4146) // unary minus operator applied to unsigned type, result still unsigned
#pragma warning(disable : 4267)

// WINDOWS
#define _WIN32_WINNT  0x0403
#define NOMINMAX
#include <windows.h>

#define USE_EASTL
#include "STL.h"

// D3DX math library
#include <DX11Includes.h>