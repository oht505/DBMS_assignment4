/*
 *
 *
 * Assignment 3 Group 2
 * written by Woonki Kim & Hao Truong
 *
 *
 *
 *
 * */
#include <string>
#include <vector>
#include <string>
#include <iostream>
#include <sstream>
#include <bitset>
using namespace std;

class Record {
public:
    int id, manager_id;
    std::string bio, name;

    Record(vector<std::string> fields) {
        id = stoi(fields[0]);
        name = fields[1];
        bio = fields[2];
        manager_id = stoi(fields[3]);
    }

    void print() {
        cout << "\tID: " << id << "\n";
        cout << "\tNAME: " << name << "\n";
        cout << "\tBIO: " << bio << "\n";
        cout << "\tMANAGER_ID: " << manager_id << "\n";
    }
};

struct Slot
{
    int offset; // offset of record in page
    int recordLength; // length of record

    Slot(int offset, int recordLength) : offset(offset), recordLength(recordLength) {}

};

class LinearHashIndex {

private:
    const int BLOCK_SIZE = 4096;
    vector<int> blockDirectory; // Map the least-significant-bits of h(id) to a bucket location in EmployeeIndex (e.g., the jth bucket)
                                // can scan to correct bucket using j*BLOCK_SIZE as offset (using seek function)
								// can initialize to a size of 256 (assume that we will never have more than 256 regular (i.e., non-overflow) buckets)
    int n;  // The number of indexes in blockDirectory currently being used
    int i;	// The number of least-significant-bits of h(id) to check. Will need to increase i once n > 2^i
    int numRecords=0;    // Records currently in index. Used to test whether to increase n
    int nextFreeBlock=BLOCK_SIZE*4; // Next place to write a bucket. Should increment it by BLOCK_SIZE whenever a bucket is written to EmployeeIndex
    string fName;      // Name of index file

    //LSB -> page num
    int getHashedIndex(int id, int bitsInNeed) {

        bitset<8> decToBit =bitset<8>(id % 216);

        // Extract the least significant bits
        bitset<8> lsbBits = decToBit & bitset<8>((1 << bitsInNeed) - 1);


        // Convert the LSB to decimal
        int lsbDecimal = static_cast<int>(lsbBits.to_ulong());

        if (lsbDecimal > n) {
            // Flip the most significant bit
            lsbDecimal ^= (1 << (bitsInNeed - 1));
        }


        return lsbDecimal;
    }

    void initBucket(int startOffset, fstream &indexFile){
        // Seek to the appropriate position at the end of the current page
        indexFile.seekp(startOffset + BLOCK_SIZE - sizeof(int)*3);
        int initFreeSpace = 0;
        int initRecordNumInPage = 0;
        int overflowIndex = -1;
        indexFile.write(reinterpret_cast<const char*>(&overflowIndex), sizeof(int));
        indexFile.write(reinterpret_cast<const char*>(&initRecordNumInPage), sizeof(int));
        indexFile.write(reinterpret_cast<const char*>(&initFreeSpace), sizeof(int));
    }

    void writeRecordToFile(Record record,int startOffset, fstream &indexFile){
        string serializedRecord = serialize(record);
        int nextFreeSpace;
        int RecordNumInPage;
        int overflowIndex;
        int recordLength = serializedRecord.size();

        indexFile.seekg(startOffset + BLOCK_SIZE - sizeof(int)*3);
        indexFile.read(reinterpret_cast<char*>(&overflowIndex), sizeof(int));
        //check to write in overflow page.
        if(overflowIndex !=-1){
            startOffset = overflowIndex;
        }
        indexFile.seekg(startOffset + BLOCK_SIZE - sizeof(int)*3);
        indexFile.read(reinterpret_cast<char*>(&overflowIndex), sizeof(int));
        indexFile.read(reinterpret_cast<char*>(&RecordNumInPage), sizeof(int));
        indexFile.read(reinterpret_cast<char*>(&nextFreeSpace), sizeof(int));

        if(BLOCK_SIZE - nextFreeSpace - sizeof(int)*3 - RecordNumInPage*sizeof(int)*2 > serializedRecord.size() + sizeof(int)*2){
            indexFile.seekp(startOffset + nextFreeSpace);

            // Write the serialized record to the file
            indexFile.write(serializedRecord.c_str(), serializedRecord.size());

            indexFile.seekp(startOffset + BLOCK_SIZE - (sizeof(int)*3+sizeof(int)*2*(RecordNumInPage+1)));
            indexFile.write(reinterpret_cast<const char*>(&recordLength), sizeof(int)); // Write record length
            indexFile.write(reinterpret_cast<const char*>(&nextFreeSpace), sizeof(int)); // Write offset

            // Update the next free space pointer
            nextFreeSpace += recordLength;
            RecordNumInPage++;

            // Write the updated next free space pointer back to the file
            indexFile.seekp(startOffset + BLOCK_SIZE - sizeof(int));
            indexFile.write(reinterpret_cast<const char*>(&nextFreeSpace), sizeof(int));
            indexFile.seekp(startOffset + BLOCK_SIZE - sizeof(int)*2);
            indexFile.write(reinterpret_cast<const char*>(&RecordNumInPage), sizeof(int));

            numRecords++;
        }
        //overflow occurs.
        else{
            overflowIndex = nextFreeBlock;
            indexFile.seekp(startOffset + BLOCK_SIZE - sizeof(int)*3);
            indexFile.write(reinterpret_cast<const char*>(&overflowIndex), sizeof(int));

            initBucket(overflowIndex,indexFile);
            //blockDirectory[bucketNum] = nextFreeBlock;
            nextFreeBlock+=BLOCK_SIZE;
            writeRecordToFile(record , startOffset,indexFile);
        }

    }
    void redistributeRecords(int oldBucketNum,int newBucketNum,fstream &existingPage, fstream &indexFile) {

        //Read page that has possibility of containing record that should fit in new bucket.
        std::streampos start_position = blockDirectory[oldBucketNum];
        std::streampos end_position = blockDirectory[oldBucketNum] + BLOCK_SIZE;
        int startOffset = blockDirectory[oldBucketNum];
        int newBucketStartOffset = blockDirectory[newBucketNum];


        existingPage.seekg(start_position);
        std::string content;
        content.resize(static_cast<size_t>(end_position - start_position));
        existingPage.read(&content[0], end_position - start_position);

        vector<Slot> slots;
        int slotOffset;
        int recordLength;
        int RecordNumInPage;
        int overflowOffset;

        existingPage.seekg(startOffset + BLOCK_SIZE - sizeof(int) * 3);
        existingPage.read(reinterpret_cast<char *>(&overflowOffset), sizeof(int));
        existingPage.read(reinterpret_cast<char *>(&RecordNumInPage), sizeof(int));

        blockDirectory[oldBucketNum] = nextFreeBlock;
        initBucket(nextFreeBlock,indexFile);
        nextFreeBlock+=BLOCK_SIZE;
        for (int j = 0; j < RecordNumInPage; j++) {
            existingPage.seekg(startOffset + BLOCK_SIZE - sizeof(int) * 3 - (RecordNumInPage - j) * 2 * sizeof(int));
            existingPage.read(reinterpret_cast<char *>(&recordLength), sizeof(int));
            existingPage.read(reinterpret_cast<char *>(&slotOffset), sizeof(int));
            slots.push_back(Slot(slotOffset, recordLength));

            int recordId, managerId;
            string name, bio;
            existingPage.seekg(startOffset + slotOffset);
            existingPage.read(reinterpret_cast<char*>(&recordId), sizeof(int));
            std::getline(existingPage, name, ',');
            std::getline(existingPage, bio, ',');
            existingPage.read(reinterpret_cast<char*>(&managerId), sizeof(int));

            bitset<8> decToBit =bitset<8>(n % 216);

            // Extract the least significant bits
            bitset<8> lsbBits = decToBit & bitset<8>((1 << i) - 1);

            cout << lsbBits;
            // Convert the LSB to decimal

            cout << "=================spliting bucket : " << oldBucketNum;

            //split bucket.
            if(getHashedIndex(recordId,i) == oldBucketNum){
                numRecords--;
                writeRecordToFile(Record({to_string(recordId), name, bio, to_string(managerId)}),blockDirectory[oldBucketNum],indexFile);
                cout <<"told : " <<recordId <<"\n";
            }
            else if(getHashedIndex(recordId,i) == newBucketNum){
                numRecords--;
                writeRecordToFile(Record({to_string(recordId), name, bio, to_string(managerId)}),blockDirectory[newBucketNum],indexFile);
                cout <<"tonew : " <<recordId <<"\n";
            }


        }
    }


    // this function add a new bucket to the block director and redistribute records (records that belong to where they should be)
    void rehash(fstream &indexFile) {
        cout <<"rehash";
        n++;
        blockDirectory.push_back(nextFreeBlock);
        initBucket(nextFreeBlock,indexFile);
        nextFreeBlock+=BLOCK_SIZE;


        //check if i needs increment.
        if(n > pow(2,i)){
            i++;
        }

        std::bitset<8> decToBit(n); // Assuming n fits within 8 bits

        // Extract the least significant i bits
        std::bitset<8> lsbBits = decToBit & std::bitset<8>((1 << i) - 1);

        // Flip the ith bit
        std::bitset<8> Bits = lsbBits;
        Bits.flip(i-1);

        // Convert bitsets back to integers
        int newBucketNum = n;
        int oldBucketNum = static_cast<int>(Bits.to_ulong());

        redistributeRecords(oldBucketNum,newBucketNum,indexFile,indexFile);

    }

    // Insert new record into index
    void insertRecord(Record record, fstream &indexFile) {
        string serializedRecord = serialize(record);
        int bucketNum = getHashedIndex(record.id,i);
        //TODO: No records written to index yet
        if (numRecords == 0) {
            // Initialize index with first blocks (start with 4)
            for(int index = 0; index <4; index++){
                blockDirectory.push_back(BLOCK_SIZE*index);
                initBucket(blockDirectory[index],indexFile);
            }
        }

        int startOffset=blockDirectory[bucketNum];
        if (static_cast<float>(numRecords) / static_cast<float>(n * 8) > 0.7) {
            // rehashing logic
            rehash(indexFile);
        }
        else{
            writeRecordToFile(record, startOffset,indexFile);
        }

    }

    string serialize(const Record &record)
    {
        ostringstream serializedRecord;
        serializedRecord.write(reinterpret_cast<const char *>(&record.id), sizeof(record.id));                 // serialize int id field, fixed size
        serializedRecord << record.name <<',';                                                                // serialize string name, variable length
        serializedRecord << record.bio << ',';                                                                 // serialize string bio, variable length
        serializedRecord.write(reinterpret_cast<const char *>(&record.manager_id), sizeof(record.manager_id)); // serialize int manager id field, fixed size
        return serializedRecord.str();
    }


public:
    LinearHashIndex(string indexFileName) {
        n = 3; // Start with 4 buckets in index
        i = 2; // Need 2 bits to address 4 buckets
        numRecords = 0;
        fName = indexFileName;

        // Create your EmployeeIndex file and write out the initial 4 buckets
        // make sure to account for the created buckets by incrementing nextFreeBlock appropriately
    }

    // Read csv file and add records to the index
    void createFromFile(string csvFName){
        fstream indexFile(fName, ios::in | ios::out | ios::trunc | ios::binary);
        ifstream file(csvFName);

        if (!file.is_open())
        {
            cerr << "Error opening file: " << csvFName << endl;
            exit(1);
        }

        string parseStr;
        while (getline(file, parseStr)){
            istringstream strStream(parseStr);
            vector<string> fields;
            string field;

            while (getline(strStream, field, ',')){
                fields.push_back(field);
            }
            Record record(fields);
            insertRecord(record,indexFile);
        }

        file.close();
    }

    Record findRecordById(int id) {
        fstream indexFile(fName, ios::in | ios::out | ios::binary);

        int bucketNum = getHashedIndex(id,i);
        int startOffset = blockDirectory[bucketNum];

        return searchPageById(startOffset,indexFile,id);

    }

    Record searchPageById(int startOffset, fstream &indexFile ,int id){
        vector<Slot> slots;
        int slotOffset;
        int recordLength;
        int RecordNumInPage;
        int overflowOffset;

        indexFile.seekg(startOffset + BLOCK_SIZE - sizeof(int)*3);
        indexFile.read(reinterpret_cast<char*>(&overflowOffset), sizeof(int));
        indexFile.read(reinterpret_cast<char*>(&RecordNumInPage), sizeof(int));

        for(int j=0; j<RecordNumInPage; j++){
            indexFile.seekg(startOffset + BLOCK_SIZE - sizeof(int)*3 - (RecordNumInPage-j)*2*sizeof(int));
            indexFile.read(reinterpret_cast<char*>(&recordLength), sizeof(int));
            indexFile.read(reinterpret_cast<char*>(&slotOffset), sizeof(int));
            slots.push_back(Slot(slotOffset,recordLength));

            int recordId, managerId;
            string name, bio;
            indexFile.seekg(startOffset + slotOffset);
            indexFile.read(reinterpret_cast<char*>(&recordId), sizeof(int));

            if(id == recordId){
                cout << "\n=============Printing Record with ID : "<< recordId <<" =============\n" ;
                std::getline(indexFile, name, ',');
                std::getline(indexFile, bio, ',');
                indexFile.read(reinterpret_cast<char*>(&managerId), sizeof(int));

                return Record({to_string(recordId), name, bio, to_string(managerId)});
            }
        }
        if(overflowOffset !=-1){
            return searchPageById(overflowOffset,indexFile,id);
        }

        else{
            throw runtime_error("No record found with id: " + to_string(id));
        }


    }



};
