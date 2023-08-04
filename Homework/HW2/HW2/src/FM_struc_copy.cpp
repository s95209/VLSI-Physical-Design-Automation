#include <iostream>
#include <string>
#include "FM_struc_copy.h"

//cell
cell::cell(int name, cell* nextc, cell* prec){
    this->name = name;
    this->nextc = nextc;
    this->prec = prec;
}
cell::cell(){
    this->name = 0;
}
/////////////////////////////////////////////////

record::record(){
    this->cell = 0;
    this->towhere = "";
    this->ATa = 0;
    this->BTa = 0;
    this->Maxgain  = 0;
}

void record::print(){
    std::cout << "cell: " << cell << " towhere: " << towhere << " ATa: " << ATa << " BTa: " << BTa << " Maxgain: " << Maxgain << std::endl;    
}

gbucket::gbucket(int Mpin){
    this -> Mpin = Mpin; 
    bucprt = new cell[Mpin];
}

gbucket::~gbucket(){
    delete[] bucprt;
}