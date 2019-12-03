#include "user/model/Location.hpp"

Location::Location() : landmark{(char)-1}, offset{0} {}

Location::Location(char l, int o) : landmark{l}, offset{o} {}
