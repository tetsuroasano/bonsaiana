from libcpp.vector cimport vector
from libc.stdlib cimport malloc, free

# For details, see https://stackoverflow.com/questions/45133276/passing-c-vector-to-numpy-through-cython-without-copying-and-taking-care-of-me
# and https://ymd_h.gitlab.io/ymd_blog/posts/sharing_memory_between_numpy_and_vector_by_cython/
cdef class VectorWrapper:
    cdef Py_ssize_t *shape
    cdef Py_ssize_t *strides
    cdef Py_ssize_t itemsize

    def __cinit__(self):
        self.shape   = <Py_ssize_t*>malloc(sizeof(Py_ssize_t) * 2)
        self.strides = <Py_ssize_t*>malloc(sizeof(Py_ssize_t) * 2)

    def __dealloc__(self):
        free(self.shape)
        free(self.strides)

    cdef void update_size(self):
        self.shape[0] = self.vec_size()
        self.strides[0] = <Py_ssize_t> self.itemsize

    cdef void set_buffer(self,Py_buffer *buffer):
        pass

    def __getbuffer__(self, Py_buffer *buffer, int flags):
        # relevant documentation http://cython.readthedocs.io/en/latest/src/userguide/buffer.html#a-matrix-class

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
        self.vec = vector[int]()
        self.itemsize = sizeof(int)

    def vec_size(self):
        return self.vec.size()

    cdef void set_buffer(self,Py_buffer* buffer):
        buffer.buf = <void*>(self.vec.data())
        buffer.format = 'i'

cdef class VectorLLong(VectorWrapper):
    cdef vector[long long] vec

    def __cinit__(self,value_dim=1):
        self.vec = vector['long long']()
        self.itemsize = sizeof(long long)

    def vec_size(self):
        return self.vec.size()

    cdef void set_buffer(self,Py_buffer* buffer):
        buffer.buf = <void*>(self.vec.data())
        buffer.format = 'q'

cdef class VectorFloat(VectorWrapper):
    cdef vector[float] vec

    def __cinit__(self,value_dim=1):
        self.vec = vector[float]()
        self.itemsize = sizeof(float)

    def vec_size(self):
        return self.vec.size()

    cdef void set_buffer(self,Py_buffer* buffer):
        buffer.buf = <void*>(self.vec.data())
        buffer.format = 'f'
