# **Taint-Analysis**

## **Introduction**

Taint analysis is a type of static analysis technique used in software security to identify potential vulnerabilities caused by the flow of untrusted or tainted data through a program. The basic idea is to track the flow of data through a program and identify points where potentially dangerous data (such as user input) can be introduced and propagated through the program.

Taint analysis involves marking or tagging the sources of potentially dangerous data, such as user input or data from external sources, as “tainted”. Then, the analysis tracks the propagation of this tainted data through the program to identify any points where it might be used in a way that could lead to security vulnerabilities.

## **Designing Taint Analysis**

### **Lattice**

In data analysis, a lattice is a mathematical structure that represents the partial ordering of a set of values. It is used to model the different possible states that a system can be in, and the relationships between those states. A lattice consists of a set of elements, a partial order relation that defines the ordering between the elements, and two binary operations, called meet and join, which define the least upper bound and greatest lower bound of any subset of the lattice. Lattices are commonly used in data analysis to model the flow of data between different parts of a system, and to track how that data changes over time.

![img](/imgs/img1.png "lattice")

### **May/must analysis**

May analysis: This analysis reports all possible taint variables of data through the program, even if they are not guaranteed. If a flow of data is possible, it is marked as a taint.

Must analysis: This analysis reports only the guaranteed taint variables of data through the program. If a data flow is guaranteed, it is marked as a taint. Any other flows of data are ignored in this analysis.

This design adopted may analysis, as it ensures that if there is any possibility of taint, it will be reported.

### **Forward/Backward analysis**

In data analysis, forward and backward refer to the direction in which the data flow is analyzed.

In forward analysis, the data flow starts from the entry point of the program and propagates forward towards the end point.

In backward analysis, the data flow starts from the end point of the program and propagates backwards towards the entry point.

The direction of the analysis is Forward. The taint flows from source to the sink, i.e., from
the entry to the end.

### **Transfer Functions**

Support Alloca, Add, Sub, Div, Mul, Rem, Load, Store

- Alloca: Since may type analysis is adopted, all possible taint variables should be tagged. Source is the origin of taint variable and alloca of source is considered as taint. Other variables allocated is set to clean, and alloca source is set to tainted.

- Add, Sub, Div, Mul, Rem: rem is the instruction used for performing remainder
operation. These functions are all mathematical instructions, if any of the operands of the instruction are tainted, then the result of the instruction is also tainted.

- Load: load is an instruction that is used to load the value of a variable from memory. The taint of the result is the same as the taint label of the loaded value.

- Store: store is an operation that writes a value to a memory location. If the value being stored is tainted, the taint of the memory location is set to taint.

## **Implementing the Taint Analysis in LLVM**

1. Read IR file using LLVM getGlobalContext, extract module from IR file, then extract main function from the module.

2. Define analysisMap as mapping of basic block labels to empty set of strings.

3. Define succMap as mapping of successor block labels to set of taint variables stored for the successor.

4. Define traversal stack using standard library to store analysisNode, which are pair of basic blocks and set of strings, which are the name of the taint variables.

5. While traversal stack is not empty, traverse the control flow graph in depth first order by poping the top analysisNode on the stack. This is consistent with design of forward type of analysis.

6. Each time analysis a analysisNode, it contains a basis block label and all possible taint variables from predecessor. For each basic block, go through all the instructions and check if any variable become taint. The instructions checked including store instructions, alloca instructions, load instructions and binary operator. Temporary variables are also checked and added to taint set.

7. After checking, the taint variables are updated into analysisMap. If this basic block label has empty set of taint variables, simply update using the current set. Otherwise, use the union of current set and the existing set so that to obtain all the possible taint variables. This is consistent with design of may type of analysis.

8. In while loop, need to check the fixpoint condition, ie only add successor nodes to the stack if the union of the new taint variables for the successor node is different from the currently stored list of taint variables for the successor node. Update the succMap with current existTaintVars set.

9. When the stack is empty, the analysis has completed checking for all basic blocks and collection of taint variables for each basic block. Print the taint variables using function printTaintMap.


## **Conclusion**

By implementing the LLVM pass to traverse the code in depth first order and use union set of new taint variables and existing set of taint variables, the forward, may type analysis of the program has been performed through the module to obtain all the possible taint variables.