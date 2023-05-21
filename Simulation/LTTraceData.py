# TSprech 2023/04/26
import numpy as np


class LTTraceData:
    def __init__(self, raw_file, trace_name: str, invert = False):
        raw_data = raw_file.get_trace(trace_name)
        self.data = np.array(raw_data)
        self.invert = -1 if invert else 1

    # @property
    # def data(self):
    #     return self.data
    #
    # @data.setter
    # def data(self, value):
    #     self._data = value

    @property
    def average(self):
        return np.average(self.data) * self.invert

    @property
    def peak_to_peak(self):
        return np.ptp(self.data) * self.invert

    @property
    def rms(self):
        return np.sqrt(np.mean(self.data ** 2)) * self.invert

    @property
    def min(self):
        return np.min(self.data) * self.invert

    @property
    def max(self):
        return np.max(self.data) * self.invert
