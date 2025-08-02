#include<stdio.h>
#include<stdlib.h>
//to generate a seed for srand()
#include<sys/time.h>

// macros
#define check( A, M) if( !(A)) { fprintf( stderr, "[ERROR]::{LINE}:%d ", M,"\n"); goto error;}
#define heads 1
#define tails 0

//global declaration
typedef unsigned int uint;

//global variables
const short  max_level = 10;
unsigned short level = 0;

typedef struct node{
	uint key;
	unsigned short level;
	struct node* down;
	struct node* next;
} node;

//function declarations
void print_list( node* head);
void clean_memory( node** ptr_to_head);
uint get_seed();
short flip_a_coin();
void create_level ( node** ptr_to_head1 , node** ptr_to_head2);
void insert ( node** ptr_to_head, uint key);
node* create_hovering_node( node* ptr_node);
node* create_node( uint key);

//creates and initializes a node, for insertion into the SLL at level 0
node* create_node( uint key) {
	//creation
	node* temp = malloc( sizeof( node));
	//initialization
	//COOMENT - NULL is a "macro" for 0
	temp->key = key;
	temp->next = temp->down = 0;
       	temp->level = 0;
	return temp;
}
//creates and initializes a node, for insertion into a skiplist
node* create_hovering_node( node* ptr_node) {
	node* temp = malloc( sizeof( node));
	temp->key = ptr_node->key;
	temp->level = ptr_node->level+1;
	temp->down = ptr_node;
	temp->next = 0;
	return temp;
}

//creates an ordered Singly-linked-list; the order is ascending
void insert ( node** ptr_to_head, uint key) {
	//*ptr_head = head
	node* temp = create_node( key);
	//If the list doesn't exist
	if ( *ptr_to_head == NULL) {
		*ptr_to_head = temp;
		return;
	}
	//else
	node* parse = *ptr_to_head;
	//For special case - If new-key < key of head-node
	if( temp->key < parse->key) {
		*ptr_to_head = temp;
		temp->next = parse;
		return;
	}
	//For the general case
	node* prev = 0;
	while( parse->next != 0) {
		prev = parse;
		parse = parse->next;
		if( temp->key < parse->key ) {
			prev->next = temp;
			temp->next = parse;
			return;
		}
	}
	parse->next = temp;
}

/*creates 1 level of the Singly-Linked-List for the Skiplist
argument 1 = [Pointer to] HEAD of an EXISTING SLL; Suppose, at level 'i'
argument 2 = [Pointer to] HEAD [== NULL]; This SLL will be created at  level 'i+1' */
void create_level ( node** ptr_to_head1 , node** ptr_to_head2) {
	// error check
	if ( *ptr_to_head1  == 0) {
		printf("Level 0 of linked-list does not exist\n");
		return;
	}

	//comment: always promote the first node
	*ptr_to_head2 = create_hovering_node( *ptr_to_head1);

	node* parse1 = ( *ptr_to_head1)->next;
	node* parse2 = *ptr_to_head2;
      	while( parse1 != NULL) {
		//if a head occurs, promote the node, else do not
		//fact: Nodes that do not exist at level 'i', cannot be promoted to level 'i+1'
		if ( flip_a_coin()) {
			parse2->next = create_hovering_node( parse1);
			parse2 = parse2->next;
		}
		parse1 = parse1->next;
	}
	level++;
	//printf("[Level %d created successfully]\n", level);
}

//to generate a random-[positive-]integer [i.e."seed"] for srand()
uint get_seed() {
	//not my code
	struct timeval tv;
	gettimeofday( &tv, NULL);
	uint seed = tv.tv_usec;
	//end of foreign code
	return seed;
}

//flip a fair-coin, to decide if a node should be promoted
short flip_a_coin() {

	short flip = rand()%10;

	if ( flip < 5)
		return heads;
	else
		return tails;
}

//deallocates memory used
void clean_memory( node** ptr_to_head) {
	node *temp, *head = *ptr_to_head;
	while( head != NULL) {
		temp = head;
		head = head->next;
		free(temp);
	}
	*ptr_to_head = 0;
	//printf("Memory deallocated successfully\n");
}

void print_count( node* head) {
	short count = 0;
	while( head != NULL) {
		head = head->next;
		count++;
	}
	printf("Length = %d\n", count);
}

void print_list( node* head) {
	//printf("Data contained:\n");
	int count= 0;
	print_count(head);
	while( head != NULL) {
		printf("%d->", head->key);
		head = head->next;
		count++;
	}
	printf("X\n\n");
}

void test(short highest_level, int inital_length, int max_int) {

	// error check
	if (highest_level > max_level){
		highest_level= max_level;
		printf("Max permissible level is %d\n.", max_level);
	}

	//initializes srand()
	srand( get_seed());

	/*
	creates an array of ptr_node
	Highest level = n, lowest level = 0
	=> total no. of levels = n+1
	*/
	node** ARRAY_OF_HEADS = calloc( (highest_level+1), sizeof ( node*));

	// creates a [Singly-] Linked List at level 0
	int i = 0;
	while ( i++ < inital_length) {
		insert( &ARRAY_OF_HEADS[0], rand()% max_int);
	}
	//end

	//Create the skiplist
	for( i = 1; i <= highest_level; i++)
		create_level( &ARRAY_OF_HEADS[i-1],  &ARRAY_OF_HEADS[i]);

	//print the skiplist
	for( i = highest_level; i >= 0 ; i--) {
		printf("Level %d: ", i);
		print_list( ARRAY_OF_HEADS[i]);
	}

	//deallocate memory-used
	for( i = highest_level; i >= 0; i--)
		clean_memory( &ARRAY_OF_HEADS[i]);
}

int main() {
	//Select the no. of levels in the skiplist
	short highest_level = 7;
	// select the number: of nodes at the base, max-value of a node
	int inital_length = 128, max_int = 2048;
	
 	test(highest_level, inital_length, max_int);

	return 0;
}
