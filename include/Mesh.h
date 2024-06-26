#ifndef MESH_H_
#define MESH_H_

#include <memory>
#include <ngl/Vec3.h>
#include <iostream>
#include "Line.h"

class Mesh{
public:
Mesh(int _numlines);
std::vector<ngl::Vec3> getChain();
std::vector<ngl::Vec3> getSup();
void updateChain();
void setChainLink(int _index, bool _start, ngl::Vec3 _val);
ngl::Vec3  getChainLink(int _index, bool _start);
float hookeForcey(int _index);
float hookeForcex(int _index);
ngl::Vec3 hookeForce(int _index);
ngl::Vec3 supForce(int _index);
void setVel(float _vel, int _index, int _dir);
void setBeg(float _vel, int _index, int _dir);
void checkLength(int _index);
void checkSide(int _index);
void setSupB();
private:
std::vector <Line> chain;
std::vector <Line> support;
std::vector <ngl::Vec3> velocity;
std::vector <ngl::Vec3> acceleration;
std::vector <ngl::Vec3> force;

std::vector <ngl::Vec3> supvelocity;
std::vector <ngl::Vec3> supacceleration;
std::vector <ngl::Vec3> supforce;
static constexpr float mass = 1;
ngl::Vec3 begin;
bool supB = false;
};


#endif