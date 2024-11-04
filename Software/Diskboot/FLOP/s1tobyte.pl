#!/usr/bin/perl

while(<>)
{
    @array = ( $_ =~ m/../g );
    if ($array[0] ne "S1") {
        next;
    }
    
    print "     fcb  ";

    for ($i = 4; $i < $#array - 1; $i++)
    {
        print "\$$array[$i],";
    }
    print "\$$array[$i]\r";

}
