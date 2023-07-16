#include "AttributeType.h"

void print_attr_type(std::ostream& os, eAttributeType t) {
  switch (t) {
    PRINT_EATTR(eNoAttribute)
    PRINT_EATTR(eGravityType)
    PRINT_EATTR(eChargeType)
    default:
      os << "NOT A TYPE" << std::endl;
      break;
  }
}
