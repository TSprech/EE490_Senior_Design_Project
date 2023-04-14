# 2023/04/14 08:57:22

import setuptools # Related: https://github.com/pypa/setuptools/issues/2257
import numpy as np
import pyximport
# pyximport.install(setup_args={'include_dirs': [np.get_include()]}, pyimport = True, language_level = 3)  # https://gist.github.com/jjh42/5529718
pyximport.install(language_level = 3)

if __name__ == "__main__":
    import CySetup # 6.74568 seconds
