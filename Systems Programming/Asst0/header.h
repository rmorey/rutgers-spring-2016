#ifndef header_h_
#define header_h_

// Debug Macros 
#define debugc(x) printf(#x " is: %c\n", x)
#define debugs(x) printf(#x " is: %s\n", x)
#define debugi(x) printf(#x " is: %d\n", x)
#define debugf(x) printf(#x " is: %f\n", x)
#define debugT(x) printf(#x ":\nplace: %d\nstream: %s\n", x->place, x->stream)

#endif
