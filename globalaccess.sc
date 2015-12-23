func1(a,b,c){
    
    puti(a);
    puti(b);
    puti(c);
};
func1(1,2,3);
a = 2;
b = 3;
func(){c = @a; @b = c;1;};
func();
puti(b);
