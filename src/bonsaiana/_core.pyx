import cython
cimport cython
import numpy as np
cimport numpy as np
from libcpp.vector cimport vector
from libcpp.string cimport string
from libc.stdlib cimport malloc, free

# --- VectorWrapper logic ---

cdef class VectorWrapper:
    cdef Py_ssize_t shape[2]
    cdef Py_ssize_t strides[2]
    cdef Py_ssize_t itemsize

    def __cinit__(self):
        pass

    def __dealloc__(self):
        pass

    cdef void update_size(self):
        self.shape[0] = self.vec_size()
        self.strides[0] = <Py_ssize_t> self.itemsize

    cdef void set_buffer(self, Py_buffer *buffer):
        pass

    def __getbuffer__(self, Py_buffer *buffer, int flags):
        self.update_size()
        self.set_buffer(buffer)
        buffer.len = self.vec_size() * self.itemsize
        buffer.readonly = 0
        buffer.ndim = 1
        buffer.shape = self.shape
        buffer.strides = self.strides
        buffer.suboffsets = NULL
        buffer.itemsize = self.itemsize
        buffer.internal = NULL
        buffer.obj = self

    def __releasebuffer__(self, Py_buffer *buffer):
        pass

cdef class VectorInt(VectorWrapper):
    cdef vector[int] vec
    def __cinit__(self):
        self.itemsize = sizeof(int)
    def vec_size(self):
        return self.vec.size()
    cdef void set_buffer(self, Py_buffer* buffer):
        buffer.buf = <void*>(self.vec.data())
        buffer.format = 'i'

cdef class VectorLLong(VectorWrapper):
    cdef vector[long long] vec
    def __cinit__(self):
        self.itemsize = sizeof(long long)
    def vec_size(self):
        return self.vec.size()
    cdef void set_buffer(self, Py_buffer* buffer):
        buffer.buf = <void*>(self.vec.data())
        buffer.format = 'q'

cdef class VectorFloat(VectorWrapper):
    cdef vector[float] vec
    def __cinit__(self):
        self.itemsize = sizeof(float)
    def vec_size(self):
        return self.vec.size()
    cdef void set_buffer(self, Py_buffer* buffer):
        buffer.buf = <void*>(self.vec.data())
        buffer.format = 'f'

# --- SnapIOCython logic ---

cdef class SnapIOCython:
    @staticmethod
    def read_bonsai(string input_file, int reduceDM, int reduceS):
        cdef SnapIO snpio
        cdef VectorLLong idDM = VectorLLong()
        cdef VectorInt typeDM = VectorInt()
        cdef VectorFloat massDM = VectorFloat()
        cdef VectorFloat posDM = VectorFloat()
        cdef VectorFloat velDM = VectorFloat()
        cdef VectorLLong idS = VectorLLong()
        cdef VectorInt typeS = VectorInt()
        cdef VectorFloat massS = VectorFloat()
        cdef VectorFloat posS = VectorFloat()
        cdef VectorFloat velS = VectorFloat()

        snpio.read_bonsai(input_file, reduceDM, reduceS, idDM.vec, typeDM.vec, massDM.vec, posDM.vec, velDM.vec, idS.vec, typeS.vec, massS.vec, posS.vec, velS.vec)

        return (np.asarray(idDM), np.asarray(typeDM), np.asarray(massDM), np.asarray(posDM), np.asarray(velDM),
                np.asarray(idS), np.asarray(typeS), np.asarray(massS), np.asarray(posS), np.asarray(velS))

    @staticmethod
    def write_bonsai(vector[long long] &idDM, vector[int] &typeDM, vector[float] &massDM, vector[float] &posDM, vector[float] &velDM, vector[long long] &idS, vector[int] &typeS, vector[float] &massS, vector[float] &posS, vector[float] &velS, string output_file, float time):
        cdef SnapIO snpio
        snpio.write_bonsai(idDM, typeDM, massDM, posDM, velDM, idS, typeS, massS, posS, velS, output_file, time)

    @staticmethod
    def read_tipsy(string input_file, int reduceDM, int reduceS):
        cdef SnapIO snpio
        cdef VectorLLong idDM = VectorLLong()
        cdef VectorInt typeDM = VectorInt()
        cdef VectorFloat massDM = VectorFloat()
        cdef VectorFloat posDM = VectorFloat()
        cdef VectorFloat velDM = VectorFloat()
        cdef VectorLLong idS = VectorLLong()
        cdef VectorInt typeS = VectorInt()
        cdef VectorFloat massS = VectorFloat()
        cdef VectorFloat posS = VectorFloat()
        cdef VectorFloat velS = VectorFloat()

        snpio.read_tipsy(input_file, reduceDM, reduceS, idDM.vec, typeDM.vec, massDM.vec, posDM.vec, velDM.vec, idS.vec, typeS.vec, massS.vec, posS.vec, velS.vec)

        return (np.asarray(idDM), np.asarray(typeDM), np.asarray(massDM), np.asarray(posDM), np.asarray(velDM),
                np.asarray(idS), np.asarray(typeS), np.asarray(massS), np.asarray(posS), np.asarray(velS))

    @staticmethod
    def write_tipsy(vector[long long] &idDM, vector[int] &typeDM, vector[float] &massDM, vector[float] &posDM, vector[float] &velDM, vector[long long] &idS, vector[int] &typeS, vector[float] &massS, vector[float] &posS, vector[float] &velS, string output_file, float time):
        cdef SnapIO snpio
        snpio.write_tipsy(idDM, typeDM, massDM, posDM, velDM, idS, typeS, massS, posS, velS, output_file, time)
