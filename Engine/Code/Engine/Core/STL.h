#pragma once

// STL wrapper to either us EASTL or regular STL

#ifndef USE_EASTL

// use regular stl
#include <string>
#include <vector>
#include <map>

#define stl_map std::map 
#define stl_vector std::vector 
#define stl_string std::string 
#define stl_swap std::swap

#define stl_find std::find

#else

#include <EASTL/vector.h>
#include <EASTL/map.h>
#include <EASTL/hash_map.h>
#include <EASTL/string.h>

#define stl_map eastl::map
#define stl_vector eastl::vector
#define stl_string eastl::string
#define stl_swap eastl::swap

#define stl_find eastl::find

#endif