from setuptools import setup, Extension
from Cython.Build import cythonize
from numpy import get_include

ext_modules = [
    Extension(
        "bonsaiana.bonsaiana",
        sources=[
            "bonsaiana/bonsaiana.pyx",
            "cpp_library/SnapIO.cpp",
            "cpp_library/tipsyIO.cpp",
        ],
        include_dirs=[get_include()],
        language="c++",
        extra_compile_args=["-std=c++11", "-fopenmp"],
        extra_link_args=["-fopenmp"]
    )
]

setup(
    ext_modules=cythonize(ext_modules)
)
