nrequire "libosext"



libosext.unix();

function on.enterKey()
	libosext.unix();
	
	math.evalStr("solve(x^2-2=0,x)");
	
	
end



