#!/bin/perl

# x by y matrix
$x = 10;
$y = 10;

$mX = 15; # margin X in cm

$mY = 15; # margin Y in cm


# two loops to sweep all values

for(my $i; $i <= $x; $i++) {
        for(my $r = 0; $r <= $y; $r++) {
                my $hypX = getHypX($i,$r);
                my $hypY = getHypY($i,$r);
                print "$i,$r|$hypX,$hypY\n";
        }
}

sub getHypX {
        my ($i, $r) = @_;
        my $b = $mX + $i;
        my $a = $mY + ($y - $r);
        # add 0.5 to round to nearest integer
        return int(sqrt($a * $a + $b * $b) + 0.5);
}

sub getHypY {
        my ($i, $r) = @_;
    my $b = $mX + ($x - $i);
    my $a = $mY + ($y - $r);
        # add 0.5 to round to nearest interger
    return int(sqrt($a * $a + $b * $b) + 0.5);
}
