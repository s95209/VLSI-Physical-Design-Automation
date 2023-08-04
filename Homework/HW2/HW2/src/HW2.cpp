#include <iostream>
#include <string>
#include <sstream>
#include "FM_struc_copy.h"
#include <fstream>
#include <algorithm>
#include <vector>
#include <cmath>
#include <time.h>
#include <stdio.h>
#include <cstring>
using namespace std;
struct node{
  int name;
  node* next = NULL;  
  node(int i){
    name = i;
  }
};

cell *cptrvec[1000001]; //紀錄每個cell在bucket list的位置
int netinAB[1000001][2] = {0}; //紀錄每個net所連接的cell [0] :　A, [1] : B
int savenetinAB[1000001][2];
int summary[4];
int cell_info[1000001][6] = {0}; //[0] : in A=0 B=1 ,[1]: A_area ,[2]: B_area ,[3]: gain ,[4]: lock ,[5]:cell_degree
int savecell_info[1000001][6];
node* cell_net_arr[1000001];
node* net_cell_arr[1000001];
int A_part[1000001];
int B_part[1000001];
int net_info[1000001][2] = {0};
int Maxpi = 0;
int save_A_B[1000001] = {0} ;
cell **bucket_B;
cell **bucket_A;

bool balance(int ATa, int BTa){
    return((fabs(float(ATa-BTa))) < (fabs((float(ATa+BTa))/10)));
}
bool compare_B_area(int f, int s){
    return cell_info[f][1] > cell_info[s][1];
}
int Total_A_Area(vector<int> &v){
    int area = 0;
    for(vector<int>::iterator vi = v.begin(); vi < v.end(); vi++){
        area += cell_info[*vi][1];
    }
    return area;
}
int Total_B_Area(vector<int> &v){
    int area = 0;
    for(vector<int>::iterator vi = v.begin(); vi < v.end(); vi++){
        area += cell_info[*vi][2];
    }
    return area;
}
void insert_bucket(cell* &b, cell* &c){
    c->nextc = b->nextc;
    if(b->nextc != NULL) b->nextc->prec = c;
    c->prec = b;
    b->nextc = c;
}
void remove_bucket(cell* &c){
    c->prec->nextc = c->nextc;
    if(c->nextc != NULL) c->nextc->prec = c->prec;
    c->nextc = NULL;
    c->prec = NULL;
}
void up_gain(cell* &c){
    node* next_n = cell_net_arr[c->name]->next;
    int BT;
    int BF;
    int AT;
    int AF;
    int T_pos;
    int F_pos;
    int gain_all_change;
    while(next_n != NULL){
        if(cell_info[c->name][0] == 0){
            T_pos = 0;
            BT = netinAB[next_n->name][0];
            BF = netinAB[next_n->name][1];
            netinAB[next_n->name][0]++;
            netinAB[next_n->name][1]--;
            AT = netinAB[next_n->name][0];
            AF = netinAB[next_n->name][1];
        }
        else{
            T_pos = 1;
            BT = netinAB[next_n->name][1];
            BF = netinAB[next_n->name][0];
            netinAB[next_n->name][0]--;
            netinAB[next_n->name][1]++;
            AT = netinAB[next_n->name][1];
            AF = netinAB[next_n->name][0];
        }
        gain_all_change = 0;
        node* next_c = net_cell_arr[next_n->name]->next;
        if(BT == 0) gain_all_change++;
        if(AF == 0) gain_all_change--;
        while(next_c != NULL){
            if(cell_info[next_c->name][4] == 0){
                cell_info[next_c->name][3] += gain_all_change;
                remove_bucket(cptrvec[next_c->name]);
                if(cell_info[next_c->name][0] == 0){
                    insert_bucket(bucket_A[cell_info[next_c->name][3]+Maxpi], cptrvec[next_c->name]);
                }
                else{
                    insert_bucket(bucket_B[cell_info[next_c->name][3]+Maxpi], cptrvec[next_c->name]);
                }
                next_c = next_c->next;
            }
            else{
                next_c = next_c->next;
            }
        }
        next_c = net_cell_arr[next_n->name]->next;
        while (next_c != NULL)
        {
            if(cell_info[next_c->name][4] == 0){
                if(T_pos == 0){
                    if(BT == 1 && cell_info[next_c->name][0] == 0){
                        cell_info[next_c->name][3]--;
                        remove_bucket(cptrvec[next_c->name]);
                        insert_bucket(bucket_A[cell_info[next_c->name][3]+Maxpi], cptrvec[next_c->name]);
                    }
                    if(AF == 1 && cell_info[next_c->name][0] == 1){
                        cell_info[next_c->name][3]++;
                        remove_bucket(cptrvec[next_c->name]);
                        insert_bucket(bucket_B[cell_info[next_c->name][3]+Maxpi], cptrvec[next_c->name]);
                    }
                }
                else{
                    if(BT == 1 && cell_info[next_c->name][0] == 1){
                        cell_info[next_c->name][3]--;
                        remove_bucket(cptrvec[next_c->name]);
                        insert_bucket(bucket_B[cell_info[next_c->name][3]+Maxpi], cptrvec[next_c->name]);
                    }
                    if(AF == 1 && cell_info[next_c->name][0] == 0){
                        cell_info[next_c->name][3]++;
                        remove_bucket(cptrvec[next_c->name]);
                        insert_bucket(bucket_A[cell_info[next_c->name][3]+Maxpi], cptrvec[next_c->name]);
                    }
                }
                next_c = next_c->next;
            }
            else{
                next_c = next_c->next;
            }
        }
        next_n = next_n->next;
    }
}

int main(int argc, char **argv)
{
///////////////////////////建立cell double linklist及cell pointer vec//////////////
    char cname[8];
    char A_area[8];
    char B_area[8];
    int i_cname;
    string sA_area;
    string sB_area;
    int iscname;
    int total_cell = 0;
    int i_A_area;
    int i_B_area;
    int ATa = 0;
    int BTa = 0;
    FILE * File1 = fopen(argv[1], "r");
    for(int i = 0; i <= 1000000; i++){
        net_cell_arr[i] = new node(0);
        cell_net_arr[i] = new node(0);
    }
    while(fscanf(File1,"%s %s %s", cname, A_area, B_area) != EOF){
        fflush(stdin);
        total_cell++;
        i_cname = stoi(string(cname).substr(1));
        i_A_area = stoi(A_area);
        i_B_area = stoi(B_area);
        BTa += i_B_area;
        cell_info[i_cname][1] = i_A_area;
        cell_info[i_cname][2] = i_B_area;
        cell_info[i_cname][0] = 1;
        cell *newcell = new cell(i_cname);
        cptrvec[i_cname] = newcell;
    }

/////////////////////////////////////////////////////////////////////////////////
/////////////////////construct net to cell vector////////////////////////////////

    int i_nccname;
    int n_cell_cont;
    int total_net = 0;
    string nccellstr;
    char *p;
    FILE * File2 = fopen(argv[2], "r");
    char nccname[8];
    char ccname[8];
    char tempstr[200];
    while (fscanf(File2, "%*s %*c%s %*c %[^\n]", nccname, tempstr) != EOF)
    {   
        total_net++;
        n_cell_cont = 0;
        nccellstr = string(tempstr);
        nccellstr.pop_back();
        i_nccname = stoi(nccname);
        string item;
        int i_tem_cell;
        node *p = NULL;
        stringstream split(nccellstr);
        while(getline(split, item, ' ')){
            n_cell_cont++;
            item.erase(0,1);
            i_tem_cell = stoi(item);
            p = new node(i_tem_cell);
            p->next = net_cell_arr[i_nccname]->next;
            net_cell_arr[i_nccname]->next = p;
            p = new node(i_nccname);
            p->next = cell_net_arr[i_tem_cell]->next;
            cell_net_arr[i_tem_cell]->next = p;
            cell_info[i_tem_cell][5]++;
        }
        netinAB[i_nccname][1] = n_cell_cont;
    }

/////////////////////////////////////////////////////////////////////////////////
/////////////////////construct bucket link list//////////////////////////////////
    for(int i=1; i <= total_cell; i++){
        Maxpi = max(cell_info[i][5], Maxpi);
    }
    bucket_B = new cell*[Maxpi*2+1];
    bucket_A = new cell*[Maxpi*2+1];

    for(int i = 0; i < Maxpi*2+1; i++){
        bucket_B[i] = new cell(0);
        bucket_A[i] = new cell(0);
    }

/////////////////////////////////////////////////////////////////////////////////
////////////////////////cell put in B area///////////////////////////////////////
    for(int i = 1; i <= total_cell; i++)
        B_part[i] = i;
    sort(B_part+1, B_part+ 1 + total_cell, compare_B_area);  
/////////////////////////////////////////////////////////////////////////////////
/////////////////////////Initialize partition////////////////////////////////////
    int temp_B_a;
    int temp_A_a;
    int b_index = total_cell;
    int a_index = 1;
    while(!balance(ATa,BTa)){
        int num = B_part[b_index];
        node* n = cell_net_arr[num]->next;
        while(n != NULL)
        {
            netinAB[n->name][0]++;
            netinAB[n->name][1]--;
            n = n->next;
        }
        A_part[a_index] = B_part[b_index];
        a_index++;
        b_index--;
        cell_info[num][0] = 0;
        ATa += cell_info[num][1];
        BTa -= cell_info[num][2];
    }
/////////////////////////////////////////////////////////////////////////////////   
/////////////////////////Initialize gain/////////////////////////////////////////
    for(int i = 1; i <= total_cell; i++){
        node* n = cell_net_arr[i]->next;
        while(n != NULL){
            int A_cell_count = netinAB[n->name][0];
            int B_cell_count = netinAB[(n->name)][1];
            if(cell_info[i][0] == 0){
                if(A_cell_count == 1){
                    cell_info[i][3]++;
                }
                if(B_cell_count == 0){
                    cell_info[i][3]--;
                }    
            }
            else{
                if(B_cell_count == 1){
                    cell_info[i][3]++;
                }
                if(A_cell_count == 0){
                    cell_info[i][3]--;
                }
            } 
            n = n->next; 
        }
        int index = cell_info[i][3] + Maxpi;

        if(cell_info[i][0] == 0){
            insert_bucket(bucket_A[index], cptrvec[i]);
        }
        else{
            insert_bucket(bucket_B[index], cptrvec[i]);
        } 
    }
/////////////////////////////////////////////////////////////////////////////////
////////////////////////Update_Gains/////////////////////////////////////////////
    int Maxgain;
    int parsum = 0;
    int Maxsum = 0;
    int next_pass = 1;
    int finded;
    cell* nt = NULL;
    memcpy(savenetinAB, netinAB, sizeof(netinAB));
    memcpy(savecell_info, cell_info, sizeof(cell_info));
    while(next_pass)
    {
        next_pass = 0;
        while(1){
            finded = 0;
            for(int i = Maxpi*2; i >= 0; i--){
                if( ((double)clock() / CLOCKS_PER_SEC) > 290 ){
                  Maxsum = 0;
                  next_pass = 0;
                  break;
                }
                nt = bucket_A[i]->nextc;
                while (nt != NULL)
                {
                    if( balance(ATa - cell_info[nt->name][1], BTa + cell_info[nt->name][2]) ){ 
                        finded = 1;
                        Maxgain = cell_info[nt->name][3];
                        cell_info[nt->name][4] = 1;
                        cell_info[nt->name][0] = 1;
                        ATa = ATa - cell_info[nt->name][1];
                        BTa = BTa + cell_info[nt->name][2];
                        remove_bucket(nt);
                        break;
                    }
                    else{
                        nt = nt->nextc;
                    }
                }
                if(finded == 1) break;
                nt = bucket_B[i]->nextc;
                while (nt != NULL)
                {
                    if( balance(ATa + cell_info[nt->name][1], BTa - cell_info[nt->name][2]) ){ 
                        finded = 1;
                        Maxgain = cell_info[nt->name][3];
                        cell_info[nt->name][4] = 1;
                        cell_info[nt->name][0] = 0;
                        ATa = ATa + cell_info[nt->name][1];
                        BTa = BTa - cell_info[nt->name][2];
                        remove_bucket(nt);
                        break;
                    }
                    else{
                        nt = nt->nextc;
                    }
                }
                if(finded == 1) break;
            }
            if(finded == 1){
                up_gain(nt);
                parsum += Maxgain;
                if(parsum > Maxsum){
                    Maxsum = parsum;
                    summary[0] = nt->name;
                    summary[1] = ATa;
                    summary[2] = BTa;
                    summary[3] = Maxgain;
                    memcpy(savenetinAB, netinAB, sizeof(netinAB));
                    memcpy(savecell_info, cell_info, sizeof(cell_info));
                    for(int i = 1; i <= total_cell; i++){
                        if(cell_info[i] == 0)
                            save_A_B[i] = 0;
                        else
                            save_A_B[i] = 1;
                    }
                }   
            }    
            else{
                break;
            }
        }
        
        if(Maxsum > 0){
            next_pass = 1;
            Maxsum = 0;
            ATa = summary[1];
            BTa = summary[2];
            memcpy(netinAB, savenetinAB, sizeof(savenetinAB));
            memcpy(cell_info, savecell_info, sizeof(savecell_info));
            for(int i = 1; i <= total_cell; i++){
                cell_info[i][3] = 0;
                cell_info[i][4] = 0;
            }
                
            /////////////////////////reInitialize gain/////////////////////////////////////////
            for(int i = 1; i <= total_cell; i++){
                cptrvec[i]->nextc = NULL;
                cptrvec[i]->prec = NULL;
            }
            for(int i = 0; i < Maxpi*2+1; i++){
                bucket_A[i]->nextc = NULL;
                bucket_B[i]->nextc = NULL;
            }
            for(int i = 1; i <= total_cell; i++){
                node* n = cell_net_arr[i]->next;
                while(n != NULL){
                    int A_cell_count = netinAB[n->name][0];
                    int B_cell_count = netinAB[n->name][1];
                    if(cell_info[i][0] == 0){
                        if(A_cell_count == 1){
                            cell_info[i][3]++;
                        }
                        if(B_cell_count == 0){
                            cell_info[i][3]--;
                        }    
                    }
                    else{
                        if(B_cell_count == 1){
                            cell_info[i][3]++;
                        }
                        if(A_cell_count == 0){
                            cell_info[i][3]--;
                        }
                    } 
                    n = n->next; 
                }
                
                int index = cell_info[i][3] + Maxpi;
                if(cell_info[i][0] == 0){
                    insert_bucket(bucket_A[index], cptrvec[i]);
                }
                else{
                    insert_bucket(bucket_B[index], cptrvec[i]);
                } 
                
            }
        }
        
        int cut_size = 0;
        for(int i = 1; i <= total_net; i++){
            if(savenetinAB[i][0] != 0 && savenetinAB[i][1] != 0){
                cut_size++;
            }
        }   
    }
    int cut_size = 0;
    for(int i = 1; i <= total_net; i++){
        if(savenetinAB[i][0] != 0 && savenetinAB[i][1] != 0){
            cut_size++;
        }
    }
    int inAcount = 0;
    int inBcount = 0;
    ATa = 0;
    BTa = 0;
    for(int i = 1; i <= total_cell; i++){
        if(savecell_info[i][0] == 0){
            ATa += savecell_info[i][1];
            inAcount++;
        }
        else{
            BTa += savecell_info[i][2];
            inBcount++;
        }
    }
    FILE *f = fopen(argv[3] ,"w");
    fprintf(f,"%s %d%s","cut_size", cut_size, "\n");
    fprintf(f,"%s %d%s","A", inAcount, "\n");
    for(int i = 1; i <= total_cell; i++){
        if(savecell_info[i][0] == 0){
            fprintf(f,"%s%d%s", "c", i, "\n");
        }
    }
    fprintf(f,"%s %d%s","B", inBcount, "\n");
    for(int i = 1; i <= total_cell; i++){
        if(savecell_info[i][0] == 1){
            fprintf(f,"%s%d%s", "c", i, "\n");
        }
    }
    fclose(f);
    return 0;
}