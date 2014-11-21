#include <iostream>
#include <string>
#include <vector>
#include <fstream>
#include "hmm.h"

using namespace std;
int main(int argc, char* argv[])
{
    // parsing arguments
    char *modelListFname = argv[1];
    char *testFileName = argv[2];
    char *outputName = argv[3];

    vector<string> modelList;

    ifstream modelListF(modelListFname);
    if( modelListF.is_open())
    {
        string buffer;
        while( !modelListF.eof() )
        {
            getline(modelListF, buffer);
            if(buffer.size() > 0)
                modelList.push_back(buffer);
        }
    }

    // load initial hmm model
    HMM hmms[modelList.size()];
//    loadHMM( &hmm_initial, init_model );
    /*
    // load training data
    vector<string> trainData;
    ifstream trainFile(trainFileName);

    if( trainFile.is_open())
    {
        string buffer;
        while( !trainFile.eof() )
        {
            getline(trainFile, buffer);
            if(buffer.size() > 0)
                trainData.push_back(buffer);
        }
    }
    */
    return 0;
}
