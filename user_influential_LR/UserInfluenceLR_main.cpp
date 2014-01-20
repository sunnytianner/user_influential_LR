//
//  main.cpp
//  user_influential_LR
//
//  Created by Eagle on 13-12-3.
//  Copyright (c) 2013年 Eagle. All rights reserved.
//

#include <iostream>
#include "UserInfluenceLR.h"

int main(int argc, const char * argv[])
{
    UserInfluenceLR lr("/Users/tian19880220/workspace/user_influential_LR/data/data_format_back.txt");
    lr.initGraph();
    lr.LR(1000,0.02,0.02,0.01,0.01);
    lr.resultSortedOutput();
}
