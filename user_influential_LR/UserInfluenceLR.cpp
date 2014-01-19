//
//  UserInfluenceHITS.cpp
//  user_influential_HITS
//
//  Created by Eagle on 13-10-8.
//  Copyright (c) 2013年 Eagle. All rights reserved.
//

#include "UserInfluenceLR.h"
#include "constants.h"
#include "util.h"
#include <string>
#include <map>
#include <fstream>
#include <iostream>
#include <stdlib.h>
#include <math.h>
#include <algorithm>
#include <functional>

using namespace std;

UserInfluenceLR::UserInfluenceLR(){
    authOutputFileName = "/Users/tian19880220/workspace/user_influential_LR/result/auth_result";
    hubOutputFileName = "/Users/tian19880220/workspace/user_influential_LR/result/hub_result";
    authSortedOutputFileName = "/Users/tian19880220/workspace/user_influential_LR/result/auth_result_sorted";
    hubSortedOutputFileName = "/Users/tian19880220/workspace/user_influential_LR/result/hub_result_sorted";
}

UserInfluenceLR::UserInfluenceLR(string file){
    graphInputFileName = file;
    UserInfluenceLR();
}

void UserInfluenceLR::initGraph(){
    cout << "init the graph......" << endl;
    ifstream inputFile;
    inputFile.open(graphInputFileName.c_str());
    
    while (!inputFile.eof()) {
        string line;
        getline(inputFile,line);
        if (line.size() == 0) {
            continue;
        }
        
        vector<string> parts = Util::split(line, SPACE);
        if (parts.size() < 2) {
            cout<< "the parts is less then 2!"<< endl;
            continue;
        }
        twitter twitter_info;
        vector<string> items = Util::split(parts[0], SIMICOLON);
        string gateway_string = items[1];
        twitter_info.gatewayName = gateway_string;
        twitter_info.gatewayCount = 0;
        
        items = Util::split(parts[1], COMMA);
        
        for (int i = 0; i < items.size(); i++) {
            vector<string> points = Util::split(items[i], ";");
            if (points.size() != 2) {
                cout << "the points is less then 2!" << endl;
                continue;
            }
            string source = points[1];  //the person who is the source of the information
            string target = points[0];  //the person who refer to the source for the information
            
            //init lr with auth=1.0 hubs=1.0
            if (lr.find(source) == lr.end()) {
                TwitterLR object = {source,(double)(rand()%100+1)/100,(double)(rand()%100+1)/100};
                lr.insert(make_pair(source, object));
            }
            if (lr.find(target) == lr.end()) {
                TwitterLR object = {target,(double)(rand()%100+1)/100,(double)(rand()%100+1)/100};
                lr.insert(make_pair(target, object));
            }
            
            //init Graph
            if (source == gateway_string) {
                twitter_info.gatewayCount += 1;
            }
            else{
                if (twitter_info.routerInfo.find(source)==twitter_info.routerInfo.end()) {
                    twitter_info.routerInfo[source] = 0;
                }
                twitter_info.routerInfo[source] += 1;
            }
        }
        
        graph.push_back(twitter_info);
    }
}


void UserInfluenceLR::LR(int step_count, double lambda1, double lambda2, double beta1, double beta2){
    cout << "Start LR algorithm..." << endl;
    
    double p_control = 0,c_control = 0;
    //map<string,TwitterLR> lr_temp;  //暂存每次迭代时的值
    //lr_temp = lr;
    for (int step = 0; step<step_count; step++) {
        c_control = 0;
        cout << "Step " << step+1 << "..." << endl;
        vector<twitter>::iterator twiiterIter;
        
        map<string,TwitterLR>::iterator iter;
        /*for (iter=lr_temp.begin(); iter!=lr_temp.end(); iter++) {
            iter->second.auth = iter->second.hub = 0;
        }*/
        
        //计算auth,使用梯度下降的方法进行迭代。因为每次求偏导数的时候是累加，所以可以拆开，随着twitter的遍历进行累加
        for (twiiterIter = graph.begin(); twiiterIter != graph.end(); twiiterIter++) {//遍历每一个twitter
            string gateway = twiiterIter->gatewayName;
            int gatewayCount = twiiterIter->gatewayCount;
            
            lr[gateway].auth += beta1 * gatewayCount * (1/(1+exp(lr[gateway].auth)));
            map<string,int>::iterator routerIter;
            for (routerIter = twiiterIter->routerInfo.begin(); routerIter != twiiterIter->routerInfo.end(); routerIter++) {
                lr[gateway].auth += beta1 * routerIter->second * (1/(1+exp(lr[gateway].auth + lr[routerIter->first].hub)));
            }
        }
        
        //一次set遍历之后，遍历所有的auth值，减去最小二乘约束项
//        map<string,TwitterLR>::iterator iter;
        for (iter=lr.begin(); iter!=lr.end(); iter++) {
//            iter->second.auth -= beta1 * lambda1 * iter->second.auth;
            c_control += iter->second.auth;
        }
        
        //计算hub，道理和上面计算auth类似
        for (twiiterIter = graph.begin(); twiiterIter != graph.end(); twiiterIter++) {//遍历每一个twitter
            string gateway = twiiterIter->gatewayName;
            
            map<string,int>::iterator routerIter;
            for (routerIter = twiiterIter->routerInfo.begin(); routerIter != twiiterIter->routerInfo.end(); routerIter++) {
                lr[routerIter->first].hub += beta2 * routerIter->second * (1/(1+exp(lr[gateway].auth + lr[routerIter->first].hub)));
            }
        }
        
        //一次set遍历之后，遍历所有的hub值，减去最小二乘约束项
        for (iter=lr.begin(); iter!=lr.end(); iter++) {
//            iter->second.hub -= beta2 * lambda2 * iter->second.hub;
            c_control += iter->second.hub;
        }
        
        if (c_control - p_control <= 0.05) {
            break;
        }
        else{
            p_control = c_control;
            cout<<p_control<<endl;
        }
    }
}

void UserInfluenceLR::resultOutput(){
    cout << "start writing the result files..." << endl;
    ofstream authOutputFile,hubOutputFile;
    authOutputFile.open(authOutputFileName.c_str());
    hubOutputFile.open(hubOutputFileName.c_str());
    
    map<string,TwitterLR>::iterator iter;
    for (iter = lr.begin(); iter!=lr.end(); iter++) {
        authOutputFile << iter->first << "\t" << iter->second.auth << endl;
        hubOutputFile << iter->first << "\t" << iter->second.hub << endl;
    }
    authOutputFile.close();
    hubOutputFile.close();
    
}

void UserInfluenceLR::resultSortedOutput(){
    cout << "start writing the sorted result files..." << endl;
    ofstream authSortedOutputFile,hubSortedOutputFile;
//    authSortedOutputFile.open(authSortedOutputFileName.c_str());
//    hubSortedOutputFile.open(hubSortedOutputFileName.c_str());
    authSortedOutputFile.open("/Users/tian19880220/Desktop/1");
    hubSortedOutputFile.open("/Users/tian19880220/Desktop/2");
    
    vector<TwitterLR> lrVect;
    map<string,TwitterLR>::iterator iter;
    for (iter = lr.begin(); iter!=lr.end(); iter++) {
        lrVect.push_back(iter->second);
    }
    
    sort(lrVect.begin(),lrVect.end(),biggerAuth);
    for (int i=0; i<lrVect.size(); i++) {
        authSortedOutputFile << lrVect[i].name << "\t" << lrVect[i].auth << endl;
    }
    authSortedOutputFile.flush();
    authSortedOutputFile.close();
    
    sort(lrVect.begin(),lrVect.end(),biggerHub);
    for (int i=0; i<lrVect.size(); i++) {
        hubSortedOutputFile << lrVect[i].name << "\t" << lrVect[i].hub << endl;
    }
    hubSortedOutputFile.close();
}
