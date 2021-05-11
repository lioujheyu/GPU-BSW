#include "driver.hpp"
#include <cmath>
#include <fstream>
#include <iostream>
#include <omp.h>
#include <sstream>
#include <string>

using namespace std;

int
main(int argc, char* argv[])
{
    int deviceCount;
    cudaGetDeviceCount(&deviceCount);
    cudaDeviceProp prop[deviceCount];
    for(int i = 0; i < deviceCount; i++)
        cudaGetDeviceProperties(&prop[i], 0);

    for(int i = 0; i < deviceCount; i++)
    {
        cout << "total Global Memory available on Device: " << i
             << " is:" << prop[i].totalGlobalMem << endl;
    }

    vector<string> G_sequencesA,
        G_sequencesB;

    string   myInLine;
    ifstream ref_file(argv[1]);
    ifstream quer_file(argv[2]);
    unsigned largestA = 0, largestB = 0;

    int totSizeA = 0, totSizeB = 0;
    // if(ref_file.is_open())
    // {
    //     while(getline(ref_file, myInLine))
    //     {
    //         string seq = myInLine.substr(myInLine.find(":") + 1, myInLine.size() - 1);
    //         G_sequencesA.push_back(seq);
    //         totSizeA += seq.size();
    //         if(seq.size() > largestA)
    //         {
    //             largestA = seq.size();
    //         }
    //     }
    // }

    // if(quer_file.is_open())
    // {
    //     while(getline(quer_file, myInLine))
    //     {
    //         string seq = myInLine.substr(myInLine.find(":") + 1, myInLine.size() - 1);
    //         G_sequencesB.push_back(seq);
    //         totSizeB += seq.size();
    //         if(seq.size() > largestB)
    //         {
    //             largestB = seq.size();
    //         }
    //     }
    // }
    if(ref_file.is_open())
    {
        while(getline(ref_file, myInLine))
        {
          //  string seq = myInLine.substr(myInLine.find(":") + 1, myInLine.size() - 1);
            if(myInLine[0] == '>'){
              continue;
            }else{
              string seq = myInLine;
              G_sequencesA.push_back(seq);
              //std::cout<<"ref:"<<G_sequencesA.size()<<std::endl;
              totSizeA += seq.size();
              if(seq.size() > largestA)
              {
                  largestA = seq.size();
              }

            }

        }
        ref_file.close();
    }

    if(quer_file.is_open())
    {
        while(getline(quer_file, myInLine))
        {
            //string seq = myInLine.substr(myInLine.find(":") + 1, myInLine.size() - 1);
            if(myInLine[0] == '>'){
              continue;
            }else{
              string seq = myInLine;
              G_sequencesB.push_back(seq);
            //  std::cout<<"que:"<<G_sequencesB.size()<<std::endl;
              totSizeB += seq.size();
              if(seq.size() > largestB)
              {
                  largestB = seq.size();
              }

            }

        }
        quer_file.close();
    }

    short* g_alAbeg;
    short* g_alBbeg;
    short* g_alAend;
    short* g_alBend;
    short* g_top_scores;

    callAlignKernel(G_sequencesB, G_sequencesA, largestB, largestA, G_sequencesA.size(),
                    &g_alAbeg, &g_alBbeg, &g_alAend, &g_alBend, &g_top_scores, argv[3]);

    // cout <<"start ref:"<<g_alAbeg[0]<<" end ref:"<<g_alAend[0]<<endl;
    // cout <<"start que:"<<g_alBbeg[0]<<" end que:"<<g_alBend[0]<<endl;
    // cout <<"start ref:"<<g_alAbeg[1]<<" end ref:"<<g_alAend[1]<<endl;
    // cout <<"start que:"<<g_alBbeg[1]<<" end que:"<<g_alBend[1]<<endl;
    verificationTest(argv[3], g_alAbeg, g_alBbeg, g_alAend, g_alBend);

    ofstream results_file("result.out");


  for(int k = 0; k < G_sequencesA.size(); k++){
        results_file<<g_top_scores[k]<<"\t"
                    <<g_alAbeg[k]<<"\t"
                    <<g_alAend[k]<<"\t"
                    <<g_alBbeg[k]<<"\t"
                    <<g_alBend[k]<<"\t"
                    <<endl;
  }
  results_file.flush();
  results_file.close();

    return 0;
}
