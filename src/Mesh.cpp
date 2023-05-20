#include "Mesh.h"
#include <ngl/ShaderLib.h>
#include <iostream>

Mesh::Mesh(int _numlines)
{   
    ngl::Vec3 start = {0.0f,0.0f,0.0f};
    ngl::Vec3 end = {0.0f,-0.5,0.0f};
    ngl::Vec3 begin = {0.0,0.0,0.0};
    for(float i = 0; i<_numlines; i+=1.0f){
        chain.push_back(Line(start, end));
        // std::cout << "Start = " << start.m_y;
        // std::cout << "End = " << end.m_y;
        velocity.push_back({0,0,0});
        acceleration.push_back({0,0,0});
        force.push_back({0,0,0});
        start.m_y = -i/2.0f;
        end.m_y = (-i/2.0f)-0.5;
        ngl::Vec3 offset = {0,0.25,0};
        supforce.push_back({0,0,0});
        if(i!= 0){
            support.push_back(Line(start, end));
            supvelocity.push_back({0,0,0});
            supacceleration.push_back({0,0,0});
        }
    }
 }

std::vector<ngl::Vec3> Mesh::getChain(){
    std::vector<ngl::Vec3> ret;
    for(int i = 0; i<chain.size(); ++i){
        ret.push_back(chain[i].getStart());
        ret.push_back(chain[i].getEnd());
    }
    return ret;
}

std::vector<ngl::Vec3> Mesh::getSup(){
    std::vector<ngl::Vec3> ret;
    for(int i = 0; i<support.size(); ++i){
        ret.push_back(support[i].getStart());
        ret.push_back(support[i].getEnd());
    }
    return ret;
}

void Mesh::updateChain(){

    for(int i = 0; i<chain.size(); i++){
        checkLength(i);

        if(i!= 0){
            support[i-1].setStart(chain[i-1].getStart());
            support[i-1].setEnd(chain[i].getEnd());
            supforce[i] = supForce(i-1);
        }
        if(supB == false){
            ngl::Vec3 zero = {0,0,0};
            supforce[i] = zero;
            // supforce[i+1] = zero;
        }
        force[i]= hookeForce(i)/2 - hookeForce(i+1)/2 + supforce[i]-supforce[i+1]; 
        force[i].m_y=force[i].m_y-0.098;

        acceleration[i].m_y = force[i].m_y/mass;
        acceleration[i].m_x = force[i].m_x/mass;


        velocity[i].m_y = (velocity[i].m_y*0.85) + (acceleration[i].m_y/100.0);
        velocity[i].m_x = (velocity[i].m_x*0.85) + (acceleration[i].m_x/100.0);


        chain[i].setEnd(chain[i].getEnd() + velocity[i]);
        if(i != chain.size()-1){
            chain[i+1].setStart(chain[i].getEnd());
        }
        chain[0].setStart(begin);
    }   
}

void Mesh::setChainLink(int _index, bool _start, ngl::Vec3 _val){
    if(_start == true ){
        chain[_index].setStart(_val);
    }
    else{
        chain[_index].setEnd(_val);
    }
}

ngl::Vec3  Mesh::getChainLink(int _index, bool _start){
    ngl::Vec3 ret;
    if(_start == true ){
        ret = chain[_index].getStart();
    }
    else{
        ret = chain[_index].getEnd();
    }
    return ret;
}


void Mesh::setVel(float _vel, int _index, int _dir){
    if(_dir == 1){
    velocity[_index].m_y += _vel;
    }
    else if(_dir == 0){
    velocity[_index].m_x += _vel;
    }
}
void Mesh::setBeg(float _vel, int _index, int _dir){
    if(_dir == 1){
    begin.m_y += _vel;
    }
    else if(_dir == 0){
    begin.m_x += _vel;
    }
}

ngl::Vec3 Mesh::hookeForce(int _index){
    // if(chain[_index].getEnd().m_y-chain[_index].getStart().m_y>0.5){
        ngl::Vec3 first = chain[_index].getStart();
        ngl::Vec3 second = chain[_index].getEnd();
        float length = std::sqrt(((first.m_x-second.m_x)*(first.m_x-second.m_x))+ ((first.m_y-second.m_y)*(first.m_y-second.m_y)) + ((first.m_z-second.m_z)
        *(first.m_z-second.m_z)));
        float diff = length - 0.3f;
        float x = chain[_index].getEnd().m_x-chain[_index].getStart().m_x;
        float y = chain[_index].getEnd().m_y-chain[_index].getStart().m_y;
        float z = chain[_index].getEnd().m_z-chain[_index].getStart().m_z;
        ngl::Vec3 ret;
        float hooke = 0.5;
        if(diff  > 0.0f){

            if(first.m_x > second.m_x){
                ret.m_x = -x * hooke * acos(y/length);
            }
            else {
                ret.m_x = -x * hooke  * acos(y/length);
            }
            if(first.m_y > second.m_y){
                ret.m_y = y * hooke * std::asin(y/length);
            }
            else {
                ret.m_y = -y * hooke * std::asin(y/length);
            }
        return ret;
        // return -(std::abs(chain[_index].getEnd().m_y-chain[_index].getStart().m_y)-0.5)*2;
    }
    // else if(chain[_index].getEnd().m_y-chain[_index].getStart().m_y<-0.5){
    ngl::Vec3 null = {0,0,0};
    return null;
    //     return (std::abs(chain[_index].getEnd().m_y-chain[_index].getStart().m_y)-0.5)*2;
    // }
}




ngl::Vec3 Mesh::supForce(int _index){
    // if(chain[_index].getEnd().m_y-chain[_index].getStart().m_y>0.5){
        ngl::Vec3 first = support[_index].getStart();
        ngl::Vec3 second = support[_index].getEnd();
        float length = std::sqrt(((first.m_x-second.m_x)*(first.m_x-second.m_x))+ ((first.m_y-second.m_y)*(first.m_y-second.m_y)) + ((first.m_z-second.m_z)
        *(first.m_z-second.m_z)));
        float diff = length - 0.4f;
        float x = support[_index].getEnd().m_x-support[_index].getStart().m_x;
        float y = support[_index].getEnd().m_y-support[_index].getStart().m_y;
        float z = support[_index].getEnd().m_z-support[_index].getStart().m_z;
        ngl::Vec3 ret;
        float hooke = 0.2;
        if(diff > 0.0f){

            if(first.m_x > second.m_x){
                ret.m_x = -x * hooke * acos(y/length);
            }
            else {
                ret.m_x = -x * hooke  * acos(y/length);
            }
            if(first.m_y > second.m_y){
                ret.m_y = y * hooke * std::asin(y/length);
            }
            else {
                ret.m_y = -y * hooke * std::asin(y/length);
            }
        return ret;
        // return -(std::abs(chain[_index].getEnd().m_y-chain[_index].getStart().m_y)-0.5)*2;
    }
    // else if(chain[_index].getEnd().m_y-chain[_index].getStart().m_y<-0.5){
    ngl::Vec3 null = {0,0,0};
    return null;
    //     return (std::abs(chain[_index].getEnd().m_y-chain[_index].getStart().m_y)-0.5)*2;
    // }
}

void Mesh::checkLength(int _index){
    ngl::Vec3 first = chain[_index].getStart();
    ngl::Vec3 second = chain[_index].getEnd();
    float length = std::sqrt(((first.m_x-second.m_x)*(first.m_x-second.m_x))+ ((first.m_y-second.m_y)*(first.m_y-second.m_y)) + ((first.m_z-second.m_z)
        *(first.m_z-second.m_z)));
    while(length < 0.5){
        ngl::Vec3 x = {0.1,0.1, 0.1};
        ngl::Vec3 diff = chain[_index].getEnd() - chain[_index].getStart();
        // if(chain[_index].getStart().m_x > chain[_index].getEnd().m_x){
        chain[_index].setEnd(chain[_index].getEnd() + (diff*x));
        ngl::Vec3 first = chain[_index].getStart();
        ngl::Vec3 second = chain[_index].getEnd();
        length = std::sqrt(((first.m_x-second.m_x)*(first.m_x-second.m_x))+ ((first.m_y-second.m_y)*(first.m_y-second.m_y)) + ((first.m_z-second.m_z)
        *(first.m_z-second.m_z)));
    // }
    // else{
    //     chain[_index].setEnd(chain[_index].getEnd() + x);
    // }
}
}

void Mesh::setSupB(){
    supB = !supB;
}