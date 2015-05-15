# Branch_Predictor
The goal of this project is to measure the effectiveness of various branch direction
prediction (“taken” or “non-taken”) schemes on several traces of conditional branch
instructions. Each trace contains a variable number of branch instructions, and for each
branch, the program counter (PC, word address is given) and the actual outcome of the
branch are recorded in a trace line. Several trace files are provided for evaluating your
predictor designs.
Your goal is to write a program in C or C++ that would use these traces to measure the
accuracy of various branch predictors that we studied in class. The branch outcomes
from the trace file should be used to train your predictors. Specifically, the following
predictors have to be implemented:
 Always Taken.
 Always Non-Taken.
 Bimodal Predictor with a single bit of history stored in each predictor
entry. Determine the prediction accuracy of this predictor for the table size of 16,
32, 128, 256, 512, 1024 and 2048 entries. Present results for all traces
separately, and the averages across all traces (details of what exactly needs to
be submitted are provided below). Assume that the initial state of all prediction
counters is “Taken” (T)
 Bimodal Predictor with 2-bit saturating counters stored in each
predictor entry. Repeat the same experiments as in part (3) above. Assume that
the initial state of all prediction counters is “Strongly Taken” (TT)
 Gshare predictor, where the PC is XOR-ed with the global history bits to
generate the index into the predictor table. Fix the table size at 2048 entries and
determine the prediction accuracy as a function of the number of bits in the
global history register. Vary the history length from 3 bits to 11 bits in 1-bit
increments. Assume that the initial state of all prediction counters is “Strongly
Taken” (TT). The global history register should be initialized to contain all zeroes
(where 0=NT and 1=T). The global history register should be maintained such
that the least significant bit of the register represents the result of the most recent
branch, and the most significant bit of the register represents the result of the
least recent branch in the history.
 Tournament Predictor. The tournament predictor selects between gshare
and bimodal predictor for every branch. Configure gshare with 2048-entry table 
and 11 bits of global history, and configure bimodal predictor with 2048-entry
table. Furthermore, configure the selector table with 2048 entries and use the
same index as you use for bimodal predictor to index into the selector table (that
is, the PC). For each entry in the selector, the two-bit counter encodes the
following states: 00 – prefer Gshare, 01 – weakly prefer Gshare, 10 – weakly
prefer Bimodal, 11 – prefer bimodal. If the two predictors provide the same
prediction, then the corresponding selector counter remains the same. If one of
the predictors is correct and the other one is wrong, then the selector’s counter is
decremented or incremented to move towards the predictor that was correct.
Initialize all the component predictors to “Strongly Taken” and initialize the
selector’s counters to “Prefer Gshare”. 
