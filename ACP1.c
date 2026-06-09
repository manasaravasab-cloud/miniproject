//So since we know the equation of the circle, and given x, and y , we can just draw the circle, like whereever with the given
//  values of x and y if it satisfies the circle equation then mark * at that point with this you can just draw a circle
#include<stdio.h>
int main(){
    float x=0, y=0;
   float r=9.0;
    for(x=-12.0;x<=12.0;x+=0.4){
        for(y=-12.0;y<=12.0;y+=0.2){
            if(x*x+y*y>= r*r-3.0&&x*x+y*y<=r*r+5.0)
            {
                printf("*");
            }
            else{
                printf(" ");
            }
        }
        printf("\n");
    }
    return 0;
}