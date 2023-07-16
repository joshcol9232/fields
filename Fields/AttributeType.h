#pragma once

#include <ostream>

// Used to check Bodies have corresponding attributes avaialble.
enum eAttributeType {
  eNoAttribute,
  eGravityType,
  eChargeType,
};

#define PRINT_EATTR(EATTR) \
  case EATTR: \
    os << #EATTR; \
    break; \

void print_attr_type(std::ostream& os, eAttributeType t);

