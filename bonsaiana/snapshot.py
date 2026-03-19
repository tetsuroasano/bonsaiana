import numpy as np
import pandas as pd

class ParticleGroup:
    """A group of particles (e.g., DM or Stars) with their properties."""
    def __init__(self, id, type, mass, pos, vel):
        self.id = id
        self.type = type
        self.mass = mass
        self.pos = pos
        self.vel = vel

    def find_center_of_mass(self):
        from .analysis import find_center_of_mass
        return find_center_of_mass(self.pos, self.mass, self.vel)

    def find_center_shrinking_sphere(self, **kwargs):
        from .analysis import find_center_shrinking_sphere
        return find_center_shrinking_sphere(self.pos, self.mass, self.vel, **kwargs)

    def align_disk(self):
        from .analysis import align_disk
        self.pos, self.vel = align_disk(self.pos, self.vel, self.mass)

    def to_pandas(self, group_name=None):
        """Convert this particle group to a pandas DataFrame."""
        df = pd.DataFrame({
            'id': self.id,
            'type': self.type,
            'mass': self.mass,
            'x': self.pos[:, 0],
            'y': self.pos[:, 1],
            'z': self.pos[:, 2],
            'vx': self.vel[:, 0],
            'vy': self.vel[:, 1],
            'vz': self.vel[:, 2],
        })
        if group_name:
            df['group'] = group_name
        return df

class Snapshot:
    """A snapshot representing simulation data, potentially containing DM and Stars."""
    def __init__(self, dm=None, stars=None):
        self.dm = dm
        self.stars = stars

    def to_pandas(self, combine=True):
        """
        Convert the snapshot to one or more pandas DataFrames.
        
        Parameters:
        -----------
        combine: bool, default: True
            If True, returns a single DataFrame containing all particles.
            If False, returns a tuple of (dm_df, stars_df).
        """
        if not combine:
            dm_df = self.dm.to_pandas() if self.dm else None
            stars_df = self.stars.to_pandas() if self.stars else None
            return dm_df, stars_df
        
        dfs = []
        if self.dm:
            dfs.append(self.dm.to_pandas(group_name='dm'))
        if self.stars:
            dfs.append(self.stars.to_pandas(group_name='stars'))
            
        if not dfs:
            return pd.DataFrame()
        
        return pd.concat(dfs, ignore_index=True)

def to_snapshot(data, contains_dm=None, contains_stars=None):
    """
    Utility to convert raw tuple data from IO.read into a Snapshot object.
    
    If 5 arrays are provided, it defaults to DM unless contains_stars=True
    or contains_dm=False.
    """
    if len(data) == 5:
        group = ParticleGroup(*data)
        # If we have a hint that it's stars, or that it's NOT DM
        if contains_stars is True or contains_dm is False:
            return Snapshot(stars=group)
        else:
            return Snapshot(dm=group)
    elif len(data) == 10:
        dm_group = ParticleGroup(*data[:5])
        stars_group = ParticleGroup(*data[5:])
        return Snapshot(dm=dm_group, stars=stars_group)
    elif len(data) == 0:
        return Snapshot()
    else:
        raise ValueError(f"Invalid snapshot data format. Expected 5 or 10 arrays, got {len(data)}.")
