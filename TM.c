#include<stdio.h>
#include<stdlib.h>
#include<string.h>

typedef struct tapes {
  struct tapes *left;
  struct tapes *right;
  char content;
} Tape;

typedef enum { LEFT,RIGHT } Direction;


//state set{s,a,c,n,o,r,h} corresponding state
//start, add, carry, noncarry, overflow, return, halt        
typedef struct transition {
  char current_state;
  char tape_symbol;
  char new_state;
  char new_tape_symbol;
  Direction dir;
} Transition;

typedef struct list {
  Transition *content;
  struct list *next;
} List;

typedef struct tm {
  char *input_alpha;
  char *input;
  char *tape_alpha;
  char start;
  char accept;
  char reject;
  List *transition;
} TM;

Tape *insert_tape(Tape *t, Direction dir, char c) {
  Tape *head = t;
  Tape *new1 = calloc(1,sizeof(Tape));;
  new1 -> content = c;
  if(dir == LEFT) {
    while(t->left != NULL) {
      t = t->left;
    }
    new1->right = t;
    new1->left = NULL;
    t->left = new1;
    return new1;
  }
  if(dir == RIGHT) {
    while(t->right != NULL) {
      t = t->right;
    }
    new1->left = t;
    new1->right = NULL;
    t->right = new1;
  }
  return head;
}

Tape *create_tape(char *input) {
  int i=1;
  Tape *t = calloc(1,sizeof(Tape));
  t->content = input[0];
  while(1) {
    if(input[i] == '\0') break;
    t = insert_tape(t,RIGHT,input[i]);
    i++;
  }
  return t;
}

/* turn the input string into Transition fields, convert a string to a transition struct */
Transition *get_transition(char *s) {
  Transition *t = calloc(1,sizeof(Transition));
  Direction dir;
  t->current_state = s[0];
  t->tape_symbol = s[1];
  t->new_state = s[2];
  t->new_tape_symbol = s[3];
  dir = (s[4]=='R')? RIGHT:LEFT;
  t->dir = dir;
  return t;
}

/* turn the string into transitions and add into list */
List *insert_list( List *l, char *elem ) {
  List *t = calloc(1,sizeof(List));
  List *head = l;
  while(l->next!=NULL)
    l = l->next;
  t->content = get_transition(elem);
  t->next = NULL;
  l->next = t;
  return head;
}

/* insert a transition into a list */
List *insert_list_transition( List *l, Transition *tr) {
  List *t = calloc(1,sizeof(List));
  List *head = l;
  while(l->next!=NULL)
    l = l->next;
  t->content = tr;
  t->next = NULL;
  l->next = t;
  return head;
}

void print_tape( Tape *t,char blank) {
  char c;
  /*
    while(1) {
    if(t->content != blank) break;
    t= t->right;
    }
  */
  while(1) {
    if(t==NULL) break;
    c = t->content;
    if(t->content != blank)
      putchar(c);
    t= t->right;
  }
  putchar('\n');
}

void print_transition (Transition *t) {
  char s1[] = "Left";
  char s2[] = "Right";
  if(t==NULL) {
    printf("NULL Transfer");
    return;
  }
  printf("current:%c tape:%c new state:%c new tape:%c direction %s\n",t->current_state,t->tape_symbol,t->new_state,t->new_tape_symbol,(t->dir == LEFT)?s1:s2);
}

/*test if the char c is in the string s */
int contains ( char c, char *s ) {
  int i=0;
  while(1) {
    if(c== s[i]) return 1;
    if(s[i] == '\0') return 0;
    i++;
  }
}

/* test if the input is a valid input */
int is_valid_input( char *input_alpha, char *input ) {
  int i=0;
  char c;
  while(1) {
    c = input[i];
    if(c == '\0') break;
    if(!contains(c,input_alpha)) return 0;
    i++;
  }
  return 1;
}

TM *createTM (char *input) {

  TM *m = calloc(1,sizeof(TM));
  List *tr = calloc(1,sizeof(List));
  char *buffer;
  /*read input alphabet of PDA*/
  buffer = strtok(input,":");
  if(buffer == NULL) {
    printf("Error in reading input alphabet!\n");
    exit(1);
  }
  m->input_alpha = buffer;

  /*read tape alphabet*/
  buffer = strtok(NULL,":");

  if(buffer == NULL) {
    printf("Error in reading tape alphabet!\n");
    exit(1);
  }
  m->tape_alpha = buffer;

  /*read input sequence*/
  buffer = strtok(NULL,":");
  if(buffer == NULL) {
    printf("Error in reading input sequence!\n");
    exit(1);
  }

  if(!is_valid_input(m->input_alpha,buffer)) {
    printf("Error! Input contains some invalid characters that don't match the input alphabet!\n");
    exit(1);
  }

  m->input = buffer;
  buffer = strtok(NULL,":");
  m->start = buffer[0];
  buffer = strtok(NULL,":");
  m->accept = buffer[0];
  buffer = strtok(NULL,":");
  m->reject = buffer[0];

  /*read tape transition*/
  while(1) {
    buffer = strtok(NULL,":");
    if(buffer == NULL) break;
    tr = insert_list(tr,buffer);
  }

  m->transition = tr->next;
  return m;
}

Transition *find_transition(List * list,char state, char tape_symbol) {
  Transition *t;
  while(1) {
    if(list==NULL) return NULL;
    t = list -> content;
    if(t->current_state == state && t->tape_symbol == tape_symbol)
      return t;
    list = list->next;
  }
}

Tape *move(Tape *t,Direction dir, char blank) {
  if(dir == LEFT) {
    if(t->left==NULL) {
      t = insert_tape(t,LEFT,blank);
    }
    return t->left;
  }
  if(dir == RIGHT) {
    if(t->right==NULL) {
      t = insert_tape(t,RIGHT,blank);
    }
    return t->right;
  }
  return NULL;
}

void simulate( TM *m ) {
  /* first symbol in input symbol used to represent the blank symbol */
  const char blank = m->tape_alpha[0];
  char current_state = m->start;
  Tape *tape = create_tape(m->input);
  Tape *current_tape = tape;
  char current_tape_symbol;
  Transition *current_transition;
  while(1) {
    if(current_state == m->accept) {
      printf("Accept\n");
      print_tape(tape,blank);
      break;
    }
    if(current_state == m->reject) {
      printf("Reject\n");
      print_tape(tape,blank);
      break;
    }
    current_tape_symbol = (current_tape==NULL||current_tape ->content == '\0')?blank:current_tape->content;
    current_transition = find_transition(m->transition,current_state,current_tape_symbol);
    current_state = current_transition -> new_state;
    current_tape -> content = current_transition -> new_tape_symbol;
    current_tape = move( current_tape, current_transition ->dir, blank);
  }
}


int testTM(void){	
  printf("testTM");
  /*create a TM & transition table*/
    
  TM Add_One;
  List Rule_Table;

  Transition Rule1; 	
  Rule1.current_state = 's';
  Rule1.tape_symbol = '*';
  Rule1.new_state = 'a';
  Rule1.new_tape_symbol = '*';
  Rule1.dir= LEFT;
  print_transition(&Rule1);
  //insert
  //insert_list_transition(&Rule_Table,&Rule1);
  Rule_Table.content = &Rule1;	
  Transition* Rule2 = get_transition("a0n1L");
  Transition* Rule3 = get_transition("a1c0L");
  Transition* Rule4 = get_transition("a*h*R");
  Transition* Rule5 = get_transition("c0n1L");
  Transition* Rule6 = get_transition("c1c0L");
  Transition* Rule7 = get_transition("c*o1L");
  Transition* Rule8 = get_transition("n0n0L");
  Transition* Rule9 = get_transition("n1n1L");
  Transition* Rule10 = get_transition("n*r*R");
  Transition* Rule11 = get_transition("o1r*R");	
  Transition* Rule12 = get_transition("o0r*R");
  Transition* Rule13 = get_transition("r0r0R");	
  Transition* Rule14 = get_transition("r1r1R");	
  Transition* Rule15 = get_transition("r*h*L");//in this case, stay,direction doesn't matter;
	
  insert_list_transition(&Rule_Table,Rule2);	
  insert_list_transition(&Rule_Table,Rule3);
  insert_list_transition(&Rule_Table,Rule4);
  insert_list_transition(&Rule_Table,Rule5);
  insert_list_transition(&Rule_Table,Rule6);
  insert_list_transition(&Rule_Table,Rule7);
  insert_list_transition(&Rule_Table,Rule8);
  insert_list_transition(&Rule_Table,Rule9);
  insert_list_transition(&Rule_Table,Rule10);
  insert_list_transition(&Rule_Table,Rule11);
  insert_list_transition(&Rule_Table,Rule12);
  insert_list_transition(&Rule_Table,Rule13);
  insert_list_transition(&Rule_Table,Rule14);
  insert_list_transition(&Rule_Table,Rule15);
	
	
  List* head = &Rule_Table;
  Tape* My_Tape = create_tape("*10*");
  print_tape(My_Tape,' ');
  Tape* temp = My_Tape;
  //move to rightmost
  while(temp->right != NULL){
    temp = temp->right;
  }
  printf("current is : %c, left is %c\n",temp->content,temp->left->content);
  //start add one TM
  char current_state_symbol = 's';
  char current_tape_symbol;
  Tape* current_tape = temp;
	
  while(current_state_symbol != 'h'){
    current_tape_symbol = current_tape->content;
    Transition* transition_rule = find_transition(head,current_state_symbol,current_tape_symbol);
    //write to current tape and state register
		
    current_state_symbol = transition_rule->new_state;
    current_tape->content = transition_rule->new_tape_symbol;
    printf("new state:%c,new tape content: %c \n",current_state_symbol,current_tape->content);
    //move
    current_tape = move(current_tape,transition_rule->dir,'*');
  }
  print_tape(My_Tape, ' ');
  return 0;

}

int main(void) {  	
  char s[300];
  TM *p;
  scanf("%s",s);
  printf("%s",s);
  return 0;
  p = createTM(s);
  simulate(p);
  return 0;
}

