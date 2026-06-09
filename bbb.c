#include<stdio.h>
int main()
{
    int x1,x2,x3,x4;
    int y1,y2,y3,y4;
    printf("enter the values of x co-ordinates\n");
    scanf("%d%d%d%d",&x1,&x2,&x3,&x4);
    printf("enter the values of y co-ordinates\n");
    scanf("%d%d%d%d",&y1,&y2,&y3,&y4);
    if(y1==y4){
        for(int i=x1;i<=x4;i++){
            printf("*\n");
        }
    }
   else if(x4==x3){
        for(int i=y4;i<=y3;i++){
            printf("*\n");
        }
    }
    else if(x3==x2){
        for(int i=y2;i<=y3;i++){
            printf("*\n");
        }
    }
    else if(y1==y2){
        for(int i=x1;i<=x2;i++){
            printf("*\n");
        }
    }
    return 0;
}