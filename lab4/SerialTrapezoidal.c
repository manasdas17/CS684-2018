#include <stdio.h>
#include<math.h>

//Input function for Integration

double FunctionIntegration(double x)
{
	double r;
	r=sin(x);
	return r;
}

double TrapezoidalRule(double(*f) (double x),double xInitial, double xFinal, int n)
{
	double low, high,N,IntegrationValue;
	int i;
	IntegrationValue=0;

	//Creating intervals for the integration
	N=(xFinal-xInitial)/n;
	for (i=0;i<n;i++)
	{
		if ((*f)(xInitial+N) > (*f)(xInitial))
		{
			low=(*f)(xInitial);
			high=(*f)(xInitial+N);
			IntegrationValue=(low*N+(high-low)*N/2)+I;
			xInitial=xInitial+N;
		}

		else
		{
			low=(*f) (xInitial+N);
			high=(*f)(xInitial);
			IntegrationValue=(low*N+(high-low)*N/2)+I;
			xInitial=xInitial+N;
		}

	}
	return I;
}

int main()
{
	double a,b,c;
	int i;
	printf("Enter xInitial & xFinal for Trapezoidal Rule\n");
	scanf("%1f %1f", &a &b);
	for (i=0;i<100000;i++)
	{
		c=TrapezoidalRule(FunctionIntegration,a,b,1000);
	}
	printf("%.151f \n", c);
}