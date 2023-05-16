#ifndef LINE_H_
#define MESH_H_

#include <ngl/Vec3.h>
#include <iostream>

class Line{
public:
Line(ngl::Vec3 _start, ngl::Vec3 _end);
void updateStart(ngl::Vec3 _start);
void updateEnd(ngl::Vec3 _end);
ngl::Vec3 getStart();
ngl::Vec3 getEnd();
void setStart(ngl::Vec3 _start);
void setEnd(ngl::Vec3 _end);
private:
ngl::Vec3 start;
ngl::Vec3 end;
};


#endif