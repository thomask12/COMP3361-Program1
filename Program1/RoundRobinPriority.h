/*
 * Simulate the preemptive Round-Robin scheduling algorithm with priorities. 
 * Need to maintain a simulation time in your program, starting at 0. 
 * For each interval, determine the process to run according to the scheduling algorithm, and determine when the interval ends, 
 * depending on the CPU time left until the process blocks, the time slice length, and when the process terminates. 
 * 
 * For either algorithm, processes which re-enter the ready list after being blocked or entering the system should be placed on the end of the list.
 * 
 * Should maintain a separate ready list for each priority. 
 * Only processes from the highest-numbered priority non-empty ready list should execute. 
 * If a higher priority process arrives during the time slice of a lower-priority process, 
 * the lower-priority process should continue to execute until the end of its time slice, 
 * or until it blocks or terminates, whichever comes first.
 */

/*
 * File:   RoundRobinPriority.h
 * Author: kt & om
 *
 * Created on April 24, 2019, 8:20 AM
 */

#ifndef ROUNDROBINPRIORITY_H
#define ROUNDROBINPRIORITY_H

#include <string>
#include <vector>

using namespace std;

class RoundRobinPriority {
public:
    /**
     * RoundRobinPriority - construct for Round Robin Scheduluer
     * @param file_name - file to read in from
     * @param block_time - the decimal integer time length that a process is unavailable to run after it blocks
     * @param time_slice - the decimal integer length of the time slice for the Round-Robin scheduler 
     */
    RoundRobinPriority(string file_name, string block_time, string time_slice);

    virtual ~RoundRobinPriority() {
    }

    //Delete copy and move constructors
    RoundRobinPriority(const RoundRobinPriority & other) = delete;
    RoundRobinPriority(RoundRobinPriority && other) = delete;
    RoundRobinPriority &operator=(RoundRobinPriority && other) = delete;
    RoundRobinPriority &operator=(const RoundRobinPriority) = delete;

private:
    /**
     * Run              manages processes and calculations associated with each running process
     *
     * @param process - vector of strings containing process names
     * @param values -  vector of vectors containing arguments for specified process names
     * @param block -   block duration
     * @param slice -   round robin timer interrupt interval
     */
    void Run(vector<string> process, vector<vector<uint32_t>> values, uint32_t block, uint32_t slice);
    /**
     * Blocker - if a process has reached its block time, add them to blocked list and remove from ready list
     * 
     * @param process - vector of strings containing process names
     * @param values -  vector of vectors containing arguments for specified process names
     * @param block -   block duration
     * @param maxIndex - process index with the highest priority
     */
    void Blocker(vector<string> &process, vector<vector<uint32_t>> &values, uint32_t block, uint32_t maxIndex);
    /**
     * Terminator - if a process has completed, terminates them (adds them to a finished list)
     * 
     * @param process - vector of strings containing process names
     * @param values -  vector of vectors containing arguments for specified process names
     * @param block -   block duration
     * @param maxIndex - process index with the highest priority
     */
    void Terminator(vector<string> &process, vector<vector<uint32_t>> &values, uint32_t block, uint32_t maxIndex);
    /**
     * blockedProc - handles blocking and unblocking
     * 
     * @param process - vector of strings containing process names
     * @param values -  vector of vectors containing arguments for specified process names
     */
    void blockedProc(vector<string> process, vector<vector<uint32_t> > values);
    /**
     * enterReady - adds processes to ready list if they are unblocked or have arrived
     * 
     * @param process - vector of strings containing process names
     * @param values -  vector of vectors containing arguments for specified process names
     * @param count - number of processes ready
     * @return - number of processes added to ready list
     */
    uint32_t enterReady(vector<string> process, vector<vector<uint32_t> > values, int count);
    /**
     * priortityCheck - checks priority of all given processes
     * 
     * @param process - vector of strings containing process names
     * @param values -  vector of vectors containing arguments for specified process names
     * @return  - process index with the highest priority
     */
    uint32_t priorityCheck(vector<vector<uint32_t> > values, vector<string> process);
    /**
     * processToIndex - returns the index of a process with a given value
     * 
     * @param process - vector of strings containing process names
     * @param toCon - process to convert to index
     * @return - index of value toCon in process
     */
    uint32_t processToindex(vector<string> process, uint32_t toCon);
    /**
     * isOnList - checks if a value given its index is on given process
     * 
     * @param valIndex - index for some value
     * @param process - vector of strings containing process names
     * @return - whether valIndex is on given process
     */
    bool isOnlist(uint32_t valIndex, vector<string> process);

    //Simulates timer for interrupts
    uint32_t timer = 0;
    //Vector of currently blocked processes
    vector<string> blocked;
    //Vector of processes ready to run
    vector<string> ready;
    //Vector of completed processes
    vector<string> finished;
    //Vector of turnaround times of processes
    vector<uint32_t> turnaround;
};

#endif /* ROUNDROBINPRIORITY_H */
