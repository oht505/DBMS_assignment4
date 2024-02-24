asdasd/* This is a skeleton code for External Memory Sorting, you can make modifications as long as you meet 
   all question requirements*/  

#include <string>
#include <ios>
#include <fstream>
#include <vector>
#include <string>
#include <string.h>
#include <iostream>
#include <sstream>
#include <stdexcept>
#include <cmath> 
#include <algorithm>
#include "record_class.h"

using namespace std;

//defines how many blocks are available in the Main Memory 
#define buffer_size 22
#define BLOCK_SIZE 1536 

Records buffers[buffer_size]; //use this class object of size 22 as your main memory

void FillBufferFromFile(Records buffers[buffer_size], fstream &dataFile){
    
    // Grab 22 employ records from the file
    for(int i=0; i<22; i++){
        // Get one record 
        Records empRecord = Grab_Emp_Record(dataFile);

        // No more records in the file
        if(empRecord.no_values == -1){
            break;
        }

        // Insert each field data into emp_record 
        buffers[i].emp_record.age = empRecord.emp_record.age;
        buffers[i].emp_record.eid = empRecord.emp_record.eid;
        buffers[i].emp_record.ename = empRecord.emp_record.ename;
        buffers[i].emp_record.salary = empRecord.emp_record.salary;

        // For checking the total number of records
        buffers->number_of_emp_records += 1;
    }

}

void PrintBufferEmployeeInfo(){
    for (int j=0; j<buffer_size; j++){
        cout<<"eid:"<<buffers[j].emp_record.eid<<", ";
        cout<<"ename:"<<buffers[j].emp_record.ename<<", ";
        cout<<"age:"<<buffers[j].emp_record.age<<", ";
        cout<<"salary:"<<buffers[j].emp_record.salary<<endl;
    }
}

static bool compareByEmployeeId(const Records& a, const Records& b){
    return a.emp_record.eid < b.emp_record.eid;
}
    
void sortRecordsByEmployeeId(){
        sort(buffers, buffers+buffer_size, compareByEmployeeId);
}

string serialize(Records empInfo)
{
    ostringstream serializedRecord;
    serializedRecord.write(reinterpret_cast<const char *>(&empInfo.emp_record.eid), sizeof(int))<< ',';                 
    serializedRecord << empInfo.emp_record.ename <<',';                                                                
    serializedRecord.write(reinterpret_cast<const char*>(&empInfo.emp_record.age), sizeof(int)) << ',';                                                                 // serialize string bio, variable length
    serializedRecord.write(reinterpret_cast<const char *>(&empInfo.emp_record.salary), sizeof(double)); 
    return serializedRecord.str();
}

void writeRecordToFile(Records buffers[], int idx, fstream &runFile){
    string serializedRecord = serialize(buffers[idx]);
    int nextFreeSpace;
    int RecordNumInPage;
    int recordLength = serializedRecord.size();

    
    

        
   
}



/***You can change return type and arguments as you want.***/
//PASS 1
//Sorting the buffers in main_memory and storing the sorted records into a temporary file (Runs) 
void Sort_Buffer(Records buffers[buffer_size], fstream &runFile){
    //Remember: You can use only [AT MOST] 22 blocks for sorting the records / tuples and create the runs

    // Sort records in the buffer
    sortRecordsByEmployeeId();

    // Insert records into the Run page
    

    // Write the Run page to the Run File
    

    return;
}

//PASS 2
//Use main memory to Merge the Runs 
//which are already sorted in 'runs' of the relation Emp.csv 
void Merge_Runs(){
    //and store the Sorted results of your Buffer using PrintSorted() 

    return;
}

void PrintSorted(){

    //Store in EmpSorted.csv
    return;
}

int main() {

    //Open file streams to read and write
    //Opening out the Emp.csv relation that we want to Sort
    fstream empin;
    empin.open("Emp2.csv", ios::in);
    if(!empin.is_open()){
        cerr << "Error opening file: "<<endl;
        return 1;
    }
   
    //Creating the EmpSorted.csv file where we will store our sorted results
    fstream SortOut;
    SortOut.open("EmpSorted.csv", ios::out | ios::app);
    if(!SortOut.is_open()){
        cerr << "Error opening the file to write"<<endl;
        return 1;
    }

    //1. Create runs for Emp which are sorted using Sort_Buffer()
    fstream Runs;
    Runs.open("run.csv", ios::out | ios::binary);
    if(!Runs.is_open()){
        cerr << "Error opening the file to write" << endl;
        return 1;
    }    
    
    //1-1. Fill buffer with 22 employee records
    FillBufferFromFile(buffers, empin);
    
    //1-2. Sort records in the buffer and write to the Run File
    Sort_Buffer(buffers, Runs);
    //PrintBufferEmployeeInfo();

    writeRecordToFile(buffers,0,Runs);

    //2. Use Merge_Runs() to Sort the runs of Emp relations 


    //Please delete the temporary files (runs) after you've sorted the Emp.csv



    empin.close();
    SortOut.close();
    Runs.close();

    return 0;
}
