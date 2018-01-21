/*
 * Author: Keith Stellyes
 *
 * A JITing Brainfuck engine.
 * The only optimization it makes is optimizing multiple add's into a single
 * "instruction"
 *
 * An interesting expansion on this would be to implement peep-hole
 * optimzations. Such as optimizing [-] as tape[index] = 0
 *
 * This was created mostly as an exercise in using GNU Lightning to write a
 * JIT compiler, and as such has a few flaws. Such as little configurations,
 * and little error correction/recovery (e.g. mismatched [] or too many open [
 * in a row w/o being closed will all result in segfaults.
 * 
 */
#include <stdio.h>
#include <stdbool.h>
#include <lightning.h>

#define TAPE_LEN 30000
static char tape[TAPE_LEN] = {0};
static int tape_index = 0;
static jit_state_t *_jit;

char next_byte(FILE *f)
{
    char c;
    while(fread(&c, 1, 1, f)) {
        switch(c) {
	    case '+':
            case '-':
            case '[':
            case ']':
            case '<':
            case '>':
            case '.':
            case ',':
              return c;
	    default:
              break;
	}
    }
    return 0;
}

void tp_putc()
{
    printf("%c", tape[tape_index]);
}

void tp_getc()
{
    fread(tape + tape_index, 1, 1, stdin);
}

void tp_left(int left)
{
    tape_index -= left;
    if(tape_index < 0) {
        tape_index = TAPE_LEN + tape_index;
    }
}

void tp_right(int right)
{
    tape_index += right;
    if(tape_index >= TAPE_LEN) {
        tape_index -= TAPE_LEN;
    }
}

void tp_setz()
{
    tape[tape_index] = 0;
}

void tp_add(int i)
{
    tape[tape_index] += i;
}

void tp_sub(int i)
{
    tape[tape_index] -= i;
}

bool is_zero()
{
    return tape[tape_index] == 0;
}

int main(int argc, char **argv)
{
    char byte;
    int n;
    FILE *f = fopen(argv[1], "r");
    jit_node_t *in;
    init_jit(argv[0]);
    _jit = jit_new_state();
    
    jit_prolog();
    // Bug, If we have more than 100 [ then this will break
    jit_node_t *stack[100];
    int stackp = 0;
    jit_node_t *label;

    while((byte = next_byte(f))) {
sw:       
	switch(byte) {
	    case '+':
              n = 1;
	      // Optimizes multiple BF adds into a a single add
	      while((byte=next_byte(f)) == '+') n++;
	      jit_prepare();
	      jit_pushargi(n);
	      jit_finishi(tp_add);
	      /* 
	       * Oh no a goto!!! :CCC the loop above will 
	       * consume the next token, so we gotta skip
	       * our loop condition which would make us lose the token.
	       */
              goto sw;
	    case '-':
	      n = 1;
	      while((byte=next_byte(f)) == '-') n++;
	      jit_prepare();
	      jit_pushargi(n);
	      jit_finishi(tp_sub);
	      goto sw;
	    case '[':
	      jit_prepare();
	      jit_finishi(is_zero);
	      jit_retval(JIT_R0);
	      stack[++stackp] = jit_beqi(JIT_R0, true); // I jump to ]
	      stack[++stackp] = jit_label(); // jump to me from ]
	      break;
	    case ']':
	      jit_prepare();
	      jit_finishi(is_zero);
	      jit_retval(JIT_R0);
	      label = jit_beqi(JIT_R0, false);
	      // We do a jit_patch_at if we wanna jump from an earlier point in 
	      // the code to now
	      jit_patch_at(label, stack[stackp--]);
	      // We do a jit_patch if we wanna jump to an earlier point in the 
	      // code
	      jit_patch(stack[stackp--]);
	      break;
	    case '<':
	      n = 1;
	      while((byte=next_byte(f)) == '<') n++;
	      jit_prepare();
	      jit_pushargi(n);
	      jit_finishi(tp_left);
	      goto sw;
	    case '>':
	      n = 1;
	      while((byte=next_byte(f)) == '>') n++;
	      jit_prepare();
	      jit_pushargi(n);
	      jit_finishi(tp_right);
	      goto sw;
	    case '.':
	      jit_prepare();
	      jit_finishi(tp_putc);
	      break;
	    case ',':
	      jit_prepare();
	      jit_finishi(tp_getc);
	      break;
	    default:
	      break;
	}
    }
    jit_ret();
    void (*func)(void) = jit_emit();
    if(argc == 3)
        jit_disassemble();
    func();
    /* cleanup */
    jit_destroy_state();
    finish_jit();
}
