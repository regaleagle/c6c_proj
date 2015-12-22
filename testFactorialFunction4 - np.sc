// Test if function can call another function
geti(In);
Fact1(){puts("In Fact0");1;}
// Fact(k){if(k==1){z=Fact1();}else{z=Fact(k-1);}; k*z;}
Fact(k){x=Fact1(); if(k==1){z=x;}else{z=Fact(k-1);}; k*z;}
d = Fact(In);
puti(d);
puts("OTAY");