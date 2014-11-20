#include <iostream>
#include <string>
#include <vector>
#include <fstream>
#include "hmm.h"

using namespace std;
int main(int argc, char* argv[])
{
    // parsing arguments
    int iterations = atoi(argv[1]);
    char *init_model = argv[2];
    char *trainFileName = argv[3];
    char *outputName = argv[4];

    // load initial hmm model
    HMM hmm_initial;
    loadHMM( &hmm_initial, init_model );

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

    float alpha[trainData[0].size()][hmm_initial.state_num] = {0.0};
    float beta[trainData[0].size()][hmm_initial.state_num] = {0.0};

    // alpha initialization (t==0)
    for(int i=0;i<hmm_initial.state_num;i++)
    {            
        float tmp = hmm_initial.initial[i] * hmm_initial.observation[trainData[0][0]-'A'][i];
        alpha[0][i] = tmp;
    }

    // beta initialization (t==0)
    for(int i=0;i<hmm_initial.state_num;i++)
    {
        beta[trainData[0].size()-1][i] = 1.0;
    }

    // for each training line in train file
    //for(int l=0;l<trainData.size();l++)
    for(int iter=0;iter<iterations;iter++)
    {
        for(int l=0;l<3;l++)
        {
            // forward
            // calculate alpha(t+1)
            for(int t=0;t<trainData[l].size()-1;t++)
            {
                for(int stateNext=0;stateNext<hmm_initial.state_num;stateNext++)
                {
                    for(int stateT=0;stateT<hmm_initial.state_num;stateT++)
                    {
                        alpha[t+1][stateNext] += alpha[t][stateT] * hmm_initial.transition[stateT][stateNext];
                    }
                    alpha[t+1][stateNext] *= hmm_initial.observation[trainData[l][t+1]-'A'][stateNext];
                }
            }
            // dump alpha for debug
            /*        
                      for(int t=0;t<trainData[l].size();t++)
                      {
                      for(int state=0;state<hmm_initial.state_num;state++)
                      cout << alpha[t][state] << " ";
                      cout << endl;
                      }*/


            // backward
            for(int t=trainData[l].size()-1;t>0;t--)
            {
                for(int stateBefore=0;stateBefore<hmm_initial.state_num;stateBefore++)
                {
                    for(int stateT=0;stateT<hmm_initial.state_num;stateT++)
                    {
                        beta[t-1][stateBefore] += beta[t][stateT] * hmm_initial.transition[stateBefore][stateT] * hmm_initial.observation[trainData[l][t]-'A'][stateT];                    
                    }
                }
            }
            /*
            // dump beta for debug
            for(int t=0;t<trainData[l].size();t++)
            {
            for(int state=0;state<hmm_initial.state_num;state++)
            cout << beta[t][state] << " ";
            cout << endl;
            }
            */

            // calcuate gamma
            float gamma[trainData[l].size()][hmm_initial.state_num] = {0.0};


            for(int t=0;t<trainData[l].size();t++)
            {
                float gamma_bot = 0.0;
                for(int i=0;i<hmm_initial.state_num;i++)
                {
                    gamma_bot += alpha[t][i] * beta[t][i];
                }

                for(int i=0;i<hmm_initial.state_num;i++)
                {
                    gamma[t][i] = alpha[t][i] * beta[t][i] / gamma_bot;
                }
            }

            /* 
            // dump gamma for debug
            for(int t=0;t<trainData[l].size();t++)
            {
            for(int state=0;state<hmm_initial.state_num;state++)
            cout << gamma[t][state] << " ";
            cout << endl;
            }*/
            // calculate epsilon
            for(int t=0;t<trainData[l].size()-1;t++)
            {

            }

        }
    }

    return 0;
}
