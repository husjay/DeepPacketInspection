#include "int_set.h"
#include "stdinc.h"

int_set::int_set(unsigned int N1) {
	N = N1; 
	item = new bool[N+1];
	for (unsigned int i = 0; i <= N; i++) item[i] = false;
	num_el=0;
	first=UNDEF;
}

int_set::~int_set() { delete [] item; }

// Change num_el of int_set. Discard old value.
void int_set::reset(unsigned int N1) {
	delete [] item; N = N1; item = new bool[N+1];
	for (unsigned int i = 0; i <= N; i++) item[i] = false;
	num_el=0;
	first=UNDEF;
}
	
// Remove all elements from int_set.
void int_set::clear() {
	for (unsigned int i = 0; i <= N; i++) item[i] = false;
	num_el=0;
	first=UNDEF;
}

// Print the contents of the int_set.
void int_set::print() {
	fprintf(stderr,"[ ");
	for (unsigned int i = 0; i <= N; i++)
		if (item[i]) fprintf(stderr,"%d ",i);
	fprintf(stderr,"]\n");
}

void int_set::operator=(int_set& L) {
	if (N != L.N) {
		N = L.N;
		delete [] item; item = new bool[L.N+1];
	};
	num_el=L.num_el;	
	first=L.first;
	for (unsigned int i = 0; i <= N; i++) item[i] = L.item[i];
}

void int_set::insert(unsigned int i){
	if (i < 0 || i > N) fatal("int_set::insert: item out of range");
	if (first==UNDEF || first > i) first=i;
	if (!item[i]) num_el++;
	item[i]=true;
}

void int_set::remove(unsigned int i)	{
	if (i < 0 || i > N) fatal("int_set::remove: item out of range");
	if (first==i) first=suc(i);
	if (item[i])num_el--;
	item[i]=false;
}

bool int_set::mbr(unsigned int i){		
	if (i < 0 || i > N) fatal("int_set::mbr: item out of range");
	return (item[i]);
}	

unsigned int int_set::suc(unsigned int i){
	if (i < 0 || i > N) fatal("int_set::suc: item out of range");
	if (!item[i] || i==N) return UNDEF;
	for (unsigned int j=i+1;j<=N;j++)
		if (item[j]) return j;
	return UNDEF;	
} 

void int_set::add(int_set *s){
	if (s->N>N) N=s->N;
	unsigned int item=s->first;
	while(item!=UNDEF){
		insert(item);
		item=s->suc(item);
	}
}

void int_set::negate(){
	first=UNDEF;
	num_el=0;
	for (unsigned int i = 0; i < N; i++){
		item[i] = !item[i];
		if (item[i]){
			if (first==UNDEF) first=i;
			num_el++;
		}	
	}
}
