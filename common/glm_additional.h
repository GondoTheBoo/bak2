#pragma once

#include "glm.h"
#include <iostream>

inline std::ostream & operator<<(std::ostream &o, glm::vec2 val)
{
	o << "( " << val.x << " | " << val.y << " )";
	return o;
}

inline std::ostream & operator<<(std::ostream &o, glm::vec3 val)
{
	o << "( " << val.x << " | " << val.y << " | " << val.z << " )";
	return o;
}

inline std::ostream & operator<<(std::ostream &o, glm::vec4 val)
{
	o << "( " << val.x << " | " << val.y << " | " << val.z << " | " << val.w << " )";
	return o;
}