#include "Line.h"
#include <ngl/ShaderLib.h>
#include <iostream>
Line::Line(ngl::Vec3 _start, ngl::Vec3 _end)
{
    start = _start;
    end = _end;
 }
void Line::updateStart(ngl::Vec3 _start){
    start = _start;
}
void Line::updateEnd(ngl::Vec3 _end){
    end = _end;
}
ngl::Vec3 Line::getStart(){
    return start;
}
ngl::Vec3 Line::getEnd(){
    return end;
}
void Line::setStart(ngl::Vec3 _start){
    start = _start;
}
void Line::setEnd(ngl::Vec3 _end){
    end = _end;
}