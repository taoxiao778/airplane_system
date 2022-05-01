#include <iostream>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <fstream>
#include <windows.h>
#include <math.h>
#include <stack>
#include <queue>
using namespace std;

#define OK 1
#define ERROR 0
#define OVER_FLOW -2
#define MAX_VERTEX_NUM 80
//定义航班信息的结构体
struct Info_Type{
    int flight_id;
    char depature_date[10];
    char domeorintl[10];
    int flight_num;
    int depature_airport;
    int arrival_airport;
    int plane_id;
    int plane_model;
    int fare;
    double depature_time;
    double arrival_time;
};

Info_Type info_list[3000];
int Filein()
{
    char fileName[100]="D:\\codes\\vscode_c\\shujujiegou\\airport_graph\\airplane.csv";
    ifstream in(fileName);//打开文件
    if (!in.is_open())
    {
        //检测文件是否成功打开
        cout<<"Error opening file"<<fileName;
        return 0;
    }
    //文件成功打开!
    char *lines=NULL;
        //开辟了一个字符指针，并初始化为空指针
        //该指针用来保存读取到的每一行
    lines=(char *)malloc(200*sizeof(char));
        //新分配一个长度为200个char的空间，并将其首地址赋给 lines
        //假定每行字符数不超过200
    if(lines==NULL)
    {
        //空间分配失败
        cout<<"malloc failed!"<<endl;
        return 0;
    }
    //空间分配成功!
    char *p1=NULL,*p2=NULL;

    int j=0;
    while (!in.eof())
    {
//        printf("*\n");//每次成功读取一行就输出一个*
        in.getline(lines, 200);//从文件中读取一行
//        cout << lines << endl;
        //下面解析数据，拆开存到一个字符串数组中
        p1=lines;//开始指针
        p2=lines;//结束指针

        char part[11][20]={{'\0'}};
        int i=0;

        while (1)//拆字符串
        {
            if(i==10)
            {
                strcpy(part[10],p1);//最后一部分数据直接全部复制
                break;
            }
            else if(*p2!=',')
                p2++;//p2后移
            else
            {
                strncpy(part[i],p1,p2-p1);//p1-p2之间的字符复制到part[i]
                p2++;
                p1=p2;//移至下一个字符
                i++;
            }
        }
        //将时间转化为距离5/5/2017 00:00的小时数
        char time[20]={'\0'};
        char date[5]={'\0'};
        char hour[5]={'\0'};
        char min[5]={'\0'};
        double time_=0;
        for(int i=6;i<=7;i++)
        {
            time_=0;
            strcpy(time,part[i]);
            strncpy(date,time+2,1);
            strncpy(hour,time+9,strlen(time)-12);
            strncpy(min,time+strlen(time)-2,2);
            time_+=(strtod(date,NULL)-5)*24+strtod(hour,NULL)+strtod(min,NULL)/60;
            if(i==6)
                info_list[j].depature_time=time_;
            else
                info_list[j].arrival_time=time_;
        }
        info_list[j].flight_id=(int)strtod(part[0],NULL);//将part0-9复制到结构体中
        strcpy(info_list[j].depature_date,part[1]);
        strcpy(info_list[j].domeorintl,part[2]);
        info_list[j].flight_num=(int)strtod(part[3],NULL);
        info_list[j].depature_airport=(int)strtod(part[4],NULL);
        info_list[j].arrival_airport=(int)strtod(part[5],NULL);
        info_list[j].plane_id=(int)strtod(part[8],NULL);
        info_list[j].plane_model=(int)strtod(part[9],NULL);
        info_list[j].fare=(int)strtod(part[10],NULL);
        j++;
    }
    in.close();//关闭文件
    return OK;
}

void printinfo(Info_Type e)
{
    cout<<e.flight_id<<"\t"
          <<e.depature_date<<"\t"
            <<e.domeorintl<<"\t"
              <<e.flight_num<<"\t"
                <<e.depature_airport<<"\t"
                  <<e.arrival_airport<<"\t"
                    <<e.depature_time<<"\t"
                      <<e.arrival_time<<"\t"
                        <<e.plane_id<<"\t"
                          <<e.plane_model<<"\t"
                            <<e.fare<<'#'<<endl;
}
//建立邻接表结构体

typedef struct ArcNode
{
    Info_Type data;//存储航班信息
    struct ArcNode *next;
}ArcNode,*Arclist_M;

typedef struct Graph_M
{
    int apt_num[79];
    Arclist_M matrix[79][79];
}Graph_M;

int InitNode(Arclist_M &L)
{
    L=(Arclist_M)malloc(sizeof(ArcNode));
    if(!L) exit(OVER_FLOW);
    L->next=NULL;
    return OK;
}

int ListInsert(Arclist_M &L,Info_Type e)//插入在表头
{
    Arclist_M p=(Arclist_M)malloc(sizeof(ArcNode));
    p->data=e;
    p->next=L->next;
    L->next=p;
    return OK;
}

void Build_MGraph(Graph_M &m)
{
    for(int i=0;i<79;i++)
        m.apt_num[i]=i+1;//机场序号
    for(int i=0;i<79;i++)
    {
        for(int j=0;j<79;j++)
            InitNode(m.matrix[i][j]);
    }
    for(int i=1;i<2347;i++)//遍历所有航班
    {
        ListInsert(m.matrix[info_list[i].depature_airport-1][info_list[i].arrival_airport-1],info_list[i]);
    }
}


typedef struct stackNode
{
    int length;//当前路径长度（机场数量）
    int trace_apt[20];//路径上经过的机场
    int trace_id[20];//路径上的航班号
//    double dep_time;//当前出发时间
    double arv_time;//当前到达时间
    double sumtime;//总时间
    int sumfare;//总费用
    double flysum;//飞行总时间
    double transfersum;//中转总时间
}stackNode;

void min_time(Graph_M m,int dep,int arv)//任意两个机场之间最短飞行时间
{
    double min_t=10000;//当前最短总时间
    int trace[20]={0};//最短时间对应航班id
    int trace_apt[20]={0};//最短时间对应机场
    int length=0;
    //从出发机场出发的所有航班压栈
    stack<stackNode> s;
    for(int i=0;i<79;i++)//遍历行(出度),即dep机场可以直飞的机场
    {
        if(m.matrix[dep-1][i]->next!=NULL)//两个机场连通
        {
            Arclist_M p=m.matrix[dep-1][i]->next;
            while(p!=NULL)
            {
                stackNode tmp;//建立临时结点
                tmp.length=1;//长度为1，只经过一个机场（除了出发机场）
                tmp.trace_apt[0]=i+1;//更新路径
                tmp.trace_id[0]=p->data.flight_id;
                //tmp.dep_time=p->data.dep_time;
                tmp.arv_time=p->data.arrival_time;
                tmp.sumtime=p->data.arrival_time-p->data.depature_time;
                s.push(tmp);//压栈
                p=p->next;
            }
        }
    }
    while(!s.empty())
    {
        stackNode previous;
        previous=s.top();
        s.pop();//弹出栈顶元素
        //剪枝:
        //1.转机后的航班起飞时间大于转机前的航班到达时间
        //2.同一个落点，只记录满足1且到达时间最早的航班
        //3.若当前用时已经大于当前最小总用时，则剪枝
        if(previous.trace_apt[previous.length-1]==arv)//到达终点，更新最短时间
        {
            if(previous.sumtime<min_t)
            {
                min_t=previous.sumtime;//更新最短总时间
                length=previous.length;
                for(int i=0;i<length;i++)//更新最短总时间对应航班id和机场
                {
                    trace[i]=previous.trace_id[i];
                    trace_apt[i]=previous.trace_apt[i];
                }
            }
        }
        else
        {
            for(int i=0;i<79;i++)
            {
                int current_apt=previous.trace_apt[previous.length-1];//路径上最后一个机场为当前的出发机场
                if(m.matrix[current_apt-1][i]->next!=NULL)//两个机场连通
                {
                    // printf("联通\n");
                    //剪枝
                    Arclist_M p=m.matrix[current_apt-1][i]->next;
                    double time=100000;//记录最早到达时间
                    int id=0;//记录最早到达航班
                    while(p!=NULL)//遍历所有路径
                    {
                        if(p->data.depature_time>previous.arv_time)//转机后的航班起飞时间大于转机前的航班到达时间
                        {
                            if(p->data.arrival_time<time)//同一个落点，只记录满足1且到达时间最早的航班
                            {
                                if(previous.sumtime+p->data.arrival_time-previous.arv_time<min_t)//当前用时小于当前最小总用时
                                {
                                    time=p->data.arrival_time;//更新最早到达时间
                                    id=p->data.flight_id;//更新最早到达航班
                                }
                            }
                        } 
                        p=p->next;
                    }
                    if(id!=0)//说明有满足条件的航班，可以入栈
                    {
                        stackNode tmp;
                        tmp.arv_time=time;
                        tmp.sumtime=previous.sumtime+time-previous.arv_time;
                        tmp.length=previous.length+1;
                        for(int j=0;j<previous.length;j++)
                        {
                            tmp.trace_apt[j]=previous.trace_apt[j];
                            tmp.trace_id[j]=previous.trace_id[j];
                        }
                        tmp.trace_apt[previous.length]=i+1;
                        tmp.trace_id[previous.length]=id;
                        s.push(tmp);
                    }
                }
            }
        }
    }
    if(min_t==10000)
        cout<<"这两个机场之间不存在航线"<<endl;
    else 
    {
        cout<<"最短时间:"<<min_t<<endl;
        cout<<"对应航班id:";
        for(int i=0;i<length;i++)  
            cout<<trace[i]<<" ";
        cout<<endl;
        cout<<"途经机场:";
        cout<<dep<<" ";
        for(int i=0;i<length;i++)
            cout<<trace_apt[i]<<" ";
        cout<<endl;
    }
}

bool Model(Info_Type e,int model)
{
    if(model==0||e.plane_model==model)
    {
        return true;
    }
    return false;
}

void ask_flight(Graph_M M,int dep,int arr,int dep_start,int dep_end,int arr_strar,int arr_end,int model)
{
    int cnt=0;

}

int main()
{
    // read_file();
    Filein();
    printinfo(info_list[1]);
    // ALGraph G;
    // creat_ALGraph(G);
    // BFS(G,50);
    Graph_M M;
    Build_MGraph(M);
    
    // create_Graph_M(M);
    // connected(M,1,3);
    // time_connected(M,48,49);
    min_time(M,1,3);
    // printf("哈哈");
    return 0;
}
