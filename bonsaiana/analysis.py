import numpy as np

def find_center_of_mass(pos, mass, vel=None):
    """
    Calculate the center of mass position and velocity.

    Parameters
    ----------
    pos : numpy.ndarray
        Particle positions (N, 3)
    mass : numpy.ndarray
        Particle masses (N,)
    vel : numpy.ndarray, optional
        Particle velocities (N, 3)

    Returns
    -------
    cm_pos : numpy.ndarray
        Center of mass position (3,)
    cm_vel : numpy.ndarray, optional
        Center of mass velocity (3,). Returned only if vel is provided.
    """
    total_mass = np.sum(mass)
    cm_pos = np.sum(pos * mass[:, np.newaxis], axis=0) / total_mass
    if vel is not None:
        cm_vel = np.sum(vel * mass[:, np.newaxis], axis=0) / total_mass
        return cm_pos, cm_vel
    return cm_pos

def find_center_shrinking_sphere(pos, mass, vel=None, r_ini=None, reduce_factor=0.8,
                                 num_min=1000, thresh=1e-3, max_iter=30):
    """
    Find the center using the shrinking sphere method.

    Parameters
    ----------
    pos : numpy.ndarray
        Particle positions (N, 3)
    mass : numpy.ndarray
        Particle masses (N,)
    vel : numpy.ndarray, optional
        Particle velocities (N, 3)
    r_ini : float, optional
        Initial radius. If None, uses the maximum distance from the initial CM.
    reduce_factor : float, default 0.8
        Factor to reduce the radius in each iteration.
    num_min : int, default 1000
        Minimum number of particles required in the sphere.
    thresh : float, default 1e-3
        Relative convergence threshold.
    max_iter : int, default 30
        Maximum number of iterations.

    Returns
    -------
    cm_pos : numpy.ndarray
        Determined center position (3,)
    cm_vel : numpy.ndarray, optional
        Determined center velocity (3,). Returned only if vel is provided.
    """
    cm_pos = find_center_of_mass(pos, mass)
    
    if r_ini is None:
        r = np.linalg.norm(pos - cm_pos, axis=1).max() * reduce_factor
    else:
        r = r_ini
        
    old_cm = cm_pos.copy()
    for i in range(max_iter):
        dist = np.linalg.norm(pos - cm_pos, axis=1)
        mask = dist < r
        
        if np.sum(mask) < num_min:
            break
        
        cm_pos = find_center_of_mass(pos[mask], mass[mask])
        
        if np.linalg.norm(cm_pos - old_cm) / r < thresh:
            break
        
        old_cm = cm_pos.copy()
        r *= reduce_factor
        
    if vel is not None:
        dist = np.linalg.norm(pos - cm_pos, axis=1)
        mask = dist < r
        _, cm_vel = find_center_of_mass(pos[mask], mass[mask], vel[mask])
        return cm_pos, cm_vel
        
    return cm_pos

def calculate_angular_momentum(pos, vel, mass):
    """
    Calculate the total angular momentum vector.

    Parameters
    ----------
    pos : numpy.ndarray
        Particle positions (N, 3)
    vel : numpy.ndarray
        Particle velocities (N, 3)
    mass : numpy.ndarray
        Particle masses (N,)

    Returns
    -------
    L : numpy.ndarray
        Total angular momentum vector (3,)
    """
    return np.sum(mass[:, np.newaxis] * np.cross(pos, vel), axis=0)

def align_disk(pos, vel, mass=None, L=None):
    """
    Align the disk so that the angular momentum vector is along the Z-axis.

    Parameters
    ----------
    pos : numpy.ndarray
        Particle positions (N, 3)
    vel : numpy.ndarray
        Particle velocities (N, 3)
    mass : numpy.ndarray, optional
        Particle masses (N,). Required if L is not provided.
    L : numpy.ndarray, optional
        Angular momentum vector (3,). If None, calculated from (pos, vel, mass).

    Returns
    -------
    new_pos : numpy.ndarray
        Rotated positions
    new_vel : numpy.ndarray
        Rotated velocities
    """
    if L is None:
        if mass is None:
            raise ValueError("Either L or mass must be provided to align_disk.")
        L = calculate_angular_momentum(pos, vel, mass)
    
    L_norm = L / np.linalg.norm(L)
    
    # Target axis
    z_axis = np.array([0, 0, 1])
    
    if np.allclose(L_norm, z_axis):
        return pos.copy(), vel.copy()
    
    if np.allclose(L_norm, -z_axis):
        # Rotate 180 degrees around X axis
        R = np.array([[1, 0, 0], [0, -1, 0], [0, 0, -1]])
    else:
        # Rodrigues' rotation formula for rotating L_norm to z_axis
        v = np.cross(L_norm, z_axis)
        s = np.linalg.norm(v)
        c = np.dot(L_norm, z_axis)
        v_skew = np.array([[0, -v[2], v[1]],
                           [v[2], 0, -v[0]],
                           [-v[1], v[0], 0]])
        R = np.eye(3) + v_skew + np.matmul(v_skew, v_skew) * ((1 - c) / (s**2))
    
    return np.dot(pos, R.T), np.dot(vel, R.T)
