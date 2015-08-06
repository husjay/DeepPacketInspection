#include "stdinc.h"
#include "nfa.h"
#include "dfa.h"
#include "hybrid_fa.h"
#include "parser.h"
#include "trace.h"
#include <string>

const int MAX_RE_NUM = 200;
int VERBOSE;
int DEBUG;
int M1[MAX_RE_NUM][MAX_RE_NUM];
double M2[MAX_RE_NUM];

static struct conf{
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



int mainq()
{
    int i=0;
	int j=0;
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

	if (config.regex_file!=NULL){
		FILE *regex_file=fopen(config.regex_file,"r");
		fprintf(stderr,"\nParsing the regular expression file %s ...\n",config.regex_file);


    rewind(regex_file);
	char *re=allocate_char_array(1000);
	int from = 1;
	int to = -1;
	string str[MAX_RE_NUM];
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

    int n;
    int d[MAX_RE_NUM];
    for(int num=1; num<=j; num++)
    {
        regex_parser *parse=new regex_parser(config.i_mod,config.m_mod);
		nfa = parse->parse(str[num].data());
		n = nfa->remove_epsilon();
	 	nfa->reduce();
		dfa=nfa->nfa2dfa();
		d[num] = dfa->minimize();
		M2[num] = (d[num]-double(n))/n;//算M2矩阵
		delete parse;
    }
    int dt;
    for(int num1=1; num1<=j; num1++)
     for(int num2=1; num2<=j; num2++)
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

    //整体计算x
    regex_parser *parse=new regex_parser(config.i_mod,config.m_mod);
    nfa = parse->parsef(regex_file);
    nfa->remove_epsilon();
    nfa->reduce();
    dfa=nfa->nfa2dfa();
    dfa->minimize();
    delete parse;
    fclose(regex_file);

	}



	if (dfa!=NULL && config.dot_file!=NULL){
		FILE *dot_file=fopen(config.dot_file,"w");
		fprintf(stderr,"\nExporting to DOT file %s ...\n",config.dot_file);
		char string[100];
		if (config.regex_file!=NULL) sprintf(string,"source: %s",config.regex_file);
		else sprintf(string,"source: %s",config.in_file);
		dfa->to_dot(dot_file, string);//计算分组数据
		fclose(dot_file);
	}

    FILE *out_file1=fopen("C:\\TestData\\m1file.txt","w");
    fprintf(out_file1, "%d", j);
    fprintf(out_file1, "\n");
    //fclose(out_file1);
    for(int num1=1; num1<=j; num1++)
    {
        for(int num2=1; num2<=j; num2++)
        {
            fprintf(out_file1, "%d", M1[num1][num2]);
            fprintf(out_file1, " ");
        }
        fprintf(out_file1, "\n");
    }
    fclose(out_file1);

    FILE *out_file2=fopen("C:\\TestData\\m2file.txt","w");
    for(int num=1; num<=j; num++)
    {
        fprintf(out_file2, "%.4f", M2[num]);
        fprintf(out_file2, " ");
    }
    fclose(out_file2);


	if (nfa!=NULL) delete nfa;
	if (dfa!=NULL) delete dfa;
	if (dfas!=NULL){
		FOREACH_DFASET(dfas,it) delete (*it);
		delete dfas;
	}
	if (hfa!=NULL) delete hfa;

	return 0;
}

