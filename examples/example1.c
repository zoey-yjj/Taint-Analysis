
// task 2 write an LLVM pass to perform the analysis on loop-free programs

// example 1 loop-free program


int main() {
    int i, j, k, sink, source;  // entry block
    source = 1234567;
    // read source from input or initilized with
    // a tainted value. e.g. source = 1234567
    i = source;   // i is initialised with source value

    if (j > 1) {  // if.then block
        // skip;
    } else {        // if.else block: k is initialised with i, which is source value
        k = i;
    }
   
    sink = k;     // if.end block: sink is k, maybe not initialised or source value
}

/*
The generated LLVM IR will have four basic blocks with labels: entry, if.then, if.else and if.end. In the
end of each of the basic blocks the list of tainted variables would be as follows (registers are skipped for
brevity):
    • entry: {source, i}
    • if.then: {source, i}
    • if.else: {source, i, k}
    • if.end: {source, i, k, sink}

Since at the last basic block, “if.end”, sink is in the list of tainted variables, we can infer sink might be a
potentially dangerous tainted variable.
*/