/* This is a skeleton code for Optimized Merge Sort, you can make modifications as long as you meet 
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

/***You can change return type and arguments as you want.***/

void fillBufferFromFile(Records buffers[buffer_size], fstream &dataFile,int n, bool isPassOne){

    // Grab employ records from the file
    for(int i=0; i<n; i++){
        Records empRecord;
        // Get one record
        if(isPassOne) {
            empRecord = Grab_Emp_Record(dataFile);
            // For checking the total number of records
            buffers->number_of_emp_records++;

            // No more records in the file
            if(empRecord.no_values == -1){
                break;
            }
        }

        // Insert each field data into emp_record
        buffers[i].emp_record.age = empRecord.emp_record.age;
        buffers[i].emp_record.eid = empRecord.emp_record.eid;
        buffers[i].emp_record.ename = empRecord.emp_record.ename;
        buffers[i].emp_record.salary = empRecord.emp_record.salary;

    }
}

static bool compareByEmployeeId(const Records& a, const Records& b){
    return a.emp_record.eid < b.emp_record.eid;
}

void sortRecordsByEmployeeId(){
    sort(buffers, buffers+buffer_size, compareByEmployeeId);
}

static bool compareByManagerId(const Records& c, const Records& d){
    return c.dept_record.managerid < d.dept_record.managerid;
}

void sortRecordsByManagerId(){
    sort(buffers, buffers + buffer_size, compareByManagerId);
}

string serialize(Records empInfo)
{
    ostringstream serializedRecord;
    serializedRecord.write(reinterpret_cast<const char *>(&empInfo.emp_record.eid), sizeof(int));
    serializedRecord << empInfo.emp_record.ename <<",";
    serializedRecord.write(reinterpret_cast<const char*>(&empInfo.emp_record.age), sizeof(int));                                                                 // serialize string bio, variable length
    serializedRecord.write(reinterpret_cast<const char *>(&empInfo.emp_record.salary), sizeof(double));
    return serializedRecord.str();
}

void writeRecordToFile(Records buffers[], int bufferIdx, int startOffset, fstream &runFile){
    if (buffers[bufferIdx].emp_record.eid != INT32_MAX) {

        string serializedRecord = serialize(buffers[bufferIdx]);
        int nextFreeSpace;
        int RecordNumInPage;
        int minPointer;
        int recordLength = serializedRecord.size();

        // Get minPointer, RecordNumInpage, nextFreeSpace pointer
        runFile.seekg(startOffset + BLOCK_SIZE - sizeof(int) * 3);
        runFile.read(reinterpret_cast<char *>(&minPointer), sizeof(int));
        runFile.read(reinterpret_cast<char *>(&RecordNumInPage), sizeof(int));
        runFile.read(reinterpret_cast<char *>(&nextFreeSpace), sizeof(int));

        // Write the serialized record to the file
        runFile.seekp(startOffset + nextFreeSpace);
        runFile.write(serializedRecord.c_str(), serializedRecord.size());


        // Add slot (offset, recold)
        runFile.seekp(startOffset + BLOCK_SIZE - (sizeof(int) * 3 + sizeof(int) * 2 * (RecordNumInPage + 1)));
        runFile.write(reinterpret_cast<char *>(&nextFreeSpace), sizeof(int));
        runFile.write(reinterpret_cast<char *>(&recordLength), sizeof(int));

        // Update the next free space pointer
        nextFreeSpace += recordLength;
        RecordNumInPage++;

        // Write the updated next free space poointer back to the file
        runFile.seekp(startOffset + BLOCK_SIZE - sizeof(int) * 2);
        runFile.write(reinterpret_cast<const char *>(&RecordNumInPage), sizeof(int));
        runFile.write(reinterpret_cast<const char *>(&nextFreeSpace), sizeof(int));
    }
}

void createEmptyRuns(int startOffset, fstream &dataFile){
        // Seek to the appropriate position at the end of the current page
        dataFile.seekp(startOffset + BLOCK_SIZE - 3*sizeof(int));
        int initFreeSpace = 0;
        int initRecordNumInPage = 0;
        int minPointer = 1;
        dataFile.write(reinterpret_cast<const char*>(&minPointer), sizeof(int));
        dataFile.write(reinterpret_cast<const char*>(&initRecordNumInPage), sizeof(int));
        dataFile.write(reinterpret_cast<const char*>(&initFreeSpace), sizeof(int));
}

//Sorting the buffers in main_memory and storing the sorted records into a temporary file (runs) 
void Sort_Buffer(Records buffers[], int startOffset, fstream &runFile){
    //Remember: You can use only [AT MOST] 22 blocks for sorting the records / tuples and create the runs
    
    sortRecordsByEmployeeId();

}

//Prints out the attributes from empRecord and deptRecord when a join condition is met 
//and puts it in file Join.csv
void PrintJoin() {
    
    return;
}

//Use main memory to Merge and Join tuples 
//which are already sorted in 'runs' of the relations Dept and Emp 
void Merge_Join_Runs(){
   

    //and store the Joined new tuples using PrintJoin() 
    return;
}

int main() {

    //Open file streams to read and write
    //Opening out two relations Emp.csv and Dept.csv which we want to join
    fstream empin;
    empin.open("Emp.csv", ios::in);
    if(!empin.is_open()){
        cerr<<"Can't open file "<<"Emp.csv"<<endl;
    }

    fstream emp_runs;
    emp_runs.open("Emp_runs.csv", ios::in|ios::out|ios::binary);
    if(!emp_runs.is_open()){
        cerr<<"Can't open file "<<"Emp_runs.csv"<<endl;
    }

    fstream deptin;
    deptin.open("Dept.csv", ios::in);
    if(!deptin.is_open()){
        cerr<<"Can't open file "<<"Dept.csv"<<endl;
    }
   
    //Creating the Join.csv file where we will store our joined results
    fstream joinout;
    joinout.open("Join.csv", ios::out | ios::app);
    if(!joinout.is_open()){
        cerr<<"Can't open file "<<"join.csv"<<endl;
    }

    //1. Create runs for Dept and Emp which are sorted using Sort_Buffer()
    createEmptyRuns(0, emp_runs);
    fillBufferFromFile(buffers, empin, buffer_size, 1);
    Sort_Buffer(buffers, 0, emp_runs);


    //2. Use Merge_Join_Runs() to Join the runs of Dept and Emp relations 



    //Please delete the temporary files (runs) after you've joined both Emp.csv and Dept.csv

    return 0;
}
