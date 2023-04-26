# TSprech 2023/04/26
import numpy as np


class LTTraceData:
    def __init__(self, raw_file, trace_name: str):
        raw_data = raw_file.get_trace(trace_name)
        self.data = np.array(raw_data)

    # @property
    # def data(self):
    #     return self.data
    #
    # @data.setter
    # def data(self, value):
    #     self._data = value

    @property
    def average(self):
        return np.average(self.data)

    @property
    def peak_to_peak(self):
        return np.ptp(self.data)

    @property
    def rms(self):
        return np.sqrt(np.mean(self.data ** 2))

    @property
    def min(self):
        return np.min(self.data)

    @property
    def max(self):
        return np.max(self.data)
