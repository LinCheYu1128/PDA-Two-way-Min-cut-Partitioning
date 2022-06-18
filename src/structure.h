#ifndef _STRUCTURE_H_
#define _STRUCTURE_H_
//*********************Delaration**************************//
class NETS;
class CELL;
//************************CELL*****************************//
class CELL{
private:
    /* data */
public:
    int name;
    int node_type;// 0:no part, 1:A , 2:B 
    int node_gain;
    int old_gain;
    int size;
    CELL* next;
    CELL* prev;
    bool lock_state;// 0:unlock, 1:lock
    list<NETS*> cell_array;
    CELL();
    // ~CELL();
    void printList();
};

//***************************NETS*************************//
class NETS{
private:
    /* data */
public:
    int name;
	bool critical;
	int  cell_in_A;
	int  cell_in_B;
    list <CELL*> net_array;
    NETS();
    // ~NETS();
    void printList();
};

class LINKLIST{
public:
    CELL* head;
    CELL* tail;
    int count;
    LINKLIST();
    void printList(int);
};
//*******************PARTITION_SET******************//
// class PARTITION_SET
// {
// private:
//     /* data */
// public:
//     list<CELL*> Best_Part_A;
//     list<CELL*> Best_Part_B;
//     int min_cutsize;
//     // make a table to store evry time behavior
//     int Area_A;
//     int Area_B;
//     int cut_size;
//     int A_ptr;
//     int B_ptr;
//     vector< unordered_map<int, CELL*> > Bucket_A;
//     vector< unordered_map<int, CELL*> > Bucket_B;
//     PARTITION_SET(/* args */);
//     // ~PARTITION_SET();
//     /***********algorithm**************/
//     void InitialPart( unordered_map<int, CELL*> Cellist, unordered_map<int, NETS*> Netlist);
//     void InitialGain( unordered_map<int, CELL*> Cellist, unordered_map<int, NETS*> Netlist);
//     void InitialBucket( unordered_map<int, CELL*> Cellist, unordered_map<int, NETS*> Netlist);
//     void Movement( unordered_map<int, CELL*> Cellist, unordered_map<int, NETS*> Netlist);
//     CELL* SelectBaseCell( int, int);
//     bool CheckBlance( CELL*);
//     bool CheckLockCell();
//     void PrintBucketList();
//     void WriteOutputFile(string);
//     void UpdateBest(unordered_map<int, CELL*> Cellist);
//     void ResetState(unordered_map<int, CELL*> Cellist, unordered_map<int, NETS*> Netlist);
// };

#endif



