from setuptools import setup, Extension
from Cython.Build import cythonize
from numpy import get_include
import os

# New structure: Extension is bonsaiana._core
# Source files are in src/bonsaiana/
ext_modules = [
    Extension(
        "bonsaiana._core",
        sources=[
            "src/bonsaiana/_core.pyx",
            "src/bonsaiana/lib/SnapIO.cpp",
            "src/bonsaiana/lib/tipsyIO.cpp",
        ],
        include_dirs=[get_include(), "src/bonsaiana/lib/"],
        language="c++",
        extra_compile_args=["-std=c++11", "-fopenmp"],
        extra_link_args=["-fopenmp"]
    )
]

setup(
    ext_modules=cythonize(ext_modules),
    package_dir={"": "src"},
)
