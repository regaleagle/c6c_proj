// Test if Functions can be used within if condition
geti(In);
Prime(n){	bool=1;
			for(x=2;x<n;x=x+1;)
			{	x1=n%x; 
				if(x1==0)
				{	bool=0;
					break;
				};
			}; 
		bool;}
if(Prime(In))
puts("Input is Prime");
else
puts("Input is not Prime");
puts("OTAY");