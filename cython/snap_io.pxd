#!python
#cython: language_level=3

from libcpp.vector cimport vector
from libcpp.string cimport string

cdef extern from '../cpp_library/SnapIO.hpp' namespace 'snapio':
    cdef cppclass SnapIO:
        SnapIO()

        void read_bonsai(string, int, int,  vector[long long]&, vector[int]&, vector[float]&, vector[float]&, vector[float]&, vector[long long]&i, vector[int]&, vector[float]&, vector[float]&, vector[float]&)

        void write_bonsai(vector[long long]&, vector[int] &, vector[float]&, vector[float]&, vector[float]&, vector[long long]&, vector[int]&, vector[float]&, vector[float]&, vector[float]&, string, float)

        void read_tipsy(string, int, int,  vector[long long]&, vector[int]&, vector[float]&, vector[float]&, vector[float]&, vector[long long]&, vector[int]&, vector[float]&, vector[float]&, vector[float]&)

        void write_tipsy(vector[long long]&, vector[int] &, vector[float]&, vector[float]&, vector[float]&, vector[long long]&, vector[int]&, vector[float]&, vector[float]&, vector[float]&, string, float)

