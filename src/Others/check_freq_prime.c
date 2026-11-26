#include <stdio.h>
#include <conio.h>
#include <math.h>
#define Size 0x2000
typedef unsigned short ushort;
typedef struct _Cell {
	unsigned prmDbl;
	unsigned prmMlt;
	struct _Cell *pNext;
} TCell;
int main(void){
	static TCell *vpCell[0x10000], vCell[Size];
	unsigned r, c, n, s, f;
	TCell **ppCell, **pp, *p;
	printf("    rng    rng/log(rng) real-cnt ( freq.)   error\n\n");
	for (n = 0; n < 0x10000; n++){ vpCell[n] = NULL; }
	r = 10; c = 1; n = 0; s = 3;
	while (1){
		p = NULL;
		ppCell = &vpCell[(ushort)(s >> 1)];
		while (*ppCell != NULL){
			if (s != (*ppCell)->prmMlt){
				ppCell = &((*ppCell)->pNext);
			} else {
				p = *ppCell;
				*ppCell = p->pNext;
				(p->prmMlt) += (p->prmDbl);
				pp = &vpCell[(ushort)((p->prmMlt) >> 1)];
				p->pNext = *pp; *pp = p;
			}
		}
		if (p == NULL){
			c++;
			if (s < 0x10000){
				if (n >= Size){ printf("Exceeded. %u\n", s); return 1; }
				p = &vCell[n]; n++;
				p->prmDbl = (s << 1);
				p->prmMlt = s * s;
				pp = &vpCell[(ushort)((p->prmMlt) >> 1)];
				p->pNext = *pp; *pp = p;
			}
		}
		if (s > r - 2){
			f = (int)round(r / log(r));
			printf("%10u : %9u %9u (%5.2f%%)  %5.1f%%\n",
			       r, f, c, (c * 100.0) / r, (c - f) * 100.0 / f);
			fflush(stdout);
			if (r >= 0xFFFFFFFF){ break; }
			if (r >= 1000000000){ r = 0xFFFFFFFF; } else { r *= 10; }
		}
		s += 2;
	}
	fprintf(stderr, "\nCompleted.  Press any key.\n");
	_getch();
	return 0;
}
