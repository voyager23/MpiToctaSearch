# Script.awk
BEGIN {FS = "[ (),]" }
/Total:/ {
	r = $3;
	i = $4;
	m = sqrt(r*r + i*i);
	print $3,$4,m,$7;	# real imag magnitude solns
	}
END {}

