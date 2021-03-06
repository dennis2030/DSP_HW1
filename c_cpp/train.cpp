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


    // for each training line in train file
    //for(int l=0;l<trainData.size();l++)
    for(int iter=0;iter<iterations;iter++)
    {
        // variable initialization
        double gamma_sum[hmm_initial.state_num] = {0.0};        
        double gamma_first_sum[hmm_initial.state_num] = {0.0};
        double epsilon[trainData[0].size()-1][hmm_initial.state_num][hmm_initial.state_num] = {0.0};
        double epsilon_sum[hmm_initial.state_num][hmm_initial.state_num] = {0.0};
        double newObserv[hmm_initial.observ_num][hmm_initial.state_num] = {0.0};         
        double gamma[trainData[0].size()][hmm_initial.state_num] = {0.0};
        for(int i=0;i<hmm_initial.observ_num;i++)
        {
            for(int j=0;j<hmm_initial.state_num;j++)
            {
                newObserv[i][j] = 0.0;
                epsilon_sum[i][j] = 0.0;
            }
        }

        
        for(int l=0;l<trainData.size();l++)
        {
            double alpha[trainData[0].size()][hmm_initial.state_num] = {0.0};
            double beta[trainData[0].size()][hmm_initial.state_num] = {0.0};            

            // alpha initialization (t==0)
            for(int i=0;i<hmm_initial.state_num;i++)
            {            
                double tmp = hmm_initial.initial[i] * hmm_initial.observation[trainData[l][0]-'A'][i];
                alpha[0][i] = tmp;
            }
            // beta initialization (t==0)
            for(int i=0;i<hmm_initial.state_num;i++)
            {
                beta[trainData[l].size()-1][i] = 1.0;
            }
            // forward
            // calculate alpha(t+1)
            for(int t=1;t<trainData[l].size();t++)
            {
                for(int j=0;j<hmm_initial.state_num;j++)
                {
                    for(int i=0;i<hmm_initial.state_num;i++)
                    {
                        alpha[t][j] += alpha[t-1][i]*hmm_initial.transition[i][j]*hmm_initial.observation[trainData[l][t]-'A'][j];
                    }
                }
            }

            // dump alpha for debug
            /*
               for(int t=0;t<trainData[l].size();t++)
               {
               for(int state=0;state<hmm_initial.state_num;state++)
               cout << alpha[t][state] << " ";
               cout << endl;
               }
               */

            // backward
            for(int t=trainData[l].size()-2; t>=0 ;t--)
            {
                for(int i=0;i<hmm_initial.state_num;i++)
                {
                    beta[t][i] = 0.0;
                    for(int j=0;j<hmm_initial.state_num;j++)
                        beta[t][i] += hmm_initial.transition[i][j] * hmm_initial.observation[trainData[l][t+1]-'A'][j] * beta[t+1][j];
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

            // calcuate gamma && gamma_sum

            for(int t=0 ; t<trainData[l].size() ;t++ )
            {
                double gamma_bot = 0.0;
                for(int i=0;i<hmm_initial.state_num;i++)
                {
                    gamma_bot += alpha[t][i] * beta[t][i];
                }
                for(int i=0;i<hmm_initial.state_num;i++)
                    gamma[t][i] = alpha[t][i] * beta[t][i] / gamma_bot;
            }

            for(int i=0;i<hmm_initial.state_num;i++)
                gamma_first_sum[i] += gamma[0][i];

            for(int t=0;t<trainData[l].size()-1;t++)
            {
                for(int i=0;i<hmm_initial.state_num;i++)
                    gamma_sum[i] += gamma[t][i];
            }


            for(int i=0;i<hmm_initial.observ_num;i++)
            {
                for(int j=0;j<hmm_initial.state_num;j++)
                {
                    for(int t=0;t<trainData[l].size();t++)
                    {
                        if(trainData[l][t]-'A' == i)
                            newObserv[i][j] += gamma[t][j];
                    }
                }
            }

            // dump gamma for debug
            /*
               for(int t=0;t<trainData[l].size();t++)
               {
               for(int state=0;state<hmm_initial.state_num;state++)
               cout << gamma[t][state] << " ";
               cout << endl;
               }
               */
            // calculate epsilon && epsilon_sum

            for(int t=0;t<trainData[l].size()-1;t++)
            {
                double epsilon_bot = 0.0;
                for(int i=0;i<hmm_initial.state_num;i++)
                {
                    for(int j=0;j<hmm_initial.state_num;j++)
                    {
                        epsilon[t][i][j] = 0.0;
                        double tmp = alpha[t][i] * hmm_initial.transition[i][j] * hmm_initial.observation[trainData[l][t+1]-'A'][j] * beta[t+1][j];
                        epsilon[t][i][j] = tmp;
                        epsilon_bot += tmp;
                    }
                }

                for(int i=0;i<hmm_initial.state_num;i++)
                {
                    for(int j=0;j<hmm_initial.state_num;j++)
                    {
                        epsilon[t][i][j] /= epsilon_bot;                        
                        epsilon_sum[i][j] += epsilon[t][i][j];
                    }
                }
            }


            // dump epsilon for debug
            /*
               for(int t=0;t<trainData[l].size()-1;t++)
               {
               for(int i=0;i<hmm_initial.state_num;i++)
               {
               for(int j=0;j<hmm_initial.state_num;j++)
               {
               cout << epsilon[t][i][j] << " ";
               }
               }
               cout << endl;
               }
               */

        }

        // update parameters

        // update initials
        for(int i=0;i<hmm_initial.state_num;i++)
        {
            hmm_initial.initial[i] = gamma_first_sum[i] / trainData.size();
            //cout << hmm_initial.initial[i] << " ";
        }

        // updata aij
        for(int i=0;i<hmm_initial.state_num;i++)
        {
            for(int j=0;j<hmm_initial.state_num;j++)
            {
                hmm_initial.transition[i][j] = epsilon_sum[i][j] / gamma_sum[i];
            }
        }

        // update observations
        for(int i=0;i<hmm_initial.observ_num;i++)
        {
            for(int j=0;j<hmm_initial.state_num;j++)
            {
                hmm_initial.observation[i][j] = newObserv[i][j] / gamma_sum[j];
            }
        }
    }
    FILE *outputModel = fopen(outputName, "w");

    dumpHMM(outputModel, &hmm_initial);
    return 0;
}
