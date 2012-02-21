#pragma once

// STL wrapper to either us EASTL or regular STL

#ifndef USE_EASTL

// use regular stl
#include <string>
#include <vector>
#include <map>
#include <set>
#include <list>

#define stl_map std::map 
#define stl_vector std::vector 
#define stl_string std::string 
#define stl_wstring std::wstring 
#define stl_set std::set
#define stl_list std::list

#define stl_swap std::swap
#define stl_find std::find
#define stl_sort std::sort
#define stl_transform std::transform

#else

#include <EASTL/vector.h>
#include <EASTL/map.h>
#include <EASTL/hash_map.h>
#include <EASTL/string.h>
#include <EASTL/set.h>
#include <EASTL/list.h>

#define stl_map eastl::map
#define stl_vector eastl::vector
#define stl_string eastl::string
#define stl_wstring eastl::wstring
#define stl_set eastl::set
#define stl_list eastl::list

#define stl_swap eastl::swap
#define stl_find eastl::find
#define stl_sort eastl::sort
#define stl_transform eastl::transform

#endif

typedef stl_vector<stl_string> TStringVector;
typedef stl_set<stl_string> TStringSet;