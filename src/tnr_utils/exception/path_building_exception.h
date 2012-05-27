#ifndef PATH_BUILDING_EXCEPTION_H
#define PATH_BUILDING_EXCEPTION_H
#include <stdexcept> // stdexcept header file contains runtime_error
#include <string>

namespace tnr {
	class path_building_exception : public runtime_error {
	public:
		path_building_exception(): runtime_error("exception in path building process") {};
		path_building_exception(std::string const &message): runtime_error(message.c_str()) {};  
	}; //class path_building_exception
}

#endif