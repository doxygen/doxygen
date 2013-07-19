// objective: test \invariant, \pre and \post commands
// check: 035__invariant_8c.xml

/** \file */

/** \invariant i+j=p
 *  \pre       p\>=0
 *  \post      *q=2^(p+1)
 */
void func(int p,int *q)
{
  int j = p, k=1, i;
  for (i=0; i<=p; i++) j--,k=k*2;
  *q = k;
}
