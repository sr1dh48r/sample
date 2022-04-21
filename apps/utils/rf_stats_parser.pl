#!/usr/bin/perl
#use strict;
use warnings;
use integer;

$num_args = $#ARGV + 1;

if ($num_args != 2) {
    print "Usage: $0 <calibration data file> <calibration format file>\n";
    exit;
}

my $index = 0;
my ($filename, $format_filename) = @ARGV;
my @format;
open(my $fh, '<', $filename)
  or die "Could not open file '$filename' $!";
open(my $fh_format, '<', $format_filename)
  or die "Could not open file '$format_filename' $!";

##format file processing
print "Reading from Format File: $format_filename\n";
while (my $row = <$fh_format>) {
  chomp $row;
  my @arr;
  $format[$index] = ();
  @arr= split(":", $row);
  $format[$index] = \@arr;
  $index++;
}

###Actual file processing.
print "Reading from RF stats File: $filename\n";
my $index_format = 0;
binmode $fh;
while (<>) {
  my @bytes;
  my $byte, my $tmp;
  #Sanity Check
  if ($index_format == $index )
  {
	  last;
  }

  #Each nibble is a byte
  read($fh, $byte, $format[$index_format][2] * 2);
  chomp $byte;
  $byte =~ s/\R//g;

  if ($byte ne "") 
  {
	  $tmp = $byte;
	  #Endianess and Sign bit conversion: Nasty way
	  #TODO: Try unpack/pack (touch)
	  if ($format[$index_format][2] * 2 == 2) 
	  {
		 if (($byte !~ m/gain/i) and hex($byte) & 0x80) 
		 {
			$byte = hex ($byte)-256;
		 } else 
		 {
			$byte = hex ($byte);
		 }
	  } elsif ($format[$index_format][2] * 2 == 4) 
	  {
		 $byte =~ s/(\w\w)(\w\w)/$2$1/g;
		 $tmp = $byte;
		 if (($byte !~ m/gain/i) and hex($byte) & 0x8000) 
		 {
			$byte = hex ($byte)-65536;
		 } else 
		 {
			$byte = hex ($byte);
		 }
	  }
	  printf("%-10s = %d\n",$format[$index_format][1], $byte);       
  }
  $index_format++;
}
__END__

=head1 Title: 

Perl Script to Parse RF Calibration Data sent to host from RPU statistic, this is sent on every working channel programming.
The idea was to debug any calibration realted issues without the need of JTAG. A simple analysis of the output should give a rough
idea about the validity of RF calibration.

=over 1

=item 1 Description:


	This script takes 2 arguments, and returns a parsed outout of rf_stats 	in a signed decimal format.

=item 2 Arguments


	Argument1: Copy only the hex output of rf_calib_data from command "echo get_stats=1> /proc/uccp420/params; cat /proc/uccp420/phy_stats"

	Argument2: Format File, which defines how to parse in the format of tuple (offset, description, length): rf_stats_format.txt

=back
