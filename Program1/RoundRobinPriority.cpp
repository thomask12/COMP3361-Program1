/*
 * File:   RoundRobinPriority.cpp
 * Author: kt & om
 *
 * Created on April 24, 2019, 8:20 AM
 */
#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <string>
#include "RoundRobinPriority.h"

using namespace std;

RoundRobinPriority::RoundRobinPriority(string file_name, string block_time, string time_slice) {
    ifstream inputFile;
    inputFile.open(file_name);
    vector<string> commandArgs;
    vector<string> procName;
    string line;
    if (!inputFile.is_open()) {
        cerr << "Cannot open file: " << file_name;
        exit(2);
    }
    //Parses entire line and stores inside commandArgs vector
    while (getline(inputFile, line)) {
        commandArgs.push_back(line);
    }
    vector<vector < uint32_t >> run(commandArgs.size());
    for (int i = 0; i < commandArgs.size(); i++) {
        stringstream stream(commandArgs[i]);
        string word;
        int j = 0;
        while (stream >> word) {
            if (j == 0) {
                procName.push_back(word);
            } else {
                stringstream ss;
                uint32_t numArgs;
                ss << dec << word;
                ss >> numArgs;
                run.at(i).push_back(numArgs);
            }
            j++;
        }
    }
    uint32_t block;
    uint32_t slice;
    stringstream argStream;
    argStream << dec << block_time;
    argStream >> block;
    argStream.clear();
    argStream << dec << time_slice;
    argStream >> slice;
    cout << block << " " << slice << "\n";
    Run(procName, run, block, slice);
}

void RoundRobinPriority::Run(vector<string> process, vector<vector<uint32_t> > values, uint32_t block, uint32_t slice) {
    int inCount = 0;
    for (int i = 0; i < process.size(); i++) {
        //Values[4] == time left until blocked
        values[i].push_back(values[i][3]);
    }
    int i = 0;
    while (finished.size() < process.size()) {
        if (inCount < process.size()) {
            //Finds when processes start time is past and they are added to the ready list
            inCount = enterReady(process, values, inCount);
        }

        //Check which processes are ready
        if (ready.size() > 0) {
            if (blocked.size() > 0) {
                blockedProc(process, values);
            }
            uint32_t maxIndex = priorityCheck(values, process);
            if (values[maxIndex][2] > slice) {
                if (values[maxIndex][4] > slice) {
                    //Decreases time from process running and from time remaining until blocked
                    values[maxIndex][2] -= slice;
                    values[maxIndex][4] -= slice;
                    //print
                    cout << " " << timer << "\t" << process[maxIndex] << "\t" << slice << "\t" << "S\n";
                    timer += slice;
                } else {
                    Blocker(process, values, block, maxIndex);
                }
            } else {
                if (values[maxIndex][2] > values[maxIndex][4]) {
                    Blocker(process, values, block, maxIndex);
                } else {
                    Terminator(process, values, block, maxIndex);
                }

            }
        } else {
            blockedProc(process, values);
        }
    }
    float average = 0;
    for (int i = 0; i < turnaround.size(); i++) {
        average += turnaround[i];
    }
    average /= turnaround.size();
    cout << " " << timer << "\t(END)\t" << average << "\n";
}

void RoundRobinPriority::Blocker(vector<string> &process, vector<vector<uint32_t> > &values, uint32_t block, uint32_t maxIndex) {
    cout << " " << timer << "\t" << process[maxIndex] << "\t" << values[maxIndex][4] << "\t" << "B\n";
    timer += values[maxIndex][4];
    //Adds time blocked to values[i][5];
    values[maxIndex].push_back(timer + block);
    values[maxIndex][2] -= values[maxIndex][4];
    values[maxIndex][4] = values[maxIndex][3];
    //Adds to blocked list
    blocked.push_back(process[maxIndex]);
    //Removes from ready list
    uint32_t max = processToindex(process, maxIndex);
    ready.erase(ready.begin() + (max));
}

void RoundRobinPriority::Terminator(vector<string>& process, vector<vector<uint32_t> >& values, uint32_t block, uint32_t maxIndex) {
    cout << " " << timer << "\t" << process[maxIndex] << "\t" << values[maxIndex][2] << "\t" << "T\n";
    timer += values[maxIndex][2];
    uint32_t max = processToindex(process, maxIndex);
    finished.push_back(process[maxIndex]);
    ready.erase(ready.begin() + max);
    //Subtracts start time from current time to get turnaround
    turnaround.push_back(timer - values[maxIndex][1]);
}

void RoundRobinPriority::blockedProc(vector<string> process, vector<vector<uint32_t> > values) {
    //Checks (the index of) processes present in the blocked list
    vector<uint32_t> blockedValueIndex;
    for (int i = 0; i < blocked.size(); i++) {
        for (int j = 0; j < process.size(); j++) {
            if (blocked[i] == process[j]) {
                blockedValueIndex.push_back(j);
            }
        }
    }
    //If there are still processes are present in the ready list (system not idle)
    if (ready.size() > 0) {
        //Always releases first value in blocked list because it will always be the first process to be unblocked
        //int blockCount;
        int blockSize = blocked.size();
        for (int i = 0; i < blockSize; i++) {
            if (values[blockedValueIndex[i]][values[blockedValueIndex[i]].size() - 1] <= timer) {
                values[blockedValueIndex[i]].pop_back();
                ready.push_back(process[blockedValueIndex[i]]);
                blocked.erase(blocked.begin());
            }
        }
    }//If there are no processes in ready list (system is idle)
    else {
        uint32_t blocks = values[blockedValueIndex[0]][values[blockedValueIndex[0]].size() - 1];
        uint32_t enterIndex = (ready.size() + blocked.size() + finished.size());
        //find greatest number process that can be executed before blocked process is released
        //Finds index with smallest time until unblocked
        // values[0][values[0].size() - 1] == time that the given process will unblock
        if (enterIndex < values.size()) {
            if (values[enterIndex][1] < blocks) {
                cout << " " << timer << "\t(IDLE)\t" << values[enterIndex][1] - timer << "\t" << "I\n";
                timer += values[enterIndex][1] - timer;
            } else {
                cout << " " << timer << "\t(IDLE)\t" << blocks - timer << "\t" << "I\n";
                timer += blocks - timer;
                values[blockedValueIndex[0]].pop_back();
                ready.push_back(process[blockedValueIndex[0]]);
                blocked.erase(blocked.begin());
            }
        } else if (blocks > timer) {
            //Needs to factor in the case where a other process enters the ready list << This is where we get our error <<
            //Must factor in case where entered value starts after idle not just freeing the first blocked value
            cout << " " << timer << "\t(IDLE)\t" << blocks - timer << "\t" << "I\n";
            timer += blocks - timer;
            values[blockedValueIndex[0]].pop_back();
            ready.push_back(process[blockedValueIndex[0]]);
            blocked.erase(blocked.begin());
        } else {
            values[blockedValueIndex[0]].pop_back();
            ready.push_back(process[blockedValueIndex[0]]);
            blocked.erase(blocked.begin());
        }
    }
}

uint32_t RoundRobinPriority::enterReady(vector<string> process, vector<vector<uint32_t> > values, int count) {
    //reset I to count so it does not repeatedly add the same process
    for (int i = count; i < process.size(); i++) {
        if (timer >= values[i][1]) {
            ready.push_back(process[i]);
            count++;
        }
    }
    return count;
}

uint32_t RoundRobinPriority::priorityCheck(vector<vector<uint32_t> > values, vector<string> process) {
    //Finds highest priority process in ready list
    int maxP = 0;
    int maxI = 0;
    vector<uint32_t> readyIndex;
    //Finds highest priority process in ready
    for (int i = 0; i < ready.size(); i++) {
        for (int j = 0; j < process.size(); j++) {
            if (process[j] == ready[i]) {
                readyIndex.push_back(j);
            }
        }
    }
    //firstRed[i] == readyIndex[i] index value at ready index corresponds with firstRed[i];
    for (int i = 0; i < ready.size(); i++) {
        if (values[readyIndex[i]][0] + 1 > maxP) {
            maxP = values[readyIndex[i]][0] + 1;
            maxI = readyIndex[i];
        }
    }
    return maxI;
}

uint32_t RoundRobinPriority::processToindex(vector<string> process, uint32_t toCon) {
    uint32_t index = 0;
    for (int i = 0; i < ready.size(); i++) {
        if (process[toCon] == ready[i]) {
            index = i;
        }
    }
    return index;
}

bool RoundRobinPriority::isOnlist(uint32_t valIndex, vector<string> process) {
    for (int i = 0; i < process.size(); i++) {
        if (i < ready.size() || i < blocked.size()) {
            if (process[valIndex] == ready[i] || process[valIndex] == blocked[i]) {
                return true;
            }
        }
    }
    return false;
}
