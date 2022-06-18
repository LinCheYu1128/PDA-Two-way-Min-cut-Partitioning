#include "header.h"
#include "structure.h"
using namespace std;

int total_cell_size;
int max_cell_number;
int max_size;
int max_gain;
int max_pin;
int max_partial_sum;
int cut_size;
const bool kReadSeed = 0; // 0 for random seed; 1 for read seed from txt;
int Area_A, Area_B;

vector<CELL*> Cellist;
vector<NETS*> Netlist;
vector<LINKLIST*> Bucket_A;
vector<LINKLIST*> Bucket_B;
vector<int> Best_Part_A;
vector<int> Best_Part_B;

void GetCell(string file){
    ifstream inFile(file);
    string temp;
    Cellist.resize(max_cell_number+1);
    total_cell_size = 0;
    max_size = 0;
    while (!inFile.eof()){
        CELL* cell =  new CELL();
        if(!getline(inFile, temp, ' ')) break;
        temp.erase(temp.begin());
        cell->name = stoi(temp);
        getline(inFile, temp, '\n');
        cell->size = stoi(temp);
        max_size = (max_size < cell->size)? cell->size:max_size;
        total_cell_size += cell->size;
        Cellist.at(cell->name) = cell;
    }
    // cout<<"finish read cell file"<<endl;
}

void GetNet(string file){
    ifstream inFile(file);
    string line,temp;
    int cellname;
    Netlist.resize(1);
    while (!inFile.eof()){
        NETS* net = new NETS();
        if(!getline(inFile, temp, '{')) break;
        temp.erase(0,5);
        net->name = stoi(temp);
        getline(inFile,line,'}');
        stringstream X(line);
        
        while (getline(X, temp, '\n')) {  
            stringstream Y(temp);
            getline(Y,temp, ' ');
            while (getline(Y,temp, ' ')) { 
                if(temp.length()==0) break;
                temp.erase(temp.begin());
                cellname = stoi(temp);
                Cellist.at(cellname)->cell_array.push_back(net);
                net->net_array.push_back(Cellist.at(cellname));
            }
        } 
        // net->printList();
        Netlist.push_back(net);
        getline(inFile, temp, '\n');
    }
    // cout<<"finish read net file"<<endl;
}

void GetListSize(string file){
    ifstream inFile(file);
    string temp;
    int cell;
    max_cell_number=0;
    while (!inFile.eof()){
        if(!getline(inFile, temp, ' ')) break;
        temp.erase(temp.begin());
        cell = stoi(temp);
        max_cell_number = (cell>max_cell_number)?cell:max_cell_number;
        getline(inFile, temp, '\n');
    }
    // cout<<"max cell number is : "<<max_cell_number<<endl;
}

void InitialPart(){
    //************************************************Set A*******************************************************///
    //*********************************************** find max pin ************************************************//
    Area_A = 0; Area_B = 0;
    max_pin = 0;
    cut_size = 0;
    for(int i=1; i<=max_cell_number;i++){
        if(Cellist.at(i)!=NULL){
            int select = 1;//rand() % 2 + 1;
            if(select==1 && Area_A < ( 10*total_cell_size / 20 )) {
                Cellist.at(i)->node_type=1;
                Area_A += Cellist.at(i)->size;
            }
            else if(select==2 && Area_B < ( 10*total_cell_size / 20 )){
                Cellist.at(i)->node_type=2;
                Area_B += Cellist.at(i)->size;
            }
            else{
                if(select==1){
                    Cellist.at(i)->node_type=2;
                    Area_B += Cellist.at(i)->size;
                }
                else{
                    Cellist.at(i)->node_type=1;
                    Area_A += Cellist.at(i)->size; 
                }
            }
            max_pin  = (Cellist.at(i)->cell_array.size() > max_pin)? Cellist.at(i)->cell_array.size() : max_pin;
        }
    }
}

void InitialGain(){
    //****************************************** Net Distribution and Find Critical Net ******************************//
    for(int i=1; i< Netlist.size(); i++){
        Netlist.at(i)->cell_in_A = 0;
        Netlist.at(i)->cell_in_B = 0;
        for(auto &item : Netlist.at(i)->net_array){
            if(item->node_type == 1)Netlist.at(i)->cell_in_A += 1;
            else  Netlist.at(i)->cell_in_B += 1;
        }
        if( Netlist.at(i)->cell_in_A== 1 || Netlist.at(i)->cell_in_B== 1 ||
            Netlist.at(i)->cell_in_A== 0 || Netlist.at(i)->cell_in_B== 0 )  Netlist.at(i)->critical = 1;
        else  Netlist.at(i)->critical = 0;
        if(Netlist.at(i)->cell_in_A!=0 && Netlist.at(i)->cell_in_B!=0) cut_size+=1;
        //*********************************************** Initial Gain ***********************************************//
        if(Netlist.at(i)->critical){
            for(auto &item : Netlist.at(i)->net_array) {
                if(item->node_type==1){
                    if(Netlist.at(i)->cell_in_A==1) item->node_gain += 1;
                    if(Netlist.at(i)->cell_in_B==0) item->node_gain -= 1;
                }
                else{
                    if(Netlist.at(i)->cell_in_B==1) item->node_gain += 1;
                    if(Netlist.at(i)->cell_in_A==0) item->node_gain -= 1;   
                }
            }
        }
    }
    
}

void Remove(LINKLIST* dlist, CELL* element){
    // dlist->printList(0);
    if(element == dlist->head && element == dlist->tail) {
        // cout << "r1 remove c"<< element->name << endl;
        dlist->head = NULL;
        dlist->tail = NULL;
    }
    else if(element == dlist->head){
        // cout << "r2 remove c"<< element->name << endl;
        dlist->head = element->next;
        element->next->prev = NULL;
    }
    else if(element == dlist->tail){
        // cout << "r3 remove c"<< element->name << endl;
        dlist->tail = element->prev;
        element->prev->next = NULL;
    }
    else{
        // cout << "r4 remove c"<< element->name << endl;
        element->prev->next = element->next;
        element->next->prev = element->prev;

    }
    dlist->count --;
}

void Insert(LINKLIST* dlist, CELL* element){
    if(dlist->head == NULL){ //empty 
        dlist->head = element;
        dlist->tail = element;
        element->next = NULL;
        element->prev = NULL;
        dlist->count ++;
    }
    // else if(dlist->head == dlist->tail){
    //     dlist->head = element;
    //     element->next = dlist->tail;
    //     element->prev = NULL;
    //     dlist->tail->prev = element;
    //     dlist->count ++; 
    // }
    else{
        dlist->head->prev = element;
        element->next = dlist->head;
        element->prev = NULL;
        dlist->head = element;
        dlist->count ++; 
    }
}

void GenerateBucketList(){
    Bucket_A.clear();
    Bucket_B.clear();
    Bucket_A.resize(2*max_pin+1);
    Bucket_B.resize(2*max_pin+1);
    for(int i=0; i<2*max_pin+1; i++){
        LINKLIST* dlistA = new LINKLIST();
        LINKLIST* dlistB = new LINKLIST();
        Bucket_A.at(i) = dlistA; 
        Bucket_B.at(i) = dlistB; 
    }
    for(int i=0 ; i <= max_cell_number ; i++){
        if(Cellist.at(i)!=NULL){
            int ptr = Cellist.at(i)->node_gain+max_pin;
            if(Cellist.at(i)->node_type==1){
                Insert(Bucket_A.at(ptr), Cellist.at(i));
            }
            else{
                Insert(Bucket_B.at(ptr), Cellist.at(i));
            }
        }
    }
    // cout<<"Bucket Finish"<<endl;
}

bool CheckBlance(CELL* base_cell){ 
    if(base_cell->node_type==1){
        if(abs(Area_A - Area_B - 2*base_cell->size) < (total_cell_size/10)) return 1;
        else return 0;
    }
    else{
        if(abs(Area_A - Area_B + 2*base_cell->size) < (total_cell_size/10)) return 1;
        else return 0;
    }
}

CELL* SelectBaseCell(){
    CELL *base;
    int trytime = 1;
    int temp;
    int A_ptr=2*max_pin;
    int B_ptr=2*max_pin;
    bool getbase = false;

    while (!getbase){
        while(Bucket_A.at(A_ptr)->count==0){
            A_ptr --;
        }
        while(Bucket_B.at(B_ptr)->count==0){
            B_ptr --;
        }
        // cout << A_ptr-max_pin << ' '<< B_ptr-max_pin<<endl;
        if(A_ptr>=B_ptr){
            base = Bucket_A.at(A_ptr)->head;
            temp = Bucket_A.at(A_ptr)->count - trytime;
            getbase = CheckBlance(base);
            if(getbase) {
                Remove(Bucket_A.at(A_ptr), base);
            }
            else{
                if(temp==0) {
                    A_ptr--; 
                    trytime = 1;
                }
                else{
                    Bucket_A.at(A_ptr)->tail->next = base;
                    base->prev = Bucket_A.at(A_ptr)->tail;
                    Bucket_A.at(A_ptr)->head = base->next;
                    Bucket_A.at(A_ptr)->head->prev = NULL;
                    base->next = NULL;
                    Bucket_A.at(A_ptr)->tail = base;
                    trytime ++;
                }
            }
        }
        else {
            base = Bucket_B.at(B_ptr)->head;
            temp = Bucket_B.at(B_ptr)->count - trytime;
            getbase = CheckBlance(base);
            if(getbase) {
                Remove(Bucket_B.at(B_ptr), base);
                // Bucket_B.at(B_ptr)->printList(B_ptr);
            }
            else{ 
                if(temp==0) {
                    B_ptr--; 
                    trytime = 1;
                }
                else{
                    Bucket_B.at(B_ptr)->head = base->next;
                    base->next->prev = NULL;
                    Bucket_B.at(B_ptr)->tail->next = base;
                    base->prev = Bucket_B.at(B_ptr)->tail;
                    base->next = NULL;
                    Bucket_B.at(B_ptr)->tail = base;
                    trytime ++;
                    // Bucket_B.at(B_ptr)->printList(B_ptr);
                }
            }
        }
    }
    return base;
}

void Movement(CELL* base){
    // cout << "before move" << endl;
    int temp_cutsize = 0;
    if(base->node_type==1){                                       // base cell is in A
        Area_A -= base->size ;
        Area_B += base->size ;
        for (const auto &net : base->cell_array) {               // for each net n on the base cell
            // before move
            if(net->critical == 1){                                   // check critical net before move
                if(net->cell_in_B==0){                                // if T(n) = 0
                    for (const auto &cell : net->net_array){         // all cell on n
                        if(cell->lock_state == 0 && cell->node_type == 1){     // free cell    // increment gain
                            Remove(Bucket_A.at(cell->node_gain + max_pin), cell);
                            cell->node_gain ++;
                            Insert(Bucket_A.at(cell->node_gain + max_pin), cell);
                        }
                        else if(cell->lock_state == 0 && cell->node_type == 2){
                            Remove(Bucket_B.at(cell->node_gain + max_pin), cell);
                            cell->node_gain ++;
                            Insert(Bucket_B.at(cell->node_gain + max_pin), cell);
                        }     
                    }
                }
                else if(net->cell_in_B==1){                           // elseif T(n) = 1
                    for (const auto &cell : net->net_array){         // all cell on n
                        if(cell->lock_state == 0 && cell->node_type == 2){  // only T cell on n and if it is free // decrement gain
                            Remove(Bucket_B.at(cell->node_gain + max_pin), cell);
                            cell->node_gain --;
                            Insert(Bucket_B.at(cell->node_gain + max_pin), cell);
                        }
                    }
                }
            }
            // move
            // cout<<"move"<<endl;
            if(net->cell_in_A!=0 && net->cell_in_B!=0) temp_cutsize ++;
            net->cell_in_A -= 1;
            net->cell_in_B += 1;
            if(net->cell_in_A==0 || net->cell_in_A==1 || net->cell_in_B==0 || net->cell_in_B==1) net->critical = 1;
            else net->critical = 0 ;
            if(net->cell_in_A!=0 && net->cell_in_B!=0) temp_cutsize --;
            // after move
            if(net->critical == 1){                                  
                if(net->cell_in_A==0){                               
                    for (const auto &cell : net->net_array){        
                        if(cell->lock_state == 0 && cell->node_type==1){
                            Remove(Bucket_A.at(cell->node_gain + max_pin), cell);
                            cell->node_gain --;
                            Insert(Bucket_A.at(cell->node_gain + max_pin), cell);
                        }  
                            
                        else if(cell->lock_state == 0 && cell->node_type==2){
                            Remove(Bucket_B.at(cell->node_gain + max_pin), cell);
                            cell->node_gain --;
                            Insert(Bucket_B.at(cell->node_gain + max_pin), cell);
                        }
                    }
                }
                else if(net->cell_in_A==1){                           
                    for (const auto &cell : net->net_array){       
                        if(cell->lock_state == 0 && cell->node_type==1){
                            Remove(Bucket_A.at(cell->node_gain + max_pin), cell);
                            cell->node_gain ++;
                            Insert(Bucket_A.at(cell->node_gain + max_pin), cell);
                        }   
                            
                    }
                }
            }
        }
        base->node_type = 2;
    }
    else{
        Area_A += base->size ;
        Area_B -= base->size ;
        for (const auto &net : base->cell_array) {                    // for each net n on the base cell
            // before move
            if(net->critical == 1){                                   // check critical net before move
                if(net->cell_in_A==0){                                // if T(n) = 0
                    for (const auto &cell : net->net_array){          // all cell on n
                        if(cell->lock_state == 0 && cell->node_type==1){  // free cell    // increment gain
                            Remove(Bucket_A.at(cell->node_gain + max_pin), cell);
                            cell->node_gain ++;
                            Insert(Bucket_A.at(cell->node_gain + max_pin), cell);
                        }
                        else if(cell->lock_state == 0 && cell->node_type==2){
                            Remove(Bucket_B.at(cell->node_gain + max_pin), cell);
                            cell->node_gain ++;
                            Insert(Bucket_B.at(cell->node_gain + max_pin), cell);

                        }
                    }
                }
                else if(net->cell_in_A==1){                           // elseif T(n) = 1
                    for (const auto &cell : net->net_array){          // all cell on n
                        if(cell->lock_state == 0 && cell->node_type==1){  // only T cell on n and if it is free // decrement gain
                            Remove(Bucket_A.at(cell->node_gain + max_pin), cell);
                            cell->node_gain --;
                            Insert(Bucket_A.at(cell->node_gain + max_pin), cell);
                        }
                    }
                }
            }
            // move
            // cout<<"move"<<endl;
            if(net->cell_in_A!=0 && net->cell_in_B!=0) temp_cutsize ++;
            net->cell_in_A += 1;
            net->cell_in_B -= 1;
            if(net->cell_in_A==0 || net->cell_in_A==1 || net->cell_in_B==0 || net->cell_in_B==1) net->critical = 1;
            else net->critical = 0 ;
            if(net->cell_in_A!=0 && net->cell_in_B!=0) temp_cutsize --;
            // after move
            if(net->critical == 1){                                  
                if(net->cell_in_B==0){                               
                    for (const auto &cell : net->net_array){         
                        if(!cell->lock_state && cell->node_type==1) {
                            Remove(Bucket_A.at(cell->node_gain + max_pin), cell);
                            cell->node_gain --;
                            Insert(Bucket_A.at(cell->node_gain + max_pin), cell); 
                        }
                        else if(!cell->lock_state && cell->node_type==2){
                            Remove(Bucket_B.at(cell->node_gain + max_pin), cell);
                            cell->node_gain --;
                            Insert(Bucket_B.at(cell->node_gain + max_pin), cell);
                        }
                    }
                }
                else if(net->cell_in_B==1){                          
                    for (const auto &cell : net->net_array){         
                        if(!cell->lock_state && cell->node_type==2){
                            Remove(Bucket_B.at(cell->node_gain + max_pin), cell);
                            cell->node_gain ++;
                            Insert(Bucket_B.at(cell->node_gain + max_pin), cell);
                        }                      
                    }
                }
            }
        }
        base->node_type = 1;
    } 
    //************************************************* update cut_size *****************************************************//
    cut_size -= temp_cutsize ;
    // cout << "finish move" << endl;
}

bool CheckLockCell(){
    int A_ptr=2*max_pin;
    int B_ptr=2*max_pin;
    while(Bucket_A.at(A_ptr)->count==0){
        A_ptr --;
    }
    while(Bucket_B.at(B_ptr)->count==0){
        B_ptr --;
    }
    if(A_ptr < (max_pin) && B_ptr < (max_pin)) return false;
    else return true;
}

void UpdateBest(){
    Best_Part_A.clear();
    Best_Part_B.clear();
    for(auto &item : Cellist){
        if(item!=NULL){
            if(item->node_type==1) Best_Part_A.push_back(item->name);
            else Best_Part_B.push_back(item->name);
        }
    }
}

void ResetState(){
    cut_size = 0;
    Area_B = 0;
    Area_A = 0;
    for (auto &item : Cellist){
        if(item!=NULL){
            item->lock_state = 0;
            item->node_gain = 0;
        }    
    } 
    for( auto &item : Best_Part_A){
        Cellist.at(item)->node_type = 1;
        Cellist.at(item)->node_gain = 0; 
        Area_A += Cellist.at(item)->size;
    }
    for( auto &item : Best_Part_B){
        Cellist.at(item)->node_type = 2;
        Cellist.at(item)->node_gain = 0;
        Area_B += Cellist.at(item)->size;
    }
}

void Restart(){
    cut_size = 0;
    Area_B = 0;
    Area_A = 0;
    for (auto &item : Cellist){
        if(item!=NULL){
            item->lock_state = 0;
            item->node_gain = 0;
        }    
    } 
}

void WriteOutputFile(string file){
    ofstream OutFile(file);
    OutFile << "cut_size "<< cut_size << endl;
    OutFile << "A " << Best_Part_A.size() << endl;
    for(const auto &item : Best_Part_A){
        OutFile <<'c';
        OutFile << item << endl;
    }
    OutFile << "B " << Best_Part_B.size() << endl;
        for(const auto &item : Best_Part_B){
        OutFile <<'c';
        OutFile << item << endl;
    }
}

void PrintBucketList(){
    cout << 'A' <<endl;
    for(int i=0; i<2*max_pin+1; i++){
        Bucket_A.at(i)->printList(i);
    }
    cout << 'B' <<endl;
    for(int i=0; i<2*max_pin+1; i++){
        Bucket_B.at(i)->printList(i);
    }
}

int main(int argc, char *argv[]){
    //**********Start time **********//
    clock_t timeStart ;
	clock_t timeEnd ;
	timeStart = clock();

    //**********Read file************//
    // string Netfile("../");
    // string Cellfile("../");
    // string Outputfile("../");
    // Netfile.append(argv[1]);
    // Cellfile.append(argv[2]);
    // Outputfile.append(argv[3]);
    string Netfile = argv[1];
    string Cellfile = argv[2];
    string Outputfile = argv[3];
    // cout << Cellfile << endl << Netfile << endl << Outputfile << endl;

    GetListSize(Cellfile);

    GetCell(Cellfile);  

    GetNet(Netfile);
    timeEnd = clock();
    cout <<"IO time = " << double(timeEnd - timeStart)/CLOCKS_PER_SEC << endl;
    // cout << "cell number = " << Cellist.size() << endl;
    // cout << "total area =" << total_cell_size << endl;
    // cout << "net number = " << Netlist.size() << endl;
    // cout << "max pin = "<< max_pin <<endl;
    // cout << "area |A-B| must small than " << total_cell_size/10 << endl ;
    //****************************************  Random Seed  *******************************************//
    srand((unsigned)time(0));
    // srand(0);
    //**************************************  Initial Partioning  **************************************//
    InitialPart();
    int terminal = 0;
    int min_cut = 9999999;
    // int gaintemp = 10;
    for (int i=0;i<4;i++){
        // InitialPart();
        //*************************************** Initail Gain ********************************************//
        InitialGain();
        //*************************************** Generate Bucket List ************************************//
        GenerateBucketList();
        while(CheckLockCell()){
            CELL* basecell = SelectBaseCell();
            if(basecell->node_gain<=0){
                if(min_cut>cut_size){
                    UpdateBest();
                    min_cut = cut_size;
                }
            }
            basecell->lock_state = 1;
            Movement(basecell);
              
            if(double(clock() - timeStart)/CLOCKS_PER_SEC>295) break;
            // if(terminal>35) break; 
            // cout<<cut_size<<endl; 
        }
        if(double(clock() - timeStart)/CLOCKS_PER_SEC>295) break;
        // terminal = 0;
        // if(min_cut>cut_size){
        //     UpdateBest();
        //     min_cut = cut_size;
        // }        
        // cout<<"cutsize = "<<min_cut<<endl;
        
        ResetState();
        // Restart();
    } 
    cout<<"min_cut = "<<min_cut<<endl;
    cut_size = min_cut;
    cout << "computation time= " << double(clock()-timeEnd)/CLOCKS_PER_SEC << endl;
    // UpdateBest();
    WriteOutputFile(Outputfile);
    //*******************************************   End Time   ***************************************//
    timeEnd = clock();
    cout << "time= " << double(timeEnd - timeStart)/CLOCKS_PER_SEC << endl;
    return 0;
}

