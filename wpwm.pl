#!/usr/bin/env perl


use strict;
use warnings;
$|++;


use Device::SerialPort;
use Digest::CRC qw/crc16/;
use Time::HiRes qw/usleep/;


my %g_dev = (
	dev => '/dev/ttyACM0',
	speed => '230400',
);


use constant {
	SLIP_END => 0300,
	SLIP_ESC => 0333,
	SLIP_ESC_END => 0334,
	SLIP_ESC_ESC => 0335,
};


sub slip_send {
	my $chunk = shift;

	my @data = map { 
		$_ == SLIP_END ? (SLIP_ESC, SLIP_ESC_END) : 
			($_ == SLIP_ESC ? (SLIP_ESC, SLIP_ESC_ESC) : $_) } @_ ;

	my $crc = crc16( pack "C*", (0,0, $chunk, @_));
	return pack "CSC*", (SLIP_END, $crc, $chunk, @data, SLIP_END); 
}


sub slip_send16 {
	my @data = map { 
		$_ == SLIP_END ? (SLIP_ESC, SLIP_ESC_END) : 
			($_ == SLIP_ESC ? (SLIP_ESC, SLIP_ESC_ESC) : $_) } @_ ;

	my $samples = int((scalar @_)/2);
	my $crc = crc16( pack "C*", (0,0, $samples, @_));
	return pack "CSC*", (SLIP_END, $crc, $samples, @data, SLIP_END); 
}


sub slip_recv {
	my $read_cb = shift || return;
	my @data = ();
	my $mode = 0;

	while (1) {
		my $c = 0x00;
		next unless &$read_cb(\$c);

		my $x = $c;
		unless ($mode) {
			last if ($c == SLIP_END);
			$mode = 1 && next if ($c == SLIP_ESC);
		}
		else {
			$x = ($c == SLIP_ESC_END ? SLIP_END : 
					($c == SLIP_ESC_ESC ? SLIP_ESC : $c));
			$mode = 0;
		}
		push @data, $x;
	}
	
	pack "C*", @data;
}


my $port = new Device::SerialPort($g_dev{dev}); 
$port->baudrate($g_dev{speed}); 
$port->parity("none"); 
$port->databits(8); 
$port->stopbits(1); 
$port->dtr_active(1);
$port->write_settings();
$port->purge_all;


# wait for the reset
sleep 2;


my $wav = shift || die "<wpwm.pl <8/16/18kHz 8/16bit mono wave-file>\n";
open my $g_fh, '<', $wav ||
	die "Unable to open file [$wav]\n";

binmode ($g_fh);
my $header = 0x00;
my $offset = 0x00;
die "Unable to read WAV header\n" unless($offset = read $g_fh, $header, 44);
my @header = unpack "a4la4a4ls2l2s2a4l", $header;


# verifications
die "Header is empty\n" unless scalar @header;
die "File is not a WAVE RIFF file\n"
	unless ($header[0] =~ /RIFF/ && $header[2] =~ /WAVE/);

die "File is [$header[7]] Hz/$header[10]. Should be 8000/8bit or 16000/8bit or 8000/16bit or 18000/8bit\n"
	unless (($header[7] == 16000 ||
			 $header[7] == 18000 ||
			$header[7] == 8000) && 
			($header[10] == 8 ||
			 $header[10] == 16));


# getchar wrapper for slip receive
sub serial_getc {
	my $cout = shift;
	my ($cin, $cdata) = $port->read(1);
	$$cout = unpack "C", $cdata;
	return $cin;
}


my $read = $offset;

my $wait_time = 5000;
my $chunk = 128;

if ($header[7] == 8000) {
	if ($header[10] == 16) {
		$wait_time = 2000;
		$chunk = 64;
	}
	else {
		$wait_time = 12000;
	}
}
elsif ($header[7] == 18000) {
	$wait_time = 1000;
}


while ($read) {

	my ($cin, $cdata) = $port->read(1);
	usleep($wait_time) if ($cdata =~ /W/);
	
	print "Sending data. Offset: [$offset]\n";

	my $data = 0x00;
	$read = read $g_fh, $data, 128;
	$offset += $read;

	$port->write(slip_send($chunk, unpack "C*", $data));
	until ($port->write_drain) {}
}

close ($g_fh);
