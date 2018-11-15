#include "Robot36.hpp"

void Robot36::asyncDecode(std::string filename) {

}

void Robot36::encode(std::string filename) {
	encoder.encode(filename, "default");
}
