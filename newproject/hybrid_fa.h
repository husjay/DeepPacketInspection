#ifndef __HYBRID_FA_H_
#define __HYBRID_FA_H_

#include "stdinc.h"
#include "dfa.h"
#include "nfa.h"
#include <list>
#include <set>
#include <map>
#include <utility>

using namespace std;

typedef map <state_t,nfa_set*>::iterator border_it;

class HybridFA{
	
	/* orginal NFA (where to get the tail information from */
	NFA *nfa;
	
	/* head-DFA */
	DFA *head;
	
	/* border: mapping between DFA state and corresponding set of NFA states */ 
	map <state_t, nfa_set*> *border;
	
	/* non-special states (for construction: to avoid that a state classified as non-special at the beginning
	 * of the construction becomes special later on) */
	nfa_set *non_special;
	
public:

	/* constructor */
	HybridFA(NFA *_nfa);
	
	/* de-allocator */
	~HybridFA();

	/* returns underlying NFA */
	NFA *get_nfa();

	/* returns the head */
	DFA *get_head();
	
	/* returns the border */
	map <state_t, nfa_set*> *get_border();
	
	/* return the number of tail-automata */
	unsigned get_num_tails();
	
	/* return the tail size */
	unsigned get_tail_size();

	/* performs DFA minimization on the head-DFA*/
	void minimize();
	
	/* exports the head of the Hybrid-FA into format suitable for dot program (http://www.graphviz.org/) */
	void to_dot(FILE *file, const char *title);
	
private:

	// builds the hybrid-FA for the current NFA.
	void build();

	// determines when a NFA state is special (that is, it must not be expanded).
	bool special(NFA *nfa);
	
	// modified nfa so that its more suitable for Hybrid-FA creation
	void optimize_nfa_for_hfa(NFA *nfa, unsigned depth);
	
};

inline NFA *HybridFA::get_nfa(){return nfa;}

	/* returns the head */
inline DFA *HybridFA::get_head(){return head;}
	
	/* returns the border */
inline map <state_t, nfa_set*> *HybridFA::get_border(){return border;}

#endif /*__HYBRID_FA_H_*/

