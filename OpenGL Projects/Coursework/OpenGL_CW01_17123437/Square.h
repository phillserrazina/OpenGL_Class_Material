#ifndef SQUARE_H
#define SQUARE_H

#include "Shape.h"

class Square : public Shape {
public:
	explicit Square(Vector3 pos, Vector3 rot, Vector3 scale) : Shape(pos, rot, scale);
};

#endif SQUARE_H