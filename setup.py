from setuptools import setup, Extension, find_packages
from Cython.Build import cythonize
from Cython.Distutils import build_ext
from distutils import sysconfig
from numpy import get_include

ext_modules = [
    Extension(
        "bonsaiana", sources=[
            "./cython/bonsaiana.pyx",
            "./cpp_library/SnapIO.cpp",
            "./cpp_library/tipsyIO.cpp",
        ],
        include_dirs=[get_include()],
        language="c++",
        extra_compile_args=["-std=c++11", "-fopenmp"],
        extra_link_args=["-fopenmp"]
    )
]

setup(
    name="bonsaiana",
    version='0.1.0',
    author='Tetsuro Asano',
    cmdclass={"build_ext": build_ext},
    ext_modules=cythonize(ext_modules)
)
