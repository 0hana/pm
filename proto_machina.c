/* Hanami Zero (C) 2022: proto_machina.c */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef enum ternary { T0, T1, T2 } ternary;

ternary compare_strings(char const * const S1, char const * const S2)
{
	size_t I = 0; while(S1[I] == S2[I]) if(S1[I] == '\0') return T0; else I++;
	
	return S1[I] > S2[I] ? T1 : T2;
}


/* Dictionary */
struct tree
{
	size_t subtrees;
	//size_t definition_depth;
	struct tree * L, * R; /*, * U; */
	char * name, * definition;  // interdependence can be determined via pointers
} * dictionary = NULL;

size_t tree_weight(struct tree * T) { return T ? T->subtrees + 1 : 1 ; }
#define DELTA 3
#define GAMMA 2


#define _tree_X_is_null_balance(L,R)\
	struct tree Copy; \
	if(T->L->L == NULL) { T->L->R->L = T->L->R; Copy = *(T->L); Copy.R = NULL; *(T->L) = *(T->L->R); *(T->L->L) = Copy; } \
	\
	/* Now T->L->R == NULL */\
	\
	T->L->R = T->L; Copy = (*T); Copy.L = NULL; (*T) = *(T->L); *(T->R) = Copy;  // Spooky eh?


void tree_R_is_null_balance(struct tree * T) { _tree_X_is_null_balance(L,R) }
void tree_L_is_null_balance(struct tree * T) { _tree_X_is_null_balance(R,L) }


#define _tree_rotate(R,L)\
	if(tree_weight((*T)->R->L) < (GAMMA * tree_weight((*T)->R->R)))\
	{\
		struct tree * S = (*T);\
		(*T) = (*T)->R;\
		S->R = (*T)->L;\
		(*T)->L = S;\
		\
		S->subtrees = S->L->subtrees + S->R->subtrees + 1;\
		(*T)->subtrees = (*T)->L->subtrees + (*T)->R->subtrees + 1;\
	}\
	else\
	{\
		struct tree * S = (*T)->R->L;\
		(*T)->R->L = (*T)->R->L->R;\
		\
		(*T)->R->subtrees = (*T)->R->L->subtrees + (*T)->R->R->subtrees + 1;\
		\
		S->R = (*T)->R;\
		(*T)->R = S->L;\
		\
		(*T)->subtrees = (*T)->L->subtrees + (*T)->R->subtrees + 1;\
		\
		S->L = (*T);\
		(*T) = S;\
		\
		(*T)->subtrees = (*T)->L->subtrees + (*T)->R->subtrees + 1;\
	}

// performs at least 1 "right" R rotation
void tree_rotate_R(struct tree ** T) { _tree_rotate(L,R) }

// performs at least 1 "left"  L rotation
void tree_rotate_L(struct tree ** T) { _tree_rotate(R,L) }


// Only for non-null trees -- handle NULL dictionary externally
struct tree * tree_insert(struct tree ** T, char * name, char * definition /*, size_t definition_depth */)
{
	switch(compare_strings((*T)->name, name))
	{
		case T0:
		{
			// 2 sub-cases:
			// current depth == definition depth -> overwrite current definition
			// current depth != definition depth -> stack definitions
			// But for now, just overwrite

			if(1/*(*T)->definition_depth == definition_depth*/)
			{
				//free(T->definition);
				(*T)->definition = definition;
			}
			//else
			//{
			//	struct tree * S = malloc(struct tree)
			//}
			return (*T);
		}
		case T1:
		{
			if((*T)->L == NULL) { (*T)->L = malloc(sizeof(struct tree)); if((*T)->L) *((*T)->L) = (struct tree){ 1, NULL, NULL, name, definition }; return (*T)->L; }
			else
			{
				struct tree * S = tree_insert(&((*T)->L), name, definition);
				if(S) { (*T)->subtrees += 1; if(tree_weight((*T)->L) > (DELTA * tree_weight((*T)->R))) tree_rotate_R(T); }
				return S;
			}
		}
		case T2:
		{
			if((*T)->R == NULL) { (*T)->R = malloc(sizeof(struct tree)); if((*T)->R) *((*T)->R) = (struct tree){ 1, NULL, NULL, name, definition }; return (*T)->R; }
			else
			{
				struct tree * S = tree_insert(&((*T)->R), name, definition);
				if(S) { (*T)->subtrees += 1; if(tree_weight((*T)->R) > (DELTA * tree_weight((*T)->L))) tree_rotate_L(T); }
				return S;
			}
		}
	}
}


struct tree * tree_search(struct tree * T, char * name)
{
	if(T == NULL) return T;
	switch(compare_strings(T->name, name))
	{
		case T0: return T;
		case T1: return tree_search(T->L, name);
		case T2: return tree_search(T->R, name);
	}
}


void delete_tree(struct tree * T)
{
	if(T->L) { delete_tree(T->L); T->L = NULL; }
	if(T->R) { delete_tree(T->R); T->R = NULL; }
	free(T);
}


struct tree * tree_remove(struct tree * T, char * name);


int main(int const main_parameters, char const * const main_parameter[])
{
	// Initialize dictionary
	dictionary = malloc(sizeof(struct tree)); *dictionary = (struct tree){ 1, NULL, NULL, ":", "" };
	printf("Successfully inserted \"%s\"\n", ":");

	for(int I = 1; I < main_parameters; I++)
	{
		if(tree_insert(&dictionary, main_parameter[I], "")) printf("Successfully inserted \"%s\"\n", main_parameter[I]);
	}

	printf("Found \"%s\"\n", tree_search(dictionary, ":")->name);

	for(int I = 1; I < main_parameters; I++)
	{
		printf("Found \"%s\"\n", tree_search(dictionary, main_parameter[I])->name);
	}

	delete_tree(dictionary);
	dictionary = NULL;
}

/*
	f x y z
*/
