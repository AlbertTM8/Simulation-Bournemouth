#include "Mesh.h"
#include <ngl/ShaderLib.h>
#include <iostream>

Mesh::Mesh(int _numlines)
{   
    ngl::Vec3 start = {0.0f,0.0f,0.0f};
    ngl::Vec3 end = {0.0f,-0.5,0.0f};
    for(float i = 0; i<_numlines; i+=1.0f){
        chain.push_back(Line(start, end));
        // std::cout << "Start = " << start.m_y;
        // std::cout << "End = " << end.m_y;
        velocity.push_back({0,0,0});
        acceleration.push_back({0,0,0});
        force.push_back({0,0,0});
        start.m_y = -i/2.0f;
        end.m_y = (-i/2.0f)-0.5;
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

void Mesh::updateChain(){

    
    for(int i = 0; i<chain.size(); i++){

        // chain[0].setStart({0.0,0.0,0.0});
        // // if(std::sqrt(((first.m_x-second.m_x)*(first.m_x-second.m_x))+ ((first.m_y-second.m_y)*(first.m_y-second.m_y)) + ((first.m_z-second.m_z)
        // //     *(first.m_z-second.m_z))) < 0.5){
        // if(i != chain.size()-1){
        //     force[i].m_y = hookeForce(i)/2 - .01 + -hookeForce(i+1)/2;
        // }
        // else{
            
        force[i]= hookeForce(i)/2 - hookeForce(i+1)/2; 
        force[i].m_y=force[i].m_y-0.1;
        // }
       
        acceleration[i].m_y = force[i].m_y/mass;
        acceleration[i].m_x = force[i].m_x/mass;
        velocity[i].m_y = (velocity[i].m_y*0.98) + (acceleration[i].m_y/100.0);
        velocity[i].m_x = (velocity[i].m_x*0.98) + (acceleration[i].m_x/100.0);
        chain[i].setEnd(chain[i].getEnd() + velocity[i]);
        if(i != chain.size()-1){
            chain[i+1].setStart(chain[i].getEnd());
        }
        ngl::Vec3 begin = {0.0,0.0,0.0};
        chain[0].setStart(begin);
}   
    // std::cout << "velocity = " << velocity.m_y << "\n";
    // std::cout << "acce = " << acceleration.m_y << "\n";;
    // chain[0].setEnd(chain[0].getEnd() + velocity);
    // std::cout << "position = " << chain[0].getEnd().m_y << "\n";;
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

ngl::Vec3 Mesh::hookeForce(int _index){
    // if(chain[_index].getEnd().m_y-chain[_index].getStart().m_y>0.5){
        ngl::Vec3 first = chain[_index].getStart();
        ngl::Vec3 second = chain[_index].getEnd();
        float length = std::sqrt(((first.m_x-second.m_x)*(first.m_x-second.m_x))+ ((first.m_y-second.m_y)*(first.m_y-second.m_y)) + ((first.m_z-second.m_z)
        *(first.m_z-second.m_z)));
        float diff = length - 0.5f;
        float x = chain[_index].getEnd().m_x-chain[_index].getStart().m_x;
        float y = chain[_index].getEnd().m_y-chain[_index].getStart().m_y;
        float z = chain[_index].getEnd().m_z-chain[_index].getStart().m_z;
        ngl::Vec3 ret;
        float hooke = 0.2;
        if(diff - 0.5f > 0.0f){

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
            if(first.m_z > second.m_z){
                ret.m_z = z * hooke * asin(z/length);
            }
            else {
                ret.m_z = -z * hooke * asin(z/length);
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