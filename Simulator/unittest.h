#ifndef UNITTEST_H
#define UNITTEST_H

#include <sstream>

#define ARDUINO_LOOP loop();

#define TEST_CHECK_EQUAL(A , B) if (A != B) { int fLine = __LINE__; \
    std::stringstream aStream; \
    aStream << "Failure in line " << fLine << ": " << #A << " = (" << A << ") != " << #B << " = (" << B << ")"; \
    gMainWindowPointer->printToSeriaDisplay(aStream.str().c_str()); \
}

#endif // UNITTEST_H
