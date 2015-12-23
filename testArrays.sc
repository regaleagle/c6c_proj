// Test Arrays
array x[5],y[5],z[5];
func(){for(x=0;x<5;x=x+1;){@x[x] = x;@y[x] = x;@z[x] = x;}; 100;};
a = func();
puti(a);
puts("x call array:");
puti(x[1]);
puts("y call array:");
puti(y[2]);
puts("z call array:");
puti(z[3]);
func2(){array q[5],y[5],z[5];for(x=0;x<5;x=x+1;){q[x] = x;y[x] = x;z[x] = x;};puts("q loc call array:"); puti(q[1]);puts("y loc call array:");puti(y[2]);puts("z loc call array:");puti(z[3]); z[4];};
b = func2();
puti(b);