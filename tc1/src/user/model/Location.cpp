#include "user/model/Location.hpp"

Location::Location() : landmark{0}, offset{0} {}

Location::Location(char l, int o) : landmark{l}, offset{o} {}
