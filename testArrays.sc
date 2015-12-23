// Test Arrays
array x[5],y[5],z[5];
func(){for(x=0;x<5;x=x+1;){@x[x] = x;@y[x] = x;@z[x] = x;}; @z[3];};
a = func();
puti(a);
puts("x call array:");
puti(x[1]);
puts("y call array:");
puti(y[2]);
puts("z call array:");
puti(z[3]);
puts("q call array:");
func2(){array y[5],z[5];for(x=0;x<5;x=x+1;){y[x] = x;z[x] = x;};puts("y loc call array:");puti(y[2]);puts("z loc call array:");puti(z[3]); array q[5] = 5; q[2];};
b = func2();
puti(b);