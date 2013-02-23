#!/usr/bin/perl
@elements;
$i = 0;
while(defined($in = <STDIN>))
{
	chomp $in;
	$elements[$i] = $in;
	$i++;
}
	
@sorted = sort { length($a) <=> length($b) } @elements;
print "@sorted";
print "\n";
