This directory is supposed to contain first two tracks of the [TNC Test CD]
used for verifying behavior of the AFSK/AX.25 demodulator implementation.

[TNC Test CD]: (http://wa8lmf.net/TNCtest/)

Since the CD is rather big and terms of re-distributing the tracks is not very
explicit the tracks are not bundled into this repository and instead are to be
downloaded from the WA8LMF's website.

The expected file naming:
- track_01_11025.wav - Track 01 in 11025 samples per second.
- track_02_11025.wav - Track 02 in 11025 samples per second.
- track_01_44100.wav - Track 01 in 44100 samples per second.
- track_02_44100.wav - Track 02 in 44100 samples per second.

The TNC Test CD consists of the binary file (.bin) and the manifest (.cue).
In order to extract actual WAV files a [bchunk] utility can be used. The utility
is available on macOS via Homebrew.

[bchunk]: https://github.com/extramaster/bchunk

Command to extra WAV files:

```
bchunk -v -w TNC_Test_Ver-1.1.bin TNC_Test_Ver-1.1.cue track_
```

This will result in a set of files named track_01.wav, track_02.wav and so on.
They are expected to be in 44100 samples per second, so after simple rename they
can be copied to this directory.

In order to downsample 44100 to 11025 samples per second a [sox] utility can
be used.

[sox]: https://sox.sourceforge.io/

Command to resample WAV files:

```
sox track_01_44100.wav track_01_11025.wav rate 11025
sox track_02_44100.wav track_02_11025.wav rate 11025
```
