//
//  main.cpp
//  DictionaryGeneration
//
//  Created by Xinyuan Lai on 2/2/14.
//
//

#include <iostream>
#include <fstream>
#include "Dictionary.h"

int main(int argc, const char * argv[])
{

    // insert code here...
    std::cout << "Hello, World!\n";
    
//    Dictionary * d = new Dictionary("/Users/Shin/Documents/MATLAB/7100/NO/", 1024, 88, 3, 0.05);
//    d->createDictionary("/Users/Shin/Desktop/");
    
    std::fstream outputFile;
    outputFile.open("/Users/Shin/Desktop/W.txt", std::ios::out);
    if (!outputFile.is_open()) {
        std::cout << "Text file open error!\n";
        return -1;
    }
    
    return 0;
}

