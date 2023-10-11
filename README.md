# bonsaiana
Python package for the analyzing N-body simulation data with  [Bonsai](https://github.com/treecode/Bonsai) code.

# Install
```
git clone https://github.com/tetsuroasano/bonsaiana
cd bonsaiana
pip install -r requirements.txt
pip install -e .
```

# Basic Usage
```
import bonsaiana
pid, ptype, mass, pos, vel = bonsaiana.IO.read('snapshot_name.bonsai')
```
`bonsai.IO.read()` returns five `numpy.arrays` (particles' ID, type, mass, position, and velocity).


