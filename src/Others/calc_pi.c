#include <stdio.h>
#include <conio.h>
#define Dgt 100000
#define Num ((int)(Dgt*0.2079))

void addAtan(long long s[], long long x0, long long a, long long f);
void divide(long long x[], long long y[], long long n, long long a, long long k);

long long main(void){
  static long long s[Num], c;
  int i, j;
  fprintf(stderr, "Calculating...  Estimated processing time is 26sec.\n\n");
  for(j=0; j<Num; j++)s[j]=0;
  addAtan(s, 16, 5, 1);
  addAtan(s, 4, 239, -1);
  printf("%d.\n", (int)s[0]);
  for(i=5; i<=Dgt; i+=5){
    c=0;
    for(j=Num-1;j>0;j--){c+=s[j]*100000; s[j]=(c&0xFFFFFFFF); c>>=32;}
    printf("%05d", (int)c);
    if((i%100)==0)printf("\n");else if((i%10)==0)printf(" ");
  }
  fprintf(stderr, "\nCompleted.  Press any key.\n");
  _getch();
  return 0;
}

void addAtan(long long s[], long long x0, long long a, long long f){
  static long long x[Num], y[Num], c;
  int k, i, j;
  for(j=0; j<Num; j++){x[j]=0; y[j]=0;}
  x[0]=x0*a;
  a*=a;
  k=0;
  for(i=1; k<Num; i+=2){
    divide(x, x, Num, a, k);
    if(x[k]==0)k++;
    divide(x, y, Num, i, k);
    c=0;
    if(f>0){
      for(j=Num-1;j>=k||c!=0;j--){c+=s[j]+y[j]; s[j]=(c&0xFFFFFFFF); c>>=32;}
    }else{
      for(j=Num-1;j>=k||c!=0;j--){c+=s[j]-y[j]; s[j]=(c&0xFFFFFFFF); c>>=32;}
    }
    f=-f;
  }
}

void divide(long long x[], long long y[], long long n, long long a, long long k){
  long long p, q;
  p=0;
  while(k<n){p=(p<<32)+x[k]; q=p/a; p=p-q*a; y[k]=q; k++;}
}
