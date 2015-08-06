#include "parser.h"

regex_parser::regex_parser(bool i_mod, bool m_mod){
	i_modifier=i_mod;
	m_modifier=m_mod;
	NFA::ignore_case=i_mod;
}

regex_parser::~regex_parser(){;}


NFA *regex_parser::parse(const char *str,int from,int to){//initial from=1 to=-1
	char *re=allocate_char_array(1000);
	int i=0;
	int j=0;
	int ptr = 0;
	char c = str[ptr];

	// NFA
	NFA *nfa=new NFA();
	NFA *non_anchored = nfa->add_epsilon(); // for .* RegEx
	NFA *anchored = nfa->add_epsilon(); // for anchored RegEx (^)

	//parsing the RegEx and putting them in a NFA
	while(c!='\0'){
		if (c=='\n' || c=='\r'){  //(if meet the character is newline or blank line) and the register is not null
			if(i!=0){
				re[i]='\0'; //insert a EOS to the register  EOS==end of string
				if (re[0]!='#'){//if the first character is not "#"
					j++;//row number+1
					if (j>=from && (to==-1 || j<=to)){
						//if (DEBUG) fprintf(stdout,"\n%d) processing regex:: <%s> ...\n",j,re);
						parse_re(nfa, re);
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
		ptr++;//read new character c from file
		c = str[ptr];
	} //end while


	//if the file end is not with the '\n'or'\r' and the re is not null
	if(i!=0){
		re[i]='\0';
		if (re[0]!= '#'){
			j++;
			if (j>=from && (to==-1 || j<=to)){
				//if (DEBUG) fprintf(stdout,"\n%d) processing regex:: <%s> ...\n",j,re);
				parse_re(nfa,re);
			}
		}
		free(re);
	}



	//if (DEBUG) fprintf(stdout, "\nAll RegEx processed\n");

	if (re!=NULL) free(re);

	//handle -m modifier
	if (m_modifier && (!anchored->get_epsilon()->empty() || !anchored->get_transitions()->empty())){
        //
		non_anchored->add_transition('\n',anchored);
		non_anchored->add_transition('\r',anchored);
	}

	//delete non_anchored, if necessary
	if(non_anchored->get_epsilon()->empty() && non_anchored->get_transitions()->empty()){
		nfa->get_epsilon()->remove(non_anchored);
		delete non_anchored;
	}else{
		non_anchored->add_any(non_anchored);
	}

	return nfa->get_first();

}

NFA *regex_parser::parsef(FILE *file, int from, int to){//initial from=1 to=-1
	rewind(file);
	char *re=allocate_char_array(1000);
	int i=0;
	int j=0;
	unsigned int c=fgetc(file);

	// NFA
	NFA *nfa=new NFA();
	NFA *non_anchored = nfa->add_epsilon(); // for .* RegEx
	NFA *anchored = nfa->add_epsilon(); // for anchored RegEx (^)

	//parsing the RegEx and putting them in a NFA
	while(c!=EOF){
		if (c=='\n' || c=='\r'){  //(if meet the character is newline or blank line) and the register is not null
			if(i!=0){
				re[i]='\0'; //insert a EOS to the register  EOS==end of string
				if (re[0]!='#'){//if the first character is not "#"
					j++;//row number+1
					if (j>=from && (to==-1 || j<=to)){
						if (DEBUG) fprintf(stdout,"\n%d) processing regex:: <%s> ...\n",j,re);


						parse_re(nfa, re);
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
		c=fgetc(file);//read new character c from file
	} //end while


	//if the file end is not with the '\n'or'\r' and the re is not null
	if(i!=0){
		re[i]='\0';
		if (re[0]!= '#'){
			j++;
			if (j>=from && (to==-1 || j<=to)){
				if (DEBUG) fprintf(stdout,"\n%d) processing regex:: <%s> ...\n",j,re);
				parse_re(nfa,re);
			}
		}
		free(re);
	}



	if (DEBUG) fprintf(stdout, "\nAll RegEx processed\n");




	if (re!=NULL) free(re);

	//handle -m modifier
	if (m_modifier && (!anchored->get_epsilon()->empty() || !anchored->get_transitions()->empty())){
        //
		non_anchored->add_transition('\n',anchored);
		non_anchored->add_transition('\r',anchored);
	}

	//delete non_anchored, if necessary
	if(non_anchored->get_epsilon()->empty() && non_anchored->get_transitions()->empty()){
		nfa->get_epsilon()->remove(non_anchored);
		delete non_anchored;
	}else{
		non_anchored->add_any(non_anchored);
	}

	return nfa->get_first();

}

unsigned num_regex(FILE *file){
	rewind(file);
	int re_0;
	int i=0;
	int j=0;
	unsigned int c=fgetc(file);

	//parsing the RegEx and putting them in a NFA
	while(c!=EOF){
		if (c=='\n' || c=='\r'){
			if(i!=0){
				if (re_0!='#') j++;
				i=0;
			}
		}else{
			if (i==0){
				re_0=c;
				i++;
			}
		}
		c=fgetc(file);
	} //end while

	if(i!=0){
		if (re_0 != '#') j++;
	}

	return j;
}

dfa_set *regex_parser::parse_to_dfa(FILE *file){
	dfa_set *dfas=new dfa_set();
	list <pair <unsigned,unsigned> > *queue = new list <pair <unsigned,unsigned> >();
	queue->push_back(pair <unsigned,unsigned>(1,num_regex(file)));
	while (!queue->empty()){
		int min=(queue->front()).first;
		int max=(queue->front()).second;
		queue->pop_front();
		NFA *nfa=parsef(file,min,max);
		nfa->remove_epsilon();
		nfa->reduce();
		DFA *dfa=nfa->nfa2dfa();
		delete nfa;
		if (dfa!=NULL){
			if (VERBOSE) printf("DFA created for regex %ld to %ld\n",min,max);
			dfa->minimize();
			dfas->insert(dfa);
		}
		else{
			if (min==max) fatal("Could not create DFA on single Regex");
			queue->push_back(pair<unsigned,unsigned>(min,(min+max)/2));
			queue->push_back(pair<unsigned,unsigned>((min+max)/2+1,max));
		}
	}
	delete queue;
	return dfas;
}


void *regex_parser::parse_re(NFA* nfa, const char *re){
	int ptr=0;
	bool tilde_re=false;
	NFA *non_anchored = *(nfa->get_epsilon()->begin());//for the .*regex
	NFA *anchored = *(++nfa->get_epsilon()->begin());//for the ^regex

	//check whether the text must match at the beginning of the regular expression
	if (re[ptr]==TILDE){//~
		tilde_re=true;
		ptr++;
	}
	//remove the '^'
	//use the below function parse_re()
	NFA *fa=parse_re(re,&ptr,false);
	fa->get_last()->accept();//returns the last state of the current NFA
	                         //sets the current NFA-state as accepting (and increment the rule number)
	                         //
	if (!tilde_re){
		non_anchored->add_epsilon(fa->get_first());
	}else{
		anchored->add_epsilon(fa->get_first());
	}
}



NFA *regex_parser::parse_re(const char *re, int *ptr, bool bracket){
//then the re  has no '\r' '\n' '#.+' '^' ....
	NFA *fa=new NFA();
	NFA *to_link=NULL;
	bool open_b=bracket;
	bool close_b=false;
	while((*ptr)<strlen(re)){
		if(re[(*ptr)]==ESCAPE){//'\'
			int_set *chars=new int_set(CSIZE);
			//process an escape sequence
			//when re[(*ptr)]==\\ use the followed function
			(*ptr)=process_escape(re, (*ptr)+1,chars);//comfirm correctness of the regex
            //if re[(*ptr)]=='*'|'+'|'?'|'{' return ture
            //return ptr is changed it's position, so check it's position and check it's content
			if((*ptr)==strlen(re)||!is_repetition(re[(*ptr)	]))//then the ptr at least+1
			{
			    //when the ptr==\0 or *ptr != * + ? {
				fa=fa->add_transition(chars);//then ptr at the last charactor of register
				                             //and to add_transition to the declared nfa
				//make the transition with the fa state into a pair<usinged,NFA *>
			}else{
			    //the ahead two charactor is '\\'
				to_link=new NFA();//in this situation,initialize the to_link
				to_link=to_link->add_transition(chars);
			}
			delete chars;

		}else if (!is_special(re[(*ptr)]) && ((*ptr)==(strlen(re)-1)||!is_repetition(re[(*ptr)+1]))){
            fa=fa->add_transition(re[(*ptr)++]);
		}else if(!is_special(re[(*ptr)])){
			to_link=new NFA();
			to_link=to_link->add_transition(re[(*ptr)++]);
		}else if (re[(*ptr)]==ANY && ((*ptr)==(strlen(re)-1)||!is_repetition(re[(*ptr)+1]))){
			fa=fa->add_any();
			(*ptr)++;
		}else if(re[(*ptr)]==ANY){
			to_link=new NFA();
			to_link=to_link->add_any();
			(*ptr)++;
		}else if (re[(*ptr)]==STAR){
			(*ptr)++;
			if (close_b)
				return fa->make_rep(0,_INFINITY);
			else{
				to_link=to_link->make_rep(0,_INFINITY);
				fa=fa->link(to_link);
			}
		}else if (re[(*ptr)]==OPT){
			(*ptr)++;
			if (close_b)
				return fa->make_rep(0,1);
			else{
				to_link=to_link->make_rep(0,1);
				fa=fa->link(to_link);
			}
		}else if (re[(*ptr)]==PLUS){
			(*ptr)++;
			if (close_b){
				return fa->make_rep(1,_INFINITY);
			}else{
				to_link=to_link->make_rep(1,_INFINITY);
				fa=fa->link(to_link);
			}
		}else if(re[(*ptr)]==OPEN_QBRACKET){
			if ((*ptr)==(strlen(re)-1))
				fatal("regex_parser:: parse_re: { in last position.");
			else{
				int lb=0; int ub=_INFINITY;
				(*ptr)=process_quantifier(re,(*ptr)+1,&lb,&ub);
				if (close_b)
					return fa->make_rep(lb,ub);
				else{
					to_link=to_link->make_rep(lb,ub);
					fa=fa->link(to_link);
				}
			}
		}else if(re[(*ptr)]==OPEN_SBRACKET){
			if ((*ptr)==(strlen(re)-1))
				fatal("regex_parser:: parse_re: [ in last position.");
			else
				(*ptr)=process_range(&fa,&to_link,re,(*ptr)+1);
		}else if(re[(*ptr)]==OR){
			(*ptr)++;
			fa=fa->make_or(parse_re(re,ptr,false));
		}else if(re[(*ptr)]==OPEN_RBRACKET){
			(*ptr)++;
			fa=fa->get_last()->link(parse_re(re,ptr,true));
		}else if(re[(*ptr)]==CLOSE_RBRACKET){
			if (open_b){
				close_b=true;
				(*ptr)++;
				if ((*ptr)==strlen(re) || !is_repetition(re[(*ptr)]))
					return fa;
			}
			//fatal("parse:: parse_re : close ) without opening it.");
			else{
				return fa;
			}
		}
	}
	return fa->get_first();
}

int regex_parser::process_escape(const char *re, int ptr, int_set *chars){
	if (ptr==strlen(re)){
		return (++ptr);
		fatal("regex_parser:: process_escape: \\ in last position.");
	}


	char c=re[ptr];
	int next;

	if(is_x(c)){

	    //if c =='x' or 'X' then must check the ptr's position
		if(ptr>strlen(re)-3)
			fatal("regex_parser::process_escape: invalid hex escape sequence.");
		else if (!is_hex_digit(re[ptr+1]) || !is_hex_digit(re[ptr+2]))
			fatal("regex_parser::process_escape: invalid hex escape sequence.");
		else{

			char tmp[5];
			tmp[0]='0';tmp[1]=c;tmp[2]=re[ptr+1];tmp[3]=re[ptr+2]; tmp[4]='\0';
			sscanf(tmp,"0x%x", &next);
			chars->insert(next);
			ptr=ptr+3;
		}

	}else if (is_oct_digit(c)){
		if(ptr>strlen(re)-3)
			{next=escaped(c);ptr++;chars->insert(next);} //normal escape sequence
		else if (!is_oct_digit(re[ptr+1]) || !is_oct_digit(re[ptr+2]))
			{next=escaped(c);ptr++;chars->insert(next);} //normal escape sequence
		else{
			//really an octal sequence!
			char tmp[5];
			tmp[0]='0';tmp[1]=c;tmp[2]=re[ptr+1];tmp[3]=re[ptr+2]; tmp[4]='\0';
			sscanf(tmp,"0%o", &next);
			chars->insert(next);
			ptr=ptr+3;
		}

	}else if(c=='s'){
		chars->insert('\t');
		chars->insert('\n');
		chars->insert('\r');
		chars->insert('\x0C');
		chars->insert('\x20');
		ptr++;
	}else if(c=='S'){
		chars->insert('\t');
		chars->insert('\n');
		chars->insert('\r');
		chars->insert('\x0C');
		chars->insert('\x20');
		chars->negate();
		ptr++;
	}else if(c=='d'){
		chars->insert('0');chars->insert('1');chars->insert('2');
		chars->insert('3');chars->insert('4');chars->insert('5');
		chars->insert('6');chars->insert('7');chars->insert('8');
		chars->insert('9');
		ptr++;
	}else if(c=='D'){
		chars->insert('0');chars->insert('1');chars->insert('2');
		chars->insert('3');chars->insert('4');chars->insert('5');
		chars->insert('6');chars->insert('7');chars->insert('8');
		chars->insert('9');
		chars->negate();
		ptr++;
	}else if(c=='w'){
		chars->insert('_');
		chars->insert('0');chars->insert('1');chars->insert('2');
		chars->insert('3');chars->insert('4');chars->insert('5');
		chars->insert('6');chars->insert('7');chars->insert('8');
		chars->insert('9');
		chars->insert('a');chars->insert('b');chars->insert('c');
		chars->insert('d');chars->insert('e');chars->insert('f');
		chars->insert('g');chars->insert('h');chars->insert('i');
		chars->insert('j');chars->insert('k');chars->insert('l');
		chars->insert('m');chars->insert('n');chars->insert('o');
		chars->insert('p');chars->insert('q');chars->insert('r');
		chars->insert('s');chars->insert('t');chars->insert('u');
		chars->insert('v');chars->insert('w');chars->insert('x');
		chars->insert('y');chars->insert('z');
		chars->insert('A');chars->insert('B');chars->insert('C');
		chars->insert('D');chars->insert('E');chars->insert('F');
		chars->insert('G');chars->insert('H');chars->insert('I');
		chars->insert('J');chars->insert('K');chars->insert('L');
		chars->insert('M');chars->insert('N');chars->insert('O');
		chars->insert('P');chars->insert('Q');chars->insert('R');
		chars->insert('S');chars->insert('T');chars->insert('U');
		chars->insert('V');chars->insert('W');chars->insert('X');
		chars->insert('Y');chars->insert('Z');
		ptr++;
	}else if(c=='W'){
		chars->insert('_');
		chars->insert('0');chars->insert('1');chars->insert('2');
		chars->insert('3');chars->insert('4');chars->insert('5');
		chars->insert('6');chars->insert('7');chars->insert('8');
		chars->insert('9');
		chars->insert('a');chars->insert('b');chars->insert('c');
		chars->insert('d');chars->insert('e');chars->insert('f');
		chars->insert('g');chars->insert('h');chars->insert('i');
		chars->insert('j');chars->insert('k');chars->insert('l');
		chars->insert('m');chars->insert('n');chars->insert('o');
		chars->insert('p');chars->insert('q');chars->insert('r');
		chars->insert('s');chars->insert('t');chars->insert('u');
		chars->insert('v');chars->insert('w');chars->insert('x');
		chars->insert('y');chars->insert('z');
		chars->insert('A');chars->insert('B');chars->insert('C');
		chars->insert('D');chars->insert('E');chars->insert('F');
		chars->insert('G');chars->insert('H');chars->insert('I');
		chars->insert('J');chars->insert('K');chars->insert('L');
		chars->insert('M');chars->insert('N');chars->insert('O');
		chars->insert('P');chars->insert('Q');chars->insert('R');
		chars->insert('S');chars->insert('T');chars->insert('U');
		chars->insert('V');chars->insert('W');chars->insert('X');
		chars->insert('Y');chars->insert('Z');
		chars->negate();
		ptr++;
	}else{
		next=escaped(c);
		chars->insert(next);
		ptr++;
	}
	return ptr;
}

int regex_parser::process_quantifier(const char *re, int ptr, int *lb_p, int *ub_p){
	int lb=0;
	int ub=_INFINITY;
	int res=sscanf(re+ptr,"%d",&lb);
	if (res!=1) fatal("regex_parser:: process_quantifier: wrong quantified expression.");
	while(ptr!=strlen(re) && re[ptr]!=COMMA && re[ptr]!=CLOSE_QBRACKET)
		ptr++;
	if (ptr==strlen(re) || (re[ptr]==COMMA && ptr==strlen(re)-1))
		fatal("regex_parser:: process_quantifier: wrong quantified expression.");
	if(re[ptr]==CLOSE_QBRACKET){
		ub=lb;
	}else{
		ptr++;
		if(re[ptr]!=CLOSE_QBRACKET){
			res=sscanf(re+ptr,"%d}",&ub);
			if (res!=1) fatal("regex_parser:: process_quantifier: wrong quantified expression.");
		}
	}
	while(re[ptr]!=CLOSE_QBRACKET)
		ptr++;
	(*lb_p)=lb;
	(*ub_p)=ub;
	ptr++;
	return ptr;
}

int regex_parser::process_range(NFA **fa, NFA **to_link, const char *re, int ptr){
	if (re[ptr]==CLOSE_SBRACKET)
		fatal("regex_parser:: process_range: empty range.");
	bool negate=false;
	int from=CSIZE+1;
	int_set *range=new int_set(CSIZE);
	if(re[ptr]==TILDE){
		negate=true;
		ptr++;
	}
	while(ptr!=strlen(re)-1 && re[ptr]!=CLOSE_SBRACKET){
		symbol_t to = re[ptr];
		//if (is_special(to) && to!=ESCAPE)
		//	fatal("regex_parser:: process_range: invalid character.");
		if (to==ESCAPE){
			int_set *chars=new int_set(CSIZE);
			ptr=process_escape(re,ptr+1,chars);
			to=chars->head();
			delete chars;
		}
		else
			ptr++;
		if (from==(CSIZE+1)) from=to;
		if (ptr!=strlen(re)-1 && re[ptr]==MINUS_RANGE){
			ptr++;
		}else{
			if (from>to)
				fatal("regex_parser:: process_range: invalid range.");
			for(symbol_t i=from;i<=to;i++){
				range->insert(i);
				if (i==255) break;
			}
			from=CSIZE+1;
		}
	}
	if (re[ptr]!=CLOSE_SBRACKET)
		fatal("regex_parser:: process_range: range not closed.");
	ptr++;
	if (i_modifier){
		int_set *is=new int_set(CSIZE);
		for (unsigned v=range->head();v!=UNDEF;v=range->suc(v)){
			if (v>='A' && v<='Z') is->insert(v+('a'-'A'));
			if (v>='a' && v<='z') is->insert(v-('a'-'A'));
		}
		range->add(is);
		delete is;
	}
	if (negate) range->negate();
	if(ptr==strlen(re)||!is_repetition(re[ptr])){
		(*fa)=(*fa)->add_transition(range);
	}else{
		(*to_link)=new NFA();
		(*to_link)=(*to_link)->add_transition(range);
	}
	delete range;
	return ptr;
}
