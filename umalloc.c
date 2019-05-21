#include "types.h"
#include "stat.h"
#include "user.h"
#include "param.h"

// Memory allocator by Kernighan and Ritchie,
// The C programming Language, 2nd ed.  Section 8.7.


typedef long Align;

union header {
  struct {
    union header *ptr;  // next block if on free list
    uint size;  // size of this block
  } s;
  Align x;  // force alignment of blocks
};

typedef union header Header;

static Header base; // empty list to get started
static Header *freep;   // start of free list

void
free(void *ap)
{
  Header *bp, *p;

  bp = (Header*)ap - 1;
  for(p = freep; !(bp > p && bp < p->s.ptr); p = p->s.ptr)
    if(p >= p->s.ptr && (bp > p || bp < p->s.ptr))
      break;
  if(bp + bp->s.size == p->s.ptr){
    bp->s.size += p->s.ptr->s.size;
    bp->s.ptr = p->s.ptr->s.ptr;
  } else
    bp->s.ptr = p->s.ptr;
  if(p + p->s.size == bp){
    p->s.size += bp->s.size;
    p->s.ptr = bp->s.ptr;
  } else
    p->s.ptr = bp;
  freep = p;
}

static Header*
morecore(uint nu)
{
  char *p;
  Header *hp;

  if(nu < 4096)
    nu = 4096;
  p = sbrk(nu * sizeof(Header));
  if(p == (char*)-1)
    return 0;
  hp = (Header*)p;
  hp->s.size = nu;
  free((void*)(hp + 1));
  return freep;
}

void*
malloc(uint nbytes)
{
  Header *p, *prevp;
  uint nunits;

  nunits = (nbytes + sizeof(Header) - 1)/sizeof(Header) + 1;
  if((prevp = freep) == 0){ // no free list yet
    base.s.ptr = freep = prevp = &base;
    base.s.size = 0;
  }
  for(p = prevp->s.ptr; ; prevp = p, p = p->s.ptr){
    if(p->s.size >= nunits){    // big enough
      if(p->s.size == nunits)   // exactly
        prevp->s.ptr = p->s.ptr;
      else {    // allocate tail end
        p->s.size -= nunits;
        p += p->s.size;
        p->s.size = nunits;
      }
      freep = prevp;
      return (void*)(p + 1);
    }
    if(p == freep)  // wrapped around free list
      if((p = morecore(nunits)) == 0)
        return 0;   // none left
  }
}
//
//static Header *
//pmorecore(uint nu) {
//    char *p;
//    Header *hp;
//
//    p = sbrk(nu * sizeof(Header));
//    if (p == (char *) -1)
//        return 0;
//    hp = (Header *) p;
//    hp->s.size = nu;
//    free((void *) (hp + 1));
//    return freep;
//}
//
//void*
//pmalloc(){
//    Header *p = pmorecore(512);
//
//}

void*
pmalloc(){
    char *p;
    Header *hp;
    uint nu=512;

    p = sbrk(nu * sizeof(Header));
    if(p == (char*)-1)
        return 0;
    hp = (Header*)p;
    hp->s.size = nu;
    free((void*)(hp + 1));

    Header* con=freep;

    // TODO:hear we need to raise the flag of con with a syscall which gets con
    /*
     * as i understands we need to use xor on the the flag PTE_A3_PROT I added.
     * I think an example for flag raising can be found in vm.c line 73.
     */

    return con;
}

int
protect_page(void* ap){
    /*
     * TODO: need to call a sys call that do the following thing:
     *  1. check that the ap has the PTE_A3_PROT flag raised.
     *  2. check that the offset is 0
     *  3. turn of the flag PT_W.
     *  if 1,2 are false return -1
     */
}

Int pfree(void* ap){
    /*
     * TODO: need to call 3 sys calls:
     *  1. syscall that checks if the flag PTE_A3_PROT is on other wis retur -1;
     *  2. syscall that check if the flag PTE_W is raised.
     *  3. syscall that turn on the PTE_W flag.
     */

    free(ap);
    return 1;
}
