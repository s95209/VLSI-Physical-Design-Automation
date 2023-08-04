#ifndef FM_COPY_H
#define FM_COPY_H
#include <string>
#include <vector>
class cell{
    public:
        int name;
        cell* nextc;
        cell* prec;
        cell(int, cell* nextc = NULL, cell* prec = NULL);
        cell();
};

class gbucket{
    private:
        int Mpin;
        cell *bucprt;
    public:
        gbucket(int);
        ~gbucket();
};

class record{
    public:
        int cell;
        std::string towhere;
        int ATa;
        int BTa;
        int Maxgain;
        record();
        void print();
};
#endif