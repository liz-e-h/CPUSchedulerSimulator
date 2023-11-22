#include "scheduler.h"
#include <algorithm>
#include <assert.h>
#include <cmath>
#include <stdio.h>
#include <vector>

// this is the function you should implement
//
// simulate_rr() implements a Round-Robin scheduling simulator
// input:
//   quantum = time slice
//   max_seq_len = maximum length of the reported executing sequence
//   processes[] = list of process with populated IDs, arrivals, and bursts
// output:
//   seq[] - will contain the compressed execution sequence
//         - idle CPU will be denoted by -1
//         - other entries will be from processes[].id
//         - sequence will be compressed, i.e. no repeated consecutive numbers
//         - sequence will be trimmed to contain only first max_seq_len entries
//   processes[]
//         - adjust finish_time and start_time for each process
//         - do not adjust other fields
//

void simulate_rr(
    int64_t quantum, // length of the time slice
    int64_t max_seq_len, // maximum length of execution order to be reported
    std::vector<Process> & processes,
    std::vector<int> & seq)
{
    // helper function
    auto append_seq = [&](int n) {
        if ((int)seq.size() >= max_seq_len) return; // reach end
        if (seq.empty() or seq.back() != n) { // don't add duplicate
            seq.push_back(n);
        }
    };

    seq.clear();

    // initialize local variables
    int64_t curr_time = 0;
    std::vector<int> jq, rq;
    std::vector<int64_t> remb(processes.size()); // remaining burst

    // initialize job queue with all processes
    for (int i = 0; i < int(processes.size()); i++) {
        jq.push_back(i);
        remb[i] = processes[i].burst;
    }

    while (1) {
        /* for debugging
        printf("curr_time: %ld\n", curr_time);
        printf("JQ: ");
        for (auto j : jq) { printf("%d ", j); }
        printf("\nRQ: ");
        for (auto r : rq) { printf("%d ", r); }
        printf("\n");*/

        // top of loop: CPU is idle

        if (rq.empty() and jq.empty()) break;

        // wait for processes to arrive in ready queue
        if (rq.empty() and not jq.empty()) {
            // cpu idle
            if (curr_time < processes[jq.front()].arrival)
                // add "-1" for idle time
                append_seq(-1);
            // job arrives from job queue to ready queue
            curr_time
                = processes[jq[0]].arrival; // set current time to tne next process arrival time
            rq.push_back(jq[0]); // job arrives to ready queue
            jq.erase(jq.begin()); // remove the first job from job queue
            continue;
        }
        // execute time slice from the next process in RQ
        int j = rq.front(); // executing job j
        append_seq(j);
        if (processes[j].start_time == -1) processes[j].start_time = curr_time;
        rq.erase(rq.begin());

        // if the job can be finished within qunatum
        if (remb[j] <= quantum) {
            curr_time += remb[j];
            remb[j] = 0;
            processes[j].finish_time = curr_time;
            // check for processes to arrive
            while (not jq.empty() && processes[jq[0]].arrival <= curr_time) {
                rq.push_back(jq[0]);
                jq.erase(jq.begin());
            }
            continue;
        }

        // the process did not finish
        // time passed: quantum
        curr_time += quantum;
        remb[j] -= quantum;
        // check for processes to arrive
        while (not jq.empty() && processes[jq[0]].arrival < curr_time) {
            rq.push_back(jq[0]);
            jq.erase(jq.begin());
        }
        // insert unfinshed process back to RQ
        rq.push_back(j);
        // any jobs from jq that arrive == curr time go to RQ now
        while (not jq.empty() && processes[jq[0]].arrival == curr_time) {
            rq.push_back(jq[0]);
            jq.erase(jq.begin());
        }

        // if only one job left
        if (rq.size() == 1 && jq.empty()) { // one process in RQ left, end right away
            int a = rq.back();
            append_seq(rq.back());
            rq.pop_back();
            curr_time += remb[a];
            processes[a].finish_time = curr_time;
            break;
        }

        // Fast-forward Optimization
        if ((jq.empty() || (processes[jq[0]].arrival - curr_time) > (int)rq.size() * quantum)) {
            // check if all start time in RQ has been init
            bool cont = false;
            for (auto p : rq) {
                if (processes[p].start_time == -1) {
                    cont = true;
                    continue;
                }
            }
            if (cont) continue; // only optimize when all processes in RQ have start time
            // calculate minimum remaining burst of processes in RQ
            int64_t min_burst = (int64_t)pow(2, 63);
            for (auto p : rq) {
                if (remb[p] < min_burst) { min_burst = remb[p]; }
            }

            int64_t n_1 = min_burst / (quantum * rq.size());
            int64_t n; // # of increments
            if (jq.size() != 0 && rq.size() != 0) {
                int64_t n_2 = (processes[jq[0]].arrival - curr_time) / (rq.size() * quantum);
                n = std::min(n_1, n_2);
            } else {
                if (min_burst == (int64_t)pow(2, 63)) continue;
                n = n_1;
            }

            // only optimze if rq.size() = 1 or seq = max_seq_len
            if (rq.size() == 1) { // special case: only 1 proc in RQ
                curr_time += n * quantum;
                remb[rq.front()] -= n * quantum;
                append_seq(rq.front());
                continue;
            } else if ((int)seq.size() != max_seq_len) // if not
                continue;

            if (n <= 1) continue;
            curr_time += n * rq.size() * quantum;

            // execute every process in ready queue n times
            for (auto & p : rq) { remb[p] -= n * quantum; }
        }
    }
}
