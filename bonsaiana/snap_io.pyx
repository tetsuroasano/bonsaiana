import cython
cimport cython
import numpy as np
cimport numpy as np
from libcpp.string cimport string
from libcpp.vector cimport vector

include 'vecwrpr.pyx'

cdef class SnapIOCython:
    cdef SnapIO* ptr
    def __cinit__(self):
        self.ptr = new SnapIO()

    def __deadaloc(self):
        del self.ptr

    @staticmethod
    def read_bonsai(string input_file, int reduceDM, int reduceS):
        cdef SnapIO snpio
        cdef VectorLLong idDM
        cdef VectorInt typeDM
        cdef VectorFloat massDM
        cdef VectorFloat posDM
        cdef VectorFloat velDM
        cdef VectorLLong idS
        cdef VectorInt typeS
        cdef VectorFloat massS
        cdef VectorFloat posS
        cdef VectorFloat velS
        idDM = VectorLLong()
        typeDM = VectorInt()
        massDM = VectorFloat()
        posDM = VectorFloat()
        velDM = VectorFloat()
        idS = VectorLLong()
        typeS = VectorInt()
        massS = VectorFloat()
        posS = VectorFloat()
        velS = VectorFloat()

        snpio.read_bonsai(input_file, reduceDM, reduceS, idDM.vec, typeDM.vec, massDM.vec, posDM.vec, velDM.vec, idS.vec, typeS.vec, massS.vec, posS.vec, velS.vec)

        return np.asarray(idDM), np.asarray(typeDM), np.asarray(massDM), np.asarray(posDM), np.asarray(velDM), np.asarray(idS), np.asarray(typeS), np.asarray(massS), np.asarray(posS), np.asarray(velS)

    @staticmethod
    def write_bonsai(vector[long long] &idDM, vector[int] &typeDM, vector[float] &massDM, vector[float] &posDM, vector[float] &velDM, vector[long long] &idS, vector[int] &typeS, vector[float] &massS, vector[float] &posS, vector[float] &velS, string output_file, float time):
        cdef SnapIO snpio
        snpio.write_bonsai(idDM, typeDM, massDM, posDM, velDM, idS, typeS, massS, posS, velS, output_file, time)

    @staticmethod
    def read_tipsy(string input_file, int reduceDM, int reduceS):
        cdef SnapIO snpio
        cdef VectorLLong idDM
        cdef VectorInt typeDM
        cdef VectorFloat massDM
        cdef VectorFloat posDM
        cdef VectorFloat velDM
        cdef VectorLLong idS
        cdef VectorInt typeS
        cdef VectorFloat massS
        cdef VectorFloat posS
        cdef VectorFloat velS
        idDM = VectorLLong()
        typeDM = VectorInt()
        massDM = VectorFloat()
        posDM = VectorFloat()
        velDM = VectorFloat()
        idS = VectorLLong()
        typeS = VectorInt()
        massS = VectorFloat()
        posS = VectorFloat()
        velS = VectorFloat()

        snpio.read_tipsy(input_file, reduceDM, reduceS, idDM.vec, typeDM.vec, massDM.vec, posDM.vec, velDM.vec, idS.vec, typeS.vec, massS.vec, posS.vec, velS.vec)

        return np.asarray(idDM), np.asarray(typeDM), np.asarray(massDM), np.asarray(posDM), np.asarray(velDM), np.asarray(idS), np.asarray(typeS), np.asarray(massS), np.asarray(posS), np.asarray(velS)

    @staticmethod
    def write_tipsy(vector[long long] &idDM, vector[int] &typeDM, vector[float] &massDM, vector[float] &posDM, vector[float] &velDM, vector[long long] &idS, vector[int] &typeS, vector[float] &massS, vector[float] &posS, vector[float] &velS, string output_file, float time):
        cdef SnapIO snpio
        snpio.write_tipsy(idDM, typeDM, massDM, posDM, velDM, idS, typeS, massS, posS, velS, output_file, time)

