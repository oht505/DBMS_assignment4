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
#define BLOCK_SIZE 1280

Records buffers[buffer_size]; //use this class object of size 22 as your main memory

/***You can change return type and arguments as you want.***/

//====================PASS 1==========================

void flushBuffer(Records buffers[]){
    // Empty Buffer
    for(int i=0; i<buffer_size; i++){
        buffers[i].emp_record.eid = INT32_MAX;
        buffers[i].emp_record.ename = "";
        buffers[i].emp_record.age = INT32_MAX;
        buffers[i].emp_record.salary = INT32_MAX;

        buffers[i].dept_record.did = INT32_MAX;
        buffers[i].dept_record.dname = "";
        buffers[i].dept_record.budget = INT32_MAX;
        buffers[i].dept_record.managerid = INT32_MAX;
    }
}

int countNumRecordsInFile(istream& inputFile){
    int result=0;
    string line;

    while(getline(inputFile, line, '\n')){
        result++;
    }

    return result;
}

void fillBufferFromEmp(Records buffers[], fstream &dataFile, int n, bool isPassOne){

    // Grab employee records from the file
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

void fillBufferFromDept(Records buffers[], fstream &dataFile, int n, bool isPassOne){

    // Grab dept records from the file
    for(int i=0; i<n; i++){
        Records deptRecord;
        // Get one record
        if(isPassOne) {
            deptRecord = Grab_Dept_Record(dataFile);
            // For checking the total number of records
            buffers->number_of_dept_records++;

            // No more records in the file
            if(deptRecord.no_values == -1){
                break;
            }
        }

        // Insert each field data into dept_record
        buffers[i].dept_record.did = deptRecord.dept_record.did;
        buffers[i].dept_record.dname = deptRecord.dept_record.dname;
        buffers[i].dept_record.budget = deptRecord.dept_record.budget;
        buffers[i].dept_record.managerid = deptRecord.dept_record.managerid;

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

string serialize(Records empInfo, int flag)
{
    ostringstream serializedRecord;

    // Emp
    if(flag==0){
        serializedRecord.write(reinterpret_cast<const char *>(&empInfo.emp_record.eid), sizeof(int));
        serializedRecord << empInfo.emp_record.ename <<",";
        serializedRecord.write(reinterpret_cast<const char*>(&empInfo.emp_record.age), sizeof(int));                                                                 // serialize string bio, variable length
        serializedRecord.write(reinterpret_cast<const char *>(&empInfo.emp_record.salary), sizeof(double));
    }

    // Dept
    if(flag==1){
        serializedRecord.write(reinterpret_cast<const char *>(&empInfo.dept_record.did), sizeof(int));
        serializedRecord << empInfo.dept_record.dname <<",";
        serializedRecord.write(reinterpret_cast<const char*>(&empInfo.dept_record.budget), sizeof(double));                                                                 // serialize string bio, variable length
        serializedRecord.write(reinterpret_cast<const char *>(&empInfo.dept_record.managerid), sizeof(int));
    }

    return serializedRecord.str();
}

void writeRecordToFile(Records buffers[], int bufferIdx, int startOffset, fstream &runFile, int flag){

    if (buffers[bufferIdx].emp_record.eid != INT32_MAX || buffers[bufferIdx].dept_record.managerid != INT32_MAX ) {

        string serializedRecord = serialize(buffers[bufferIdx], flag);
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
void Sort_Buffer(Records buffers[], int startOffset, fstream &RunFile, int flag){
    //Remember: You can use only [AT MOST] 22 blocks for sorting the records / tuples and create the runs

    if(flag==0){
        sortRecordsByEmployeeId();
        for(int bufferIdx=0; bufferIdx<buffer_size; bufferIdx++){
            writeRecordToFile(buffers, bufferIdx, startOffset, RunFile, flag);
        }
    }

    if(flag==1){
        sortRecordsByManagerId();
        for(int bufferIdx=0; bufferIdx<buffer_size; bufferIdx++){
            writeRecordToFile(buffers, bufferIdx, startOffset, RunFile, flag);
        }
    }
}

//====================PASS 2==========================
//get records from EmpRun file.
Records getEmpRecordFromRun(fstream& runFile, int pageNum, int recordNum) {
    Records records;
    int slotOffset, recordLength, RecordNumInPage;
    int startOffset = BLOCK_SIZE*pageNum;
    runFile.seekg(startOffset + BLOCK_SIZE - sizeof(int) * 2);
    runFile.read(reinterpret_cast<char *>(&RecordNumInPage), sizeof(int));

    int eid;
    string ename;
    int age;
    double salary;

    runFile.seekg(startOffset + BLOCK_SIZE - sizeof(int)*3 - recordNum*2*sizeof(int));
    runFile.read(reinterpret_cast<char*>(&slotOffset), sizeof(int));
    runFile.read(reinterpret_cast<char*>(&recordLength), sizeof(int));

    runFile.seekg(startOffset+slotOffset);
    runFile.read(reinterpret_cast<char*>(&eid), sizeof(int));
    std::getline(runFile, ename, ',');
    runFile.read(reinterpret_cast<char*>(&age), sizeof(int));
    runFile.read(reinterpret_cast<char*>(&salary), sizeof(double));
    if(recordNum == RecordNumInPage) return records.initEmpRecord(INT32_MAX,"-1", -1, -1);
    else return records.initEmpRecord(eid,ename,age,salary);

}
//get records from deptRun file.
Records getDeptRecordFromRun(fstream & runFile, int recordNum) {
    Records records;
    int slotOffset, recordLength, minPointer;
    int startOffset = 0;

    int did;
    string dname;
    double budget;
    int managerid;

    runFile.seekg(startOffset + BLOCK_SIZE - sizeof(int)*3);
    runFile.read(reinterpret_cast<char*>(&minPointer), sizeof(int));
    runFile.seekg(startOffset + BLOCK_SIZE - sizeof(int)*3 - recordNum*2*sizeof(int));
    runFile.read(reinterpret_cast<char*>(&slotOffset), sizeof(int));
    runFile.read(reinterpret_cast<char*>(&recordLength), sizeof(int));

    runFile.seekg(startOffset+slotOffset);
    runFile.read(reinterpret_cast<char*>(&did), sizeof(int));
    std::getline(runFile, dname, ',');
    runFile.read(reinterpret_cast<char*>(&budget), sizeof(double));
    runFile.read(reinterpret_cast<char*>(&managerid), sizeof(int));
    return records.initDeptRecord(did,dname,budget,managerid);
}

void changeEmp(Records buffers[buffer_size], fstream &empRun,int n, int pageNum){
    int recordPerPage = (buffer_size-2)/pageNum;
    for(int p =0; p<pageNum; p++){
        for(int i=0; i<recordPerPage; i++){
            int recordNum =n*recordPerPage + i +1;
            Records empRecord = getEmpRecordFromRun(empRun, p,recordNum);
            if(empRecord.emp_record.eid != INT32_MAX)
                buffers[(p*recordPerPage)+i] = empRecord;
        }
    }

}

void changeDept(Records buffers[buffer_size], fstream &deptRun,int recordNum){
    //cout << recordNum <<"\n";
    buffers[buffer_size-2] = getDeptRecordFromRun(deptRun,recordNum);
}

//Prints out the attributes from empRecord and deptRecord when a join condition is met
//and puts it in file Join.csv
void PrintJoin() {

    return;
}

void printBuffer(){
    for(int i=0; i<buffer_size-2;i++){
        cout << buffers[i].emp_record.eid<<"\n";
        cout << buffers[i].emp_record.ename<<"\n";
        cout << buffers[i].emp_record.salary<<"\n";
        cout << buffers[i].emp_record.age<<"\n";
    }
    cout << buffers[buffer_size-2].dept_record.did<<"\n";
    cout << buffers[buffer_size-2].dept_record.dname<<"\n";
    cout << buffers[buffer_size-2].dept_record.budget<<"\n";
    cout << buffers[buffer_size-2].dept_record.managerid<<"\n";

}

//Use main memory to Merge and Join tuples
//which are already sorted in 'runs' of the relations Dept and Emp
void Merge_Join_Runs(fstream &empRun, fstream &deptRun){
    int recordNum = 1;
    int nForEmp =0;
    bool hasJoined= false;
    flushBuffer(buffers);
    changeEmp(buffers, empRun, 0,2);
    //sortRecordsByEmployeeId();
    printBuffer();
    //and store the Joined new tuples using PrintJoin()
    return;
}




int main() {

    int totalNumEmpRecords = 0;
    int totalNumEmpPages = 0;
    int totalNumDeptRecords = 0;
    int totalNumDeptPages = 0;

    //Open file streams to read and write
    //Opening out two relations Emp.csv and Dept.csv which we want to join
    fstream empin;
    empin.open("Emp.csv", ios::in);
    if(!empin.is_open()){
        cerr<<"Can't open file "<<"Emp.csv"<<endl;
    }

    fstream deptin;
    deptin.open("Dept.csv", ios::in);
    if(!deptin.is_open()){
        cerr<<"Can't open file "<<"Dept.csv"<<endl;
    }

    // Total number of Records and Runs in Emp.csv and Dept.csv
    totalNumEmpRecords = countNumRecordsInFile(empin);
    totalNumEmpPages = totalNumEmpRecords / 15 + (totalNumEmpRecords % 15 != 0);

    totalNumDeptRecords = countNumRecordsInFile(deptin);
    totalNumDeptPages = totalNumDeptRecords / 15 + (totalNumDeptRecords % 15 != 0);

    // Clear flags and Move pointer to the beginning of the file
    empin.clear();
    empin.seekg(0, ios::beg);

    deptin.clear();
    deptin.seekg(0, ios::beg);

    cout<<totalNumEmpRecords<<","<<totalNumEmpPages<<","<<totalNumDeptRecords<<","<<totalNumDeptPages<<endl;

    // Create Emp Runs File
    fstream emp_runs;
    emp_runs.open("Emp_runs", ios::in | ios::out | ios:: trunc | ios::binary);
    if(!emp_runs.is_open()){
        cerr<<"Can't open file "<<"Emp_runs.csv"<<endl;
    }

    // Create Dept Runs File
    fstream dept_runs;
    dept_runs.open("Dept_runs", ios::in | ios::out | ios:: trunc | ios::binary);
    if(!dept_runs.is_open()){
        cerr<<"Can't open file "<<"dept_runs.csv"<<endl;
    }

    //Creating the Join.csv file where we will store our joined results
    fstream joinout;
    joinout.open("Join.csv", ios::out | ios::app);
    if(!joinout.is_open()){
        cerr<<"Can't open file "<<"join.csv"<<endl;
    }

    //1. Create runs for Dept and Emp which are sorted using Sort_Buffer()
    // Emp_runs
    int flagForFile = 0;
    int isPassone = 1;
    for(int i=0; i<totalNumEmpPages; i++){
        createEmptyRuns(i*BLOCK_SIZE, emp_runs);
        flushBuffer(buffers);
        fillBufferFromEmp(buffers, empin, 15, isPassone);
        Sort_Buffer(buffers, i*BLOCK_SIZE, emp_runs, flagForFile);
    }

    // dept_runs.clear();
    // dept_runs.seekg(0, ios::beg);

    // Dept_runs
    flagForFile = 1;
    for(int i=0; i<totalNumDeptPages; i++){
        createEmptyRuns(i*BLOCK_SIZE, dept_runs);
        flushBuffer(buffers);
        fillBufferFromDept(buffers, deptin, 15, isPassone);
        Sort_Buffer(buffers, i*BLOCK_SIZE, dept_runs, flagForFile);
    }


    //2. Use Merge_Join_Runs() to Join the runs of Dept and Emp relations
    Merge_Join_Runs(emp_runs,dept_runs);

    //Please delete the temporary files (runs) after you've joined both Emp.csv and Dept.csv

    return 0;

}
