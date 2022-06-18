#include "header.h"
#include "structure.h"
using namespace std;


extern int total_cell_size;
extern int max_size;
extern int max_gain;
extern int max_pin;
extern int max_partial_sum;
extern int iter, terminal;

CELL::CELL(){
    node_type  = 0;                            // 0:no part, 1:A , 2:B 
    lock_state = 0;                            // 0:unlock, 1:lock
    node_gain  = 0;
    prev = NULL;
    next = NULL;
}
void CELL::printList(){

    cout <<'c'<<name << '{';
    for (const auto &item : cell_array) {
        cout <<'n'<< item->name << "; ";
    }
    cout << '}'<<endl;
}
void NETS::printList(){
    cout <<'n'<< name << '{';
     for (const auto &item : net_array) {
        cout <<'c'<< item->name << "; ";
    }
    cout << '}'<<endl;
}
NETS::NETS(){
    critical    = 0;
	cell_in_A   = 0;
	cell_in_B   = 0;
}
LINKLIST::LINKLIST(){
    head = NULL;
    tail = NULL;
    count = 0;
}
void LINKLIST::printList(int i){
    cout << "gain:"<< i-max_pin << " count:" <<count << '{';
    CELL*nd = head;
    while (nd){
        cout<<'c'<<nd->name<<" ";//'('<<nd->lock_state<<')';
        nd = nd->next;
    }
    cout<<'}'<<endl;
}