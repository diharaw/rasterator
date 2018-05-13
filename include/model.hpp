#ifndef __MODEL_H__
#define __MODEL_H__

#include <vector>
#include <math/vec3.hpp>

using namespace math;

class Model {
private:
	std::vector<vec3f> verts_;
	std::vector<std::vector<int> > faces_;
public:
	Model(const char *filename);
	~Model();
	int nverts();
	int nfaces();
	vec3f vert(int i);
	std::vector<int>& face(int idx);
};

#endif //__MODEL_H__
