#ifndef MAPPING_EXCEPTION_H
#define MAPPIMG_EXCEPTION_H
#include <stdexcept> // stdexcept header file contains runtime_error
#include <string>
namespace tnr {
	using std::runtime_error;

	class mapping_exception : public runtime_error {
	public:
		mapping_exception(): runtime_error("some mapping exception") {};
		mapping_exception(std::string const &message): runtime_error(message.c_str()) {};  
	}; //class mapping_exception
}
#endif