from .bonsaiana import IO
from .snapshot import Snapshot, ParticleGroup, to_snapshot
from .analysis import find_center_of_mass, find_center_shrinking_sphere, align_disk, calculate_angular_momentum

def to_pandas(data, contains_dm=None, contains_stars=None):
    """
    Convenience function to convert IO.read results directly to a pandas DataFrame.
    
    Hints for distinguishing between DM and stars when only 5 arrays are present:
    contains_stars=True will put the 5 arrays into stars.
    """
    return to_snapshot(data, contains_dm=contains_dm, contains_stars=contains_stars).to_pandas()
