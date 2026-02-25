# Copyright (c) 2025 radio core authors
#
# SPDX-License-Identifier: MIT-0

import numpy as np
import soundfile as sf
from pathlib import Path


class WavReader:
    """
    WAV file reader which allows to access frames in a streamed manner
    """

    _sound_file: sf.SoundFile

    def __init__(self, filename: Path):
        self._sound_file = sf.SoundFile(filename)

    def get_num_channels(self) -> int:
        """Get the number of channels in the WAV file"""
        return self._sound_file.channels

    def get_num_samples(self) -> int:
        """Get the number of samples in the WAV file"""
        return self._sound_file.frames

    def get_sample_rate(self) -> int:
        """
        Get the sample rate of the WAV file

        The sample in this context is a frame (a sample of all channels at a single point in time).
        """
        return self._sound_file.samplerate

    def get_sample_type(self) -> str:
        """Get string representation of sample type in the WAV file"""
        return f"{self._sound_file.format} {self._sound_file.subtype}"

    def get_duration_sec(self) -> float:
        """Get the WAV file duration in seconds"""
        return self.get_num_samples() / self.get_sample_rate()

    def rewind(self):
        """Rewind reader to the beginning of the file"""
        self._sound_file.seek(0, whence=sf.SEEK_SET)

    def read_num_samples(self, num_samples: int) -> np.ndarray:
        """Read the given number of samples from the WAV file"""
        return self._sound_file.read(frames=num_samples)
