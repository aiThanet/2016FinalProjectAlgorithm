#include "student.h"
#include <stdio.h>
#include <iostream>
#include <queue>
#include <vector>
#include <stdlib.h>
#include <map>

using namespace std;

vector <int> table64[100];
bool isGen = false;

int pow(int a,int b){
    int result = 1;
    for(int i=1;i<=b;i++) result =a*result;
    return result;
}

int calLowerB(int n){
    int ans=1;
    int LowerB=0;
    if(n<=9) return 1;
    while(ans<=n){
        ans=ans*9;
        LowerB++;
    }
    return LowerB;
}

int sum_Vec(vector<int> v){
    int ans = 1;
    for(auto it = v.begin();it!=v.end();it++) ans*=*it;
    return ans;
}

struct comparator{bool operator()(pair<int,vector<int>> i,pair<int,vector<int>> j){return i.first > j.first;}};

// gen 25 < N <= 32 table
void gen_32Fac(int n,int pos){
    int x[5];
    for(int i=1;i<=5;i++) x[i-1] = (n%pow(10,i))/(pow(10,i-1));
    for(int i=0;i<5;i++){
        int ret = i*2;
        int ret2 = ret+1;
        if(x[i]==0) continue;
        else if(x[i]<=4 && x[i]>0){
            for(int j=1;j<=x[i];j++) send(pos*10+ret);
        }
        else if(x[i]<=8 && x[i]>4){
            for(int j=5;j<=x[i];j++) send(pos*10+ret2);
        }
        else{
            send(pos*10+ret);
            send(pos*10+ret2);
        }
    }
}

vector<int> gen_LessFac(int n){
    priority_queue<pair<int,vector<int>>,vector<pair<int,vector<int>>>,comparator> minHeap;
    int bird_min = 1000000;
    vector<int> x_min;
    minHeap.push(make_pair(calLowerB(n),x_min));
    while(!minHeap.empty()){
        int LB = minHeap.top().first;
        vector<int> x = minHeap.top().second;
        int m = x.size();
        minHeap.pop();
        int sum = sum_Vec(x);
        if(sum==n){
            bird_min=LB; x_min=x;
            break;
        }
        if((abs(sum-n)+m<=10) && m!=0 ){
            if(LB<bird_min){
                    bird_min=LB; x_min=x;}
            }
        else{
            int last = 0;
            for(int i=2;i<=9;i++){
                if(m!=0) last = x.back();
                if((sum*i)-n<=10 && last<=i){
                    vector<int> x_new(x);
                    x_new.push_back(i);
                    int remain = (n==(sum*i))? 0 : (calLowerB(n/(sum*i)));
                    minHeap.push(make_pair(m+1+abs((sum*i)-n),x_new));
                }
            }
        }
    }
    return x_min;
}

// gen 50 < N <= 64 table
void gen_64table(){
    queue<vector<int>> q;
    vector<int> init;
    q.push(init);
    int cc = 1;
    while(!q.empty()){
        vector<int> x = q.front();
        if(cc==100) break;
        q.pop();
        vector<int> x0(x);
        x0.push_back(0);
        vector<int> x1(x);
        x1.push_back(1);
        if(x.size()>0 && x.back()==1){
            table64[cc] = x1;
            q.push(x1);
            cc++;
        }
        else{
            table64[cc] = x0;
            table64[cc+1] = x1;
            q.push(x0);
            q.push(x1);
            cc+=2;
        }
    }
}

void gen_64Fac(int pos,int x[]){
    pos = (pos*10)/4;
    vector<int> y[5];
    for(int i=0;i<5;i++) y[i] = table64[x[i]];
    for(int i=0;i<5;i++)
        for(int j=0;j<y[i].size();j++)
            send(pos+y[i][j]+i*2);
}

void encode(int N, int M[]) {
    //this simply send the message as it is
    //this is definitely wrong
    if(N<=25){
        for (int i = 0;i < N;i++) {
            if(M[i]<=9){
                send((i*10+M[i]));
            }
            else{
                vector<int> gen = gen_LessFac(M[i]);
                for(auto it=gen.begin();it!=gen.end();it++) send((i*10)+*it);
                int sum = sum_Vec(gen)-M[i];
                while(sum!=0){
                    if(sum>0) {
                        send(i*10);
                        sum--;
                    }
                    else{
                        send(i*10+1);
                        sum++;
                    }
                }
            }
        }
    }
    else if(N<=50 ){
        for (int i = 0;i < N;i+=2) {
            int x1 = M[i];
            int x2 = (i+1<N)? M[i+1] : 0;
            x2 = 256*x2;
            int base_256 = x2+x1;
            gen_32Fac(base_256,i/2);
        }
    }
    else{
        if(!isGen) gen_64table();
        isGen = true;
        for(int i=0;i<N;i+=4){
            int value1 = M[i];
            int value2 = (i+1<N)? M[i+1]:0 ;
            int value3 = (i+2<N)? M[i+2]:0 ;
            int value4 = (i+3<N)? M[i+3]:0 ;
            unsigned int num = value1+(value2*256)+(value3*pow(256,2))+(value4*pow(256,3));
            int x[5];
            x[0] = num%100;
            x[1] = (num%10000)/100;
            x[2] = (num%1000000)/10000;
            x[3] = (num%100000000)/1000000;
            x[4] = num/100000000;
            gen_64Fac(i,x);
        }
    }
}

//------------------- DO NOT REMOVE NOR EDIT THESE 3 LINES -----------------
//------------------- above is encode, below is decode ---------------------
//--------------------cut here----------------------------------------------
vector <int> table64_2[100];
bool isGen2 = false;
map<pair<int,int>,int> decoder64;
bool isDecode = false;

void gen_64table2(){
    queue<vector<int>> q;
    vector<int> init;
    q.push(init);
    int cc = 1;
    while(!q.empty()){
        vector<int> x = q.front();
        if(cc==100) break;
        q.pop();
        vector<int> x0(x);
        x0.push_back(0);
        vector<int> x1(x);
        x1.push_back(1);
        if(x.size()>0 && x.back()==1){
            table64_2[cc] = x1;
            q.push(x1);
            cc++;
        }
        else{
            table64_2[cc] = x0;
            table64_2[cc+1] = x1;
            q.push(x0);
            q.push(x1);
            cc+=2;
        }
    }
}

void gen_64decoder(){
    for(int i=0;i<100;i++){
        int c1 = 0;
        int c2 = 0;
        for(auto it=table64_2[i].begin();it!=table64_2[i].end();it++){
            if(*it==0) c1++;
            if(*it==1) c2++;
        }
        decoder64[make_pair(c1,c2)]=i;
    }
}

void decode(int N, int L, int X[]) {
    //this simply output the message as it is
    //this is definitely wrong
    if(N<=25){
        int anss[N];
        int countt[N];
        bool mul[N];
        for (int i = 0;i < N;i++) {
            mul[i]=false;
            countt[i]=0;
            anss[i]=0;
        }
        for (int i = 0;i < L;i++){
            int pos = (X[i] /10);
            int value = X[i]%10;
            if(value==0){
                countt[pos]-=1;
            }
            else if(value==1){
                countt[pos]+=1;
            }
            else{
                if(anss[pos]==0) anss[pos]=1;
                mul[pos]=true;
                anss[pos]*=value;
            }
        }
        for (int i = 0;i < N;i++) {
            if(mul[i]){
                anss[i]+=countt[i];
            }
            else{
                if(countt[i]>0) anss[i]=1;
                else anss[i]=0;
            }
        }
        for (int i = 0;i < N;i++) output(anss[i]);
    }
    else if(N<=50){
        vector<int> decoding [(N/2)+1];
        int decoder [(N/2)+1];
        for(int i=0;i<(N/2)+1;i++) decoder[i]=0;
        int ans[N];
        for(int i=0;i<L;i++){
            int pos = (X[i] /10);
            int value = X[i]%10;
            decoding[pos].push_back(value);
        }
        for(int i=0;i<(N/2)+1;i++){
            int c [10];
            for(int j=0;j<10;j++) c[j]=0;
            for(int j=0;j<decoding[i].size();j++) c[decoding[i][j]]++;
            for(int j=0;j<5;j++){
                int jj = j*2;
                if(c[jj]==0 && c[jj+1]==0){
                    continue;
                }
                else if(c[jj]==1 && c[jj+1]==1){
                    decoder[i]+=9*pow(10,j);
                }
                else if(c[jj+1]==0){
                    decoder[i]+=c[jj]*pow(10,j);
                }
                else{
                    decoder[i]+=(4+c[jj+1])*pow(10,j);
                }
            }
        }
        for(int i=0;i<(N/2)+1;i++){
            int v1 = decoder[i]%256;
            ans[i*2] = v1;
            if(i*2+1<N){
                int v2=decoder[i]/256;
                ans[i*2+1] = v2;
            }
        }
        for (int i = 0;i < N;i++) output(ans[i]);
    }
    else{
        int ansss[N];
        if(!isGen2) gen_64table2();
        isGen2 = true;
        if(!isDecode) gen_64decoder();
        isDecode = true;

        vector<int> decoding [(N/4)+1];
        for(int i=0;i<L;i++){
            int pos = (X[i] /10);
            int value = X[i]%10;
            decoding[pos].push_back(value);
        }
        for(int i=0;i<(N/4)+1;i++){
            int c [10];
            for(int j=0;j<10;j++) c[j]=0;
            for(int j=0;j<decoding[i].size();j++) c[decoding[i][j]]++;
            int x1 = decoder64[make_pair(c[0],c[1])];
            int x2 = decoder64[make_pair(c[2],c[3])];
            int x3 = decoder64[make_pair(c[4],c[5])];
            int x4 = decoder64[make_pair(c[6],c[7])];
            int x5 = decoder64[make_pair(c[8],c[9])];
            unsigned int value = x1+(x2*100)+(x3*10000)+(x4*1000000)+(x5*100000000);;
            int v1 = value%256;
            value/=256;
            int v2 = value%256;
            value/=256;
            int v3 = value%256;
            int v4 = value/256;
            ansss[i*4] = v1;
            if((i*4)+1<N) ansss[(i*4)+1] = v2;
            if((i*4)+2<N) ansss[(i*4)+2] = v3;
            if((i*4)+3<N) ansss[(i*4)+3] = v4;
        }
        for (int i = 0;i < N;i++) output(ansss[i]);
    }
}
