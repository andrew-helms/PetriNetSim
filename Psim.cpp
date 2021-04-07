//On my honor, I have neither given nor received any unauthorized aid on this assignment

#include <vector>
#include <deque>
#include <iostream>
#include <fstream>
#include "stdio.h"
#include <string.h>
#include <string>
#include <sstream>

struct Instruction
{
public:
    unsigned char opCode;
    unsigned char destReg;
    unsigned char srcReg1;
    unsigned char srcReg2;
    
    Instruction(const char* opCodeName, const char* destRegName, const char* srcReg1Name, const char* srcReg2Name)
    {
        if (strcmp(opCodeName, "ADD") == 0)
            opCode = 0;
        else if (strcmp(opCodeName, "SUB") == 0)
            opCode = 1;
        else if (strcmp(opCodeName, "AND") == 0)
            opCode = 2;
        else if (strcmp(opCodeName, "OR") == 0)
            opCode = 3;
        else if (strcmp(opCodeName, "LD") == 0)
            opCode = 4;
            
        destReg = destRegName[1] - '0';
        srcReg1 = srcReg1Name[1] - '0';
        srcReg2 = srcReg2Name[1] - '0';
    }
    
    Instruction() : opCode(0), destReg(0), srcReg1(0), srcReg2(0) {}
};

struct Register
{
public:
    unsigned char addr;
    unsigned char val;
    
    Register(unsigned char address, unsigned char value) : addr(address), val(value) {}
    Register() : addr(0), val(0) {}
};

bool Update(std::deque<Instruction*>& INM, std::deque<Instruction*>& INB, std::deque<Instruction*>& AIB, std::deque<Instruction*>& LIB, std::deque<Register*>& ADB, std::deque<Register*>& REB, std::vector<Register*>& RGF,
std::vector<Register*>& DAM);
std::string Print(int step, std::deque<Instruction*>& INM, std::deque<Instruction*>& INB, std::deque<Instruction*>& AIB, std::deque<Instruction*>& LIB, std::deque<Register*>& ADB, std::deque<Register*>& REB,
std::vector<Register*>& RGF, std::vector<Register*>& DAM);

int main()
{
    /* INITIALIZE VALUES FROM FILES */
    
    std::ifstream file;
    
    std::deque<Instruction*> INM;  
    
    file.open("instructions.txt");
      
    std::string instruct;
    while (std::getline(file, instruct))
    {
        int firstComma = instruct.find(',');        
        INM.push_back(new Instruction(instruct.substr(1, firstComma - 1).c_str(), instruct.substr(firstComma + 1, 2).c_str(), instruct.substr(firstComma + 4, 2).c_str(), instruct.substr(firstComma + 7, 2).c_str()));
    }
    
    file.close();
    
    std::vector<Register*> RGF;
    
    file.open("registers.txt");
    
    std::string reg;
    for (int i = 0; i < 8 && std::getline(file, reg); i++)
    {
        int endBracket = reg.find('>');
        RGF.push_back(new Register(i, std::stoi(reg.substr(4, endBracket - 4))));
    }
    
    file.close();
    
    std::vector<Register*> DAM;
    
    file.open("datamemory.txt");
    
    std::string data;
    for (int i = 0; i < 8 && std::getline(file, data); i++)
    {
        int endBracket = data.find('>');
        DAM.push_back(new Register(i, std::stoi(data.substr(3, endBracket - 3))));
    }
    
    /* INITIALIZE EMPTY DATA STRUCTURES */
    
    std::deque<Instruction*> INB;
    std::deque<Instruction*> AIB;
    std::deque<Instruction*> LIB;
    std::deque<Register*> ADB;
    std::deque<Register*> REB;
    
    std::string output = "";
    
    int step = 0;
    do{
        output += Print(step++, INM, INB, AIB, LIB, ADB, REB, RGF, DAM);
    } while (Update(INM, INB, AIB, LIB, ADB, REB, RGF, DAM));
    
    std::ofstream outputFile;
    outputFile.open("simulation.txt");
    
    outputFile << output.substr(2); //to avoid the first two line breaks
    
    outputFile.close();
    
    return 0;
}

bool Update(std::deque<Instruction*>& INM, std::deque<Instruction*>& INB, std::deque<Instruction*>& AIB, std::deque<Instruction*>& LIB, std::deque<Register*>& ADB, std::deque<Register*>& REB, std::vector<Register*>& RGF,
std::vector<Register*>& DAM)
{
    bool validStep = false;
    
    /* READ / DECODE */
    Instruction* readOutput = nullptr;
    
    if (!INM.empty())
    {
        validStep = true;
        readOutput = INM.front();
        INM.pop_front();
        
        readOutput->srcReg1 = RGF[(int)readOutput->srcReg1]->val;
        readOutput->srcReg2 = RGF[(int)readOutput->srcReg2]->val;
    }
    
    /* ISSUE1 / ISSUE2 */
    Instruction* issueOutput = nullptr;
    
    if (!INB.empty())
    {
        validStep = true;
        issueOutput = INB.front();
        INB.pop_front();
    }
    
    /* ALU */
    Register* ALUOutput = nullptr;

    if (!AIB.empty())
    {
        validStep = true;
        Instruction* instruct = AIB.front();
        AIB.pop_front();
        
        switch(instruct->opCode)
        {
            case 0:
                ALUOutput = new Register(instruct->destReg, (instruct->srcReg1 + instruct->srcReg2) % 64);
                break;
            case 1:
                ALUOutput = new Register(instruct->destReg, (instruct->srcReg1 - instruct->srcReg2) % 64);
                break;            
            case 2:
                ALUOutput = new Register(instruct->destReg, (instruct->srcReg1 & instruct->srcReg2) % 64);
                break; 
            case 3:
                ALUOutput = new Register(instruct->destReg, (instruct->srcReg1 | instruct->srcReg2) % 64);
                break; 
        }
        
        delete instruct;
    }
    
    /* ADDR */
    Register* ADDROutput = nullptr;
    
    if (!LIB.empty())
    {
        validStep = true;
        Instruction* instruct = LIB.front();
        LIB.pop_front();
        
        ADDROutput = new Register(instruct->destReg, (instruct->srcReg1 + instruct->srcReg2) % 8);
        
        delete instruct;
    }
    
    /* LOAD */
    Register* loadOutput = nullptr;
    
    if (!ADB.empty())
    {
        validStep = true;
        loadOutput = ADB.front();
        ADB.pop_front();
        
        loadOutput->val = DAM[loadOutput->val]->val;
    }
    
    /* WRITE */
    Register* writeOutput = nullptr;
    
    if (!REB.empty())
    {
        validStep = true;
        writeOutput = REB.front();
        REB.pop_front();
    }
    
    if (readOutput != nullptr)
        INB.push_back(readOutput);
        
    if (issueOutput != nullptr)
        if (issueOutput->opCode == 4)
            LIB.push_back(issueOutput);
        else
            AIB.push_back(issueOutput);
        
    if (ADDROutput != nullptr)
        ADB.push_back(ADDROutput);
        
    if (loadOutput != nullptr)
        REB.push_back(loadOutput);
            
    if (ALUOutput != nullptr)
        REB.push_back(ALUOutput);
        
    if(writeOutput != nullptr)
    {
        Register* temp = RGF[writeOutput->addr];
        RGF[writeOutput->addr] = writeOutput;
        delete temp;
    }
    
    return validStep;
}

std::string Print(int step, std::deque<Instruction*>& INM, std::deque<Instruction*>& INB, std::deque<Instruction*>& AIB, std::deque<Instruction*>& LIB, std::deque<Register*>& ADB, std::deque<Register*>& REB,
std::vector<Register*>& RGF, std::vector<Register*>& DAM)
{
    std::stringstream output;
    
    output << std::endl << std::endl;
    
    output << "STEP " << step << ":" << std::endl << "INM:";
    
    for (std::deque<Instruction*>::iterator iter = INM.begin(); iter != INM.end(); iter++)
    {
        output << "<";
        
        switch ((*iter)->opCode)
        {
            case 0:
                output << "ADD";
                break;
            case 1:
                output << "SUB";
                break;
            case 2:
                output << "AND";
                break;
            case 3:
                output << "OR";
                break;
            case 4:
                output << "LD";
                break;
        }
        
        output << ",R" << (int)(*iter)->destReg << ",R" << (int)(*iter)->srcReg1 << ",R" << (int)(*iter)->srcReg2 << ">";
        
        if (iter+1 != INM.end())
            output << ",";
    }
    
    output << std::endl << "INB:";
    
    for (std::deque<Instruction*>::iterator iter = INB.begin(); iter != INB.end(); iter++)
    {
        output << "<";
        
        switch ((*iter)->opCode)
        {
            case 0:
                output << "ADD";
                break;
            case 1:
                output << "SUB";
                break;
            case 2:
                output << "AND";
                break;
            case 3:
                output << "OR";
                break;
            case 4:
                output << "LD";
                break;
        }
        
        output << ",R" << (int)(*iter)->destReg << "," << (int)(*iter)->srcReg1 << "," << (int)(*iter)->srcReg2 << ">";
        
        if (iter+1 != INB.end())
            output << ",";
    }
    
    output << std::endl << "AIB:";
    
    for (std::deque<Instruction*>::iterator iter = AIB.begin(); iter != AIB.end(); iter++)
    {
        output << "<";
        
        switch ((*iter)->opCode)
        {
            case 0:
                output << "ADD";
                break;
            case 1:
                output << "SUB";
                break;
            case 2:
                output << "AND";
                break;
            case 3:
                output << "OR";
                break;
        }
        
        output << ",R" << (int)(*iter)->destReg << "," << (int)(*iter)->srcReg1 << "," << (int)(*iter)->srcReg2 << ">";
        
        if (iter+1 != AIB.end())
            output << ",";
    }
    
    output << std::endl << "LIB:";

    for (std::deque<Instruction*>::iterator iter = LIB.begin(); iter != LIB.end(); iter++)
    {
        output << "<LD";
        
        output << ",R" << (int)(*iter)->destReg << "," << (int)(*iter)->srcReg1 << "," << (int)(*iter)->srcReg2 << ">";
        
        if (iter+1 != LIB.end())
            output << ",";
    }
    
    output << std::endl << "ADB:";
    
    for (std::deque<Register*>::iterator iter = ADB.begin(); iter != ADB.end(); iter++)
    {
        output << "<R" << (int)(*iter)->addr << "," << (int)(*iter)->val << ">";
        
        if (iter+1 != ADB.end())
            output << ",";
    }
    
    output << std::endl << "REB:";
    
    for (std::deque<Register*>::iterator iter = REB.begin(); iter != REB.end(); iter++)
    {
        output << "<R" << (int)(*iter)->addr << "," << (int)(*iter)->val << ">";
        
        if (iter+1 != REB.end())
            output << ",";
    }
    
    output << std::endl << "RGF:";
    
    for (std::vector<Register*>::iterator iter = RGF.begin(); iter != RGF.end(); iter++)
    {
        output << "<R" << (int)(*iter)->addr << "," << (int)(*iter)->val << ">";
        
        if (iter+1 != RGF.end())
            output << ",";
    }
    
    output << std::endl << "DAM:";
    
    for (std::vector<Register*>::iterator iter = DAM.begin(); iter != DAM.end(); iter++)
    {
        output << "<" << (int)(*iter)->addr << "," << (int)(*iter)->val << ">";
        
        if (iter+1 != DAM.end())
            output << ",";
    }
    
    return output.str();
}