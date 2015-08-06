/*
    读取正则表达式，然后将正则表达式分组，并生成分组结果
*/

#include "stdinc.h"
#include "nfa.h"
#include "dfa.h"
#include "hybrid_fa.h"
#include "parser.h"
#include "trace.h"
#include <string>
#include <limits.h>
#include <iostream>
#include <strstream>

using namespace std;

//计算当前正则表达式和K个分组的相关性
void countCurrentM(int i, int currentM[]);

//找到最小的相关性的正则表达式
int findMinCurrentM(int *currentM);

int numberOfDigits(int n);

const int MAX_RE_NUM = 200; //正则表达式最大个数
int VERBOSE;
int DEBUG;
int M1[MAX_RE_NUM][MAX_RE_NUM]; //冲突矩阵
struct expansionRate{ //膨胀率
    int num;
    double ER;
}M2[MAX_RE_NUM+1];

string str[MAX_RE_NUM]; //正则表达式
const int K = 6; //分组大小
int regexNum=0; //读入的正则表达式数目
int groupResult[K+1][MAX_DFA_SIZE+1]; //存放分组结果

static struct conf {
	char *regex_file;
	char *in_file;
	char *out_file;
	char *dot_file;
	char *trace_file;
	bool i_mod;
	bool m_mod;
	bool verbose;
	bool debug;
	bool hfa;
} config;

void init_conf(){
	config.regex_file=NULL;
	config.in_file=NULL;
	config.out_file=NULL;
	config.dot_file=NULL;
	config.trace_file=NULL;
	config.i_mod=false;
	config.m_mod=false;
	config.debug=false;
	config.verbose=false;
	config.hfa=false;
}

//正则表达式分组算法
void newGroup() {
    int i, j;
    int stateNum[K+1];
    int currentNum[K+1];
    for(i=1; i<=K; i++){
        stateNum[i] = 0;
        currentNum[i] = 0;
        for(j=0; j<MAX_DFA_SIZE; j++)
            groupResult[i][j] = 0;
    }

    for(i=1; i<=K; i++) { //前K个正则表达式依次放入K个组中
        groupResult[i][1] = M2[i].num;
        groupResult[i][0] = 1;
        countCurrentM(M2[i].num, stateNum);
        cout<<"stateNum "<<i<<" ="<<stateNum[i]<<endl;
    }

    for(i=K+1; i<=regexNum; i++){
        for(j=1; j<=K; j++) {
            int t = numberOfDigits(stateNum[j]);
            currentNum[j] = stateNum[j] - (int)(M2[i].ER * t);
            cout<<"currentNum "<<j<<" = "<<currentNum[j]<<endl;
        }
        int n = findMinCurrentM(currentNum); //找到最小的相关性的正则表达式的分组
        cout<<"n= "<<n<<endl;
        groupResult[n][groupResult[n][0]+1] = i; //将其加入相应的分组中
        groupResult[n][0]++;
        countCurrentM(M2[i].num, stateNum);
    }

}

int numberOfDigits(int n) {
    int result = 1;
    int num = n / 10;
    while(num > 0) {
        result *= 10;
        num = num / 10;
    }
    return result;
}

//正则表达式分组算法
void group() {
    int i, j;
    int currentM[K+1];
    for(i=1; i<=K; i++){
        currentM[i] = 0;
        for(j=0; j<MAX_DFA_SIZE; j++)
            groupResult[i][j] = 0;
    }

    for(i=1; i<=K; i++) { //前K个正则表达式依次放入K个组中
        groupResult[i][1] = M2[i].num;
        groupResult[i][0] = 1;
    }

    for(i=K+1; i<=regexNum; i++){
        int num = M2[i].num;
        countCurrentM(num, currentM); //计算当前正则表达式和K个分组的相关性
        int n = findMinCurrentM(currentM); //找到最小的相关性的正则表达式的分组
        groupResult[n][groupResult[n][0]+1] = i; //将其加入相应的分组中
        groupResult[n][0]++;
    }
}

void countCurrentM(int num, int currentM[]){
    NFA *nfa=NULL;  	// NFA
	DFA *dfa=NULL;		// DFA
    int i, j;
    for(i=1; i<=K; i++){
        regex_parser *parse=new regex_parser(config.i_mod,config.m_mod);
        string s;
        for(j=1; j<=groupResult[i][0]; j++){
            s = s + "\n" + str[groupResult[i][j]];
        }
		nfa = parse->parse((s+"\n"+str[num]).data());
		nfa->remove_epsilon();
	 	nfa->reduce();
		dfa=nfa->nfa2dfa();
		currentM[i] = dfa->minimize();
		//cout<<"minDFA= "<<currentM[i];
		delete parse;
    }
}

int findMinCurrentM(int *currentM){
    int Min = INT_MAX;
    int i, num=-1;
    for(i=1; i<=K; i++){
        cout<<"currentM"<<i<<"= "<<currentM[i]<<endl;
        if(currentM[i] < Min){
            num = i;
            Min = currentM[i];
        }
    }
    return num;
}

void outputGroupResult(){
    int i, j;

    for(i=1; i<=K; i++){
        string fileName = "C:\\TestData\\groupResult\\group";
        strstream ss;
        string s;
        ss << i;
        ss >> s;
        fileName = fileName + s +".txt";
        cout<<fileName<<endl;
        const char* filename = fileName.data();
        FILE *outFile = fopen(filename, "w");
        for(j=1; j<=groupResult[i][0]; j++){
            fprintf(outFile, "%s", str[groupResult[i][j]].data());
            fprintf(outFile, "\n");
        }
        fclose(outFile);
    }
}

//读取正则表达式
int readRegex(char *fileName){

    int i=0;
    int j=0;
    if (fileName!=NULL){
		FILE *regex_file=fopen(config.regex_file,"r");
		fprintf(stderr,"\nParsing the regular expression file %s ...\n",config.regex_file);

        rewind(regex_file);
        char *re=allocate_char_array(1000);
        int from = 1;
        int to = -1;
        unsigned int c=fgetc(regex_file);
        while(c!=EOF){
            if (c=='\n' || c=='\r'){  //(if meet the character is newline or blank line) and the register is not null
                if(i!=0){
                    re[i]='\0'; //insert a EOS to the register  EOS==end of string
                    if (re[0]!='#'){//if the first character is not "#"
                        j++;//row number+1
                        if (j>=from && (to==-1 || j<=to)){
                            str[j] = re;
                        }
                    }
                    /*this is deal with the comment_line*/
                    i=0;
                    free(re);
                    re=allocate_char_array(1000);
                }
            }else{
                re[i++]=c;//not newline character,c store in re[i++]
            }
            c=fgetc(regex_file);//read new character c from file
        } //end while


        //if the file end is not with the '\n'or'\r' and the re is not null
        if(i!=0){
            re[i]='\0';
            if (re[0]!= '#'){
                j++;
                if (j>=from && (to==-1 || j<=to)){
                    str[j] = re;
                }
            }
            free(re);
        }
        fclose(regex_file);
    }
    return j;
}

int compare(const void *p1, const void *p2){
    struct expansionRate *a = (struct expansionRate *)p1;
    struct expansionRate *b = (struct expansionRate *)p2;
    int result = 0;
    if(a->ER > b->ER)
        return -1;
    else if(a->ER < b->ER)
        return 1;
    else
        return 0;
}

void sortM2(int num) {
    qsort(M2, num, sizeof(M2[0]), compare);
}


int main()
{
    int i=0;
	init_conf();
	VERBOSE=config.verbose;
	DEBUG = 1;
	if (DEBUG) VERBOSE=1;

	config.regex_file = "C:\\TestData\\regex0.txt";
	config.dot_file = "C:\\TestData\\dfile0.txt";
	config.out_file = "C:\\TestData\\ofile0.txt";

	/* FA declaration */
	NFA *nfa=NULL;  	// NFA
	DFA *dfa=NULL;		// DFA
	dfa_set *dfas=NULL; // set of DFAs, in case a single DFA for all RegEx in the set is not possible
	HybridFA *hfa=NULL; // Hybrid-FA

    regexNum = readRegex(config.regex_file); //从文件中读取正则表达式，regexNum是表达式的数目

    cout<<"regexNum= "<<regexNum<<endl;
    for(i=1; i<=regexNum; i++){
        cout<<str[i]<<endl;
    }


    int n;
    int d[MAX_RE_NUM];
    for(int num=1; num<=regexNum; num++)
    {
        regex_parser *parse=new regex_parser(config.i_mod,config.m_mod);
		nfa = parse->parse(str[num].data());
		n = nfa->remove_epsilon();
	 	nfa->reduce();
		dfa=nfa->nfa2dfa();
		d[num] = dfa->minimize();
		M2[num].ER = (d[num]-double(n))/n;//算M2矩阵
		M2[num].num = num;
		delete parse;
    }

    M2[0].ER = INT_MAX;
    sortM2(regexNum+1); //对M2从小到大进行排序，而M2中的num记录正则表达式在str中的位置
/*
    int dt;
    for(int num1=1; num1<=regexNum; num1++)
     for(int num2=1; num2<=regexNum; num2++)
    {
        regex_parser *parse=new regex_parser(config.i_mod,config.m_mod);
		nfa = parse->parse((str[num1]+"\n"+str[num2]).data());
		nfa->remove_epsilon();
	 	nfa->reduce();
		dfa=nfa->nfa2dfa();
		dt = dfa->minimize();
		M1[num1][num2] = dt-d[num1]-d[num2];//算M1矩阵
		M1[num2][num1] = dt-d[num1]-d[num2];
		delete parse;
    }
*/
/*
    //整体计算x
    FILE *regex_file=fopen(config.regex_file,"r");
		fprintf(stderr,"\nParsing the regular expression file %s ...\n",config.regex_file);
    regex_parser *parse=new regex_parser(config.i_mod,config.m_mod);
    nfa = parse->parsef(regex_file);
    nfa->remove_epsilon();
    nfa->reduce();
    dfa=nfa->nfa2dfa();
    int allNum = dfa->minimize();
    cout<<"allNum= "<<allNum<<endl;
    delete parse;
    fclose(regex_file);
*/
/*
	if (dfa!=NULL && config.dot_file!=NULL){
		FILE *dot_file=fopen(config.dot_file,"w");
		fprintf(stderr,"\nExporting to DOT file %s ...\n",config.dot_file);
		char string[100];
		cout<<"dot_file"<<endl;
		if (config.regex_file!=NULL){
            sprintf(string,"source: %s",config.regex_file);
		}
		else sprintf(string,"source: %s",config.in_file);
		dfa->to_dot(dot_file, string);//计算分组数据
		fclose(dot_file);
	}


    FILE *out_file1=fopen("C:\\TestData\\m1file.txt","w");
    fprintf(out_file1, "%d", regexNum);
    fprintf(out_file1, "\n");
    //fclose(out_file1);
    for(int num1=1; num1<=regexNum; num1++)
    {
        for(int num2=1; num2<=regexNum; num2++)
        {
            fprintf(out_file1, "%d", M1[num1][num2]);
            fprintf(out_file1, " ");
        }
        fprintf(out_file1, "\n");
    }
    fclose(out_file1);
*/
/*
    FILE *out_file2=fopen("C:\\TestData\\m2file.txt","w");
    for(int num=1; num<=regexNum; num++)
    {
        fprintf(out_file2, "%.4f", M2[num].ER);
        fprintf(out_file2, " ");
    }
    fclose(out_file2);
*/

    //newGroup();
    group();
    outputGroupResult();

    for(i=1; i<=K; i++){
        cout<<"--"<<groupResult[i][0]<<"--"<<endl;
        cout<<endl;
        int j;
        string fileName = "C:\\TestData\\groupResult\\group";
        strstream ss;
        string s;
        ss << i;
        ss >> s;
        fileName = fileName + s +".txt";
        const char* filename = fileName.data();
        FILE *regex_file=fopen(filename,"r");
            fprintf(stderr,"\nParsing the regular expression file %s ...\n",config.regex_file);
        regex_parser *parse=new regex_parser(config.i_mod,config.m_mod);
        nfa = parse->parsef(regex_file);
        nfa->remove_epsilon();
        nfa->reduce();
        dfa=nfa->nfa2dfa();
        int allNum = dfa->minimize();
        cout<<"group "<<i<<" ="<<allNum<<endl;
        delete parse;
        fclose(regex_file);
    }

	if (nfa!=NULL) delete nfa;
	if (dfa!=NULL) delete dfa;
	if (dfas!=NULL){
		FOREACH_DFASET(dfas,it) delete (*it);
		delete dfas;
	}
	if (hfa!=NULL) delete hfa;

	return 0;
}
