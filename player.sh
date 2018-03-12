#!/bin/bash

if [ $# -lt 1 ]; then
	echo "player.sh <16b | 16k | 8k | 18k> <audiofile(s)> "
	exit
fi

case "$1" in
	"16b")
	bitrate=8000
	samples=16
	;;

	"16k")
	bitrate=16000
	samples=8
	;;

	"18k")
	bitrate=18000
	samples=8
	;;

	"8k")
	bitrate=8000
	samples=8
	;;
	
	*)
	echo "Specify the audio format as a first argument <16b | 16k | 8k | 18k>"
	exit
	;;
esac

shift
TMP_WAV=tmp_file.wav

echo "Will convert files [$*]"
sox --magic -S -V3 "$*" -G -b $samples -c 1 -r $bitrate "$TMP_WAV"
sox_es=$?

if [ $sox_es -ne 0 ]; then
	echo "File conversion failed. Status: [$sox_es]"
	exit
fi

time ./wpwm.pl "$TMP_WAV"
rm -rfv "$TMP_WAV"
