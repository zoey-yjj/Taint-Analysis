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


std::set<std::string> intersect_sets(std::set<std::string> A, std::set<std::string> B);

int main(int argc, char **argv) {}


std::set<std::string> intersect_sets(std::set<std::string> A, std::set<std::string> B) {
	std::set<std::string> result;
	std::set_intersection(A.begin(), A.end(), B.begin(), B.end(), std::inserter(result, result.begin()));
    return result;
}
