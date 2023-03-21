#include <cstdio>
#include <iostream>
#include <set>
#include <map>
#include <stack>

#include "llvm/IR/LLVMContext.h"
#include "llvm/IR/Module.h"
#include "llvm/IR/Function.h"
#include "llvm/IR/Instructions.h"
#include "llvm/IRReader/IRReader.h"
#include "llvm/Support/SourceMgr.h"
#include "llvm/Support/raw_ostream.h"
#include "llvm/IR/Constants.h"

using namespace llvm; // mainly use construct from llvm


std::set<std::string> findTaintVars(BasicBlock* BB, std::set<std::string> taintVars);
std::set<std::string> intersect_sets(std::set<std::string> A, std::set<std::string> B);
std::set<std::string> union_sets(std::set<std::string> A, std::set<std::string> B);


int main(int argc, char **argv) {
    // Read the IR file.
    LLVMContext &Context = getGlobalContext(); 
    SMDiagnostic Err;

    // Extract Module M from IR (assuming only one Module exists)
    Module *M = ParseIRFile(argv[1], Err, Context);
    if (M == nullptr) {
        fprintf(stderr, "error: failed to load LLVM IR file \"%s\"", argv[1]);
        return EXIT_FAILURE;
    }

    // 1.Extract Function main from Module M
    Function *F = M->getFunction("main");
    
    // 2.Define analysisMap as a mapping of basic block labels to empty set (of instructions):
    std::map<std::string, std::set<std::string>> analysisMap;
    for (auto &BB: *F) {
        std::set<std::string> emptySet;
    	analysisMap[getSimpleNodeLabel(&BB)] = emptySet;
    }

    // 2.1 Define succMap to store successor and the set of taint variables stored for the successor
    std::map<std::string, std::set<std::string>> succMap;

    // 3. Traversing the CFG in Depth First Order
    std::stack<std::pair<BasicBlock*, std::set<std::string> > > traversalStack;
    BasicBlock* entryBB = &F->getEntryBlock();
    std::set<std::string> emptySet;
    std::pair<BasicBlock*, std::set<std::string> > analysisNode = std::make_pair(entryBB, emptySet);
    traversalStack.push(analysisNode); 

    // 4. while the stack is not empty we pop the top analysisNode
    while(!traversalStack.empty()) {
        // Pop the top analysis node from stack
        std::pair<BasicBlock*, std::set<std::string> > analysisNode = traversalStack.top();
       	traversalStack.pop();
       	
	    // Extract the basic block and the set of taint variables from  analysisNode
	    BasicBlock* BB = analysisNode.first; 
      	std::set<std::string> entryTaintVars = analysisNode.second; 

        // Extract updatedTaintVars (The list of taint variables after BB) from BB and taintVars.
        // By processing each instruction in BasicBlock, get to know what are the variables get taint in a particular BasicBlock
        std::set<std::string> updatedTaintVars = findTaintVars(BB, entryTaintVars); 
        
        // Update the analysis of node BB in the MAP to the union of currently stored taintVars and the generated updatedTaintVars
        std::set<std::string> exitTaintVars;

        if (analysisMap[getSimpleNodeLabel(BB)].empty()) {
        	exitTaintVars = updatedTaintVars;
        } else {
        	exitTaintVars = union_sets(analysisMap[getSimpleNodeLabel(BB)], updatedTaintVars); 
        }

    	analysisMap[getSimpleNodeLabel(BB)] = exitTaintVars;

    }

    return 0;
}


std::set<std::string> findTaintVars(BasicBlock* BB, std::set<std::string> taintVars) {
    // updatedTaintVars is first initialized to the current list of taint variables 
    std::set<std::string> updatedTaintVars(taintVars);

    // trace the taint values by variable name source
    StringRef expectedName = "source"; 
    
    // Loop through instructions in BB
    for (auto &I: *BB) {

        // Check if an Instruction is of the type Store Instruction
        // Other variables will get taint when they store value from taint variable
        if (isa<StoreInst>(I)) {

            // Load Arg1 to an instance of the class Value 
            Value* v = I.getOperand(1);
            
            // Convert the instance of Value class to a variable
            Instruction* var = dyn_cast<Instruction>(v);

            // if operand 1 name is source, some value or variable is stored to source, add source to updatedTaintVars
            // check operand 0 is taint or not, if so, when store value from operand 0 to operand 1, operand 1 is taint
            if (I.getOperand(1)->getName() == expectedName || isTaint(I.getOperand(0), updatedTaintVars, expectedName)) {
                updatedTaintVars.insert(var->getName().str());	// Gen: operand 2, var is added to updatedTaintVars
            } else {
                updatedTaintVars.erase(var->getName().str());
            }

        // when instruction is allocated
        } else if (isa<AllocaInst>(I)) {

            // if source is allocated, include in taint set and start tracing
            if (I.getName() == expectedName) {
                updatedTaintVars.insert(I.getName().str());	// Gen
            } else {
                updatedTaintVars.erase(I.getName().str());
            }

        // when instruction is load
        } else if (isa<LoadInst>(I)) {

            // use function getInstructionLabel to obtain the label 
            std::string name = getInstructionLabel(&I);

            // if the label is loaded with taint variable, the lebal is also taint
            if (isTaint(I.getOperand(0), updatedTaintVars, expectedName)) {
                updatedTaintVars.insert(name);	// Gen
            } else {
                updatedTaintVars.erase(name);	
            }

        // when instruction is binary operator
        } else if (isa<llvm::BinaryOperator>(I)) {

        }
    }
    return updatedTaintVars;
}


std::set<std::string> intersect_sets(std::set<std::string> A, std::set<std::string> B) {
	std::set<std::string> result;
	std::set_intersection(A.begin(), A.end(), B.begin(), B.end(), std::inserter(result, result.begin()));
    return result;
}


std::set<std::string> union_sets(std::set<std::string> A, std::set<std::string> B) {
    A.insert(B.cbegin(), B.cend());
    return A;
}
