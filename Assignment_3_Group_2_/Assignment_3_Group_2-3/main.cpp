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
#include <ios>
#include <fstream>
#include <vector>
#include <string>
#include <string.h>
#include <iostream>
#include <sstream>
#include <stdexcept>
#include <cmath>
#include <chrono>
#include "classes.h"
using namespace std;
void readTwoPossibleNums(int num, int& out1, int& out2) {
    int bit = num & 1; // Extract the least significant bit
    out1 = num << 1;   // Left shift by one bit to append 0
    out2 = out1 | 1;   // Set the least significant bit to 1

    // If the read bit is 0, swap the outputs
    if (bit == 0) {
        int temp = out1;
        out1 = out2;
        out2 = temp;
    }

}

int main(int argc, char* const argv[]) {

    // Create the index
    LinearHashIndex emp_index("EmployeeIndex");
    emp_index.createFromFile("Employee.csv");
    int id;
    while (true) {
        cout << "\nEnter employee ID (or -1 to quit): ";
        // Input validation
        if (!(cin >> id)) {
            // If the input is not an integer
            cout << "Error: Please enter a valid integer." << endl;
            cin.clear();  // Clear the error flag
            cin.ignore(numeric_limits<streamsize>::max(), '\n');  // Discard invalid input
            continue;  // Restart the loop
        }

        if (id == -1) {
            break;
        }

        try {
            Record foundRecord = emp_index.findRecordById(id);
            foundRecord.print();
        } catch (const std::runtime_error& e) {
            cout << "Error: " << e.what() << endl;
        }
    }

    return 0;
}

