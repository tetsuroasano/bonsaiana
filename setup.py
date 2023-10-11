from setuptools import setup, Extension, find_packages
from Cython.Build import cythonize
from Cython.Distutils import build_ext
from distutils import sysconfig
from numpy import get_include


def _requires_from_file(filename):
    return open(filename).read().splitlines()


ext_modules = [
    Extension(
        "bonsaiana", sources=[
            "./bonsaiana/bonsaiana.pyx",
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
    ext_modules=cythonize(ext_modules),
    install_requires=_requires_from_file('requirements.txt')
)
