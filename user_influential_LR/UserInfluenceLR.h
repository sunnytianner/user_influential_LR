//
//  user_influential_LR.h
//  user_influential_LR
//
//  Created by Eagle on 13-12-3.
//  Copyright (c) 2013年 Eagle. All rights reserved.
//
#include <vector>
#include <string>
#include <map>

using namespace std;

//数据结构记录每条twitter的信息，包括gateway的名称，gateway下第一层router的个数，以及所有router被转发的个数
struct twitter{
    string gatewayName; //gateway的名称
    int gatewayCount;   //gateway的第一层router个数
    map<string,int> routerInfo;   //router的信息
};

struct TwitterLR{
    string name;
    double auth;
    double hub;
};

class UserInfluenceLR {
protected:
    map<string,TwitterLR> lr;   //the data structure for all LR values of twitters
    vector<twitter> graph;      //the data structure for the graph data
    string graphInputFileName;  //the source file name of the graph
    string authOutputFileName;  //the file name put the auth values:userName\tauthValue
    string hubOutputFileName;   //the file name put the auth values:userName\thubValue
    string authSortedOutputFileName;  //the sorted file name put the auth values:userName\tauthValue
    string hubSortedOutputFileName;   //the sorted file name put the auth values:userName\thubValue
public:
    UserInfluenceLR(string file);
    UserInfluenceLR();
    
    void initGraph();
    void LR(int step_count, double lambda1, double lambda2, double beta1, double beta2);
    void resultOutput();
    static bool biggerAuth(const TwitterLR & t1, const TwitterLR & t2){return t1.auth > t2.auth;}
    static bool biggerHub(const TwitterLR & t1, const TwitterLR & t2){return t1.hub > t2.hub;}
    void resultSortedOutput();
};