#include <stdio.h>
#include <stdlib.h>

#define TEST_OPS

char terminate; 

typedef struct{
 void *previous;
 void *data;
} stacknode;

typedef struct{
  stacknode * stackhead;
  int result;
  int cp; // how far into the code we are
} vm_t;

vm_t *new_vm(){
 vm_t *vm = malloc(sizeof(vm_t));
 vm->result=0;
 vm->stackhead=0;
 vm->cp=0;
 return vm;
}

int get_opcode(void * code,int cp){
  unsigned char * c = code;
  int bytestart = cp/8;
  int bitshift = cp%8;
  return 0xff & ((c[bytestart] << bitshift) | (c[bytestart+1] >> (8-bitshift)));
}

typedef void (*op_t)(vm_t*);

op_t opcodes[256];

void op_test(vm_t *vm){
#ifdef TEST_OPS
  puts("test opcode executed\n");
  printf("\nvm->cp %d\nvm->result %d\n",vm->cp,vm->result);
#endif
  vm->cp++;
}

void op_term(vm_t *vm){
#ifdef TEST_OPS
  puts("Terminating...");
#endif
  terminate = 1;
  vm->cp++;
}

void init_opcodes(){
  for(int i = 0;i<256;i++){
    printf("%d\n",i);
    opcodes[i] = op_test;
  }
  opcodes['!'] = op_term;
}

op_t get_opfn(int oc){
  return opcodes[oc];
}

void process_shift(vm_t *vm, void * code){
  int oc = get_opcode(code,vm->cp);
  printf("oc:%d\n",oc);
  void (*opfn)(vm_t*) = get_opfn(oc);
  opfn(vm);
}

int main(int c, char**v){
  terminate = 0;
  char *fname;
  if(c==2){
    fname = v[1];
    FILE *f = fopen(fname,"r");
    fseek(f,0,SEEK_END);
    size_t fsize = ftell(f);
    void *mem = malloc(fsize);
    fseek(f,0,SEEK_SET);
    size_t nread = fread(mem,1,fsize,f);
    if(nread > 0){
    init_opcodes();
    vm_t *vm = new_vm();

    while(!terminate){
      process_shift(vm,mem);
    }
    }
  }
  return 0;
}
