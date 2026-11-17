  /* [input] */
#include <stdio.h>
#include <stdlib.h>

void main()
{
  int i,n,temp,j,arr[25];
  printf("Enter the number of elements in the Array: ");
  scanf("%d",&n);
  printf("\nEnter the elements:\n\n");

  for(i=0 ; i<n ; i++)
  {
    printf(" Array[%d] = ",i);
    scanf("%d",&arr[i]);
  }

  for(i=0 ; i<n ; i++)
  {
    for(j=0 ; j<n-i-1 ; j++)
    {
      if(arr[j]>arr[j+1]) // Keep this comment!
      {
        temp=arr[j];
        arr[j]=arr[j+1];
        arr[j+1]=temp;
      }
    }
  }

  printf("\nThe Sorted Array is:\n\n");
  /// comment to strip
  for(i=0 ; i<n ; i++)
  {
    printf(" %4d",arr[i]);
  }
}
  /* [input] */
