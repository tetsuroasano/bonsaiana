from wurlitzer import pipes
import errno
import os
include "snap_io.pyx"

class IO:
    @staticmethod
    def read(file_name, reduce_dm=0, reduce_star=1, format='bonsai', unit_len=1., unit_vel=100., unit_mass=2.324876e9):
        """
        Read an N-body snapshot file

        Parameters
        ----------
        file_name: string
            Snapshot file name
        reduce_dm: int
            Reduce factor of the dark-matter particles
        reduce_star: int
            Reduce factor of the stellar particles
        format: {'bonsai', 'tipsy'}, default: 'bonsai'
            Format of the snapshot
        unit_len: float, default: 1.0
            Unit length. Default value: 1.0 [kpc]
        unit_vel: float, default: 100.0
            Unit velocity. Default value: 100.0 [km/s]
        unit_mass: float, default: 2.324876e9
            Unit mass. Default value: 2.324876e9 [solar mass]

        Returns
        -------
        id_dm: numpy.ndarray
            IDs of dark-matter particles
        type_dm: numpy.ndarray
            Types of dark-matter particles
        mass_dm: numpy.ndarray
            Masses of dark-matter particles
        pos_dm: numpy.ndarray
            Positions of dark-matter particles
        vel_dm: numpy.ndarray
            Velocities of dark-matter particles
        id_s: numpy.ndarray
            IDs of stellar particles
        type_s: numpy.ndarray
            Types of stellar particles
        mass_s: numpy.ndarray
            Masses of stellar particles
        pos_s: numpy.ndarray
            Positions of stellar particles
        vel_s: numpy.ndarray
            Velocities of stellar particles

        """

        if not os.path.isfile(file_name):
            raise FileNotFoundError(errno.ENOENT,
                                    os.strerror(errno.ENOENT),
                                    file_name)
        
        if format == 'bonsai':

            with pipes() as (out, err):
                id_dm, type_dm, mass_dm, pos_dm, vel_dm, id_s, type_s, mass_s, pos_s, vel_s = SnapIOCython.read_bonsai(file_name.encode('utf-8'), reduce_dm, reduce_star)
            pos_dm *= unit_len
            vel_dm *= unit_vel
            mass_dm *= unit_mass * reduce_dm
            pos_dm = pos_dm.reshape([-1,3])
            vel_dm = vel_dm.reshape([-1,3])
            pos_s *= unit_len
            vel_s *= unit_vel
            mass_s *= unit_mass * reduce_star
            pos_s = pos_s.reshape([-1,3])
            vel_s = vel_s.reshape([-1,3])

            if reduce_dm == 0 and reduce_star == 0:
                return ()
            elif reduce_dm == 0:
                return id_s, type_s, mass_s, pos_s, vel_s
            elif reduce_star == 0:
                return id_dm, type_dm, mass_dm, pos_dm, vel_dm
            else:
                return id_dm, type_dm, mass_dm, pos_dm, vel_dm, id_s, type_s, mass_s, pos_s, vel_s

        elif format == 'tipsy':
            id_dm, type_dm, mass_dm, pos_dm, vel_dm, id_s, type_s, mass_s, pos_s, vel_s = SnapIOCython.read_tipsy(file_name.encode('utf-8'), reduce_dm, reduce_star)
            pos_dm *= unit_len
            vel_dm *= unit_vel
            mass_dm *= unit_mass * reduce_dm
            pos_dm = pos_dm.reshape([-1,3])
            vel_dm = vel_dm.reshape([-1,3])
            pos_s *= unit_len
            vel_s *= unit_vel
            mass_s *= unit_mass * reduce_star
            pos_s = pos_s.reshape([-1,3])
            vel_s = vel_s.reshape([-1,3])

            if reduce_dm == 0 and reduce_star == 0:
                return ()
            elif reduce_dm == 0:
                return id_s, type_s, mass_s, pos_s, vel_s
            elif reduce_star == 0:
                return id_dm, type_dm, mass_dm, pos_dm, vel_dm
            else:
                return id_dm, type_dm, mass_dm, pos_dm, vel_dm, id_s, type_s, mass_s, pos_s, vel_s

        else:
            raise Exception(f'{format}: unsupported file format')

    @staticmethod
    def write(id_dm, type_dm, mass_dm, pos_dm, vel_dm, id_s, type_s, mass_s, pos_s, vel_s, file_name, time=0., format='tipsy', unit_len=1., unit_vel=100., unit_mass=2.324876e9):
        """
        Write N-body data in a file

        Parameters
        ----------
        id_dm: numpy.ndarray
            IDs of dark-matter particles
        type_dm: numpy.ndarray
            Types of dark-matter particles
        mass_dm: numpy.ndarray
            Masses of dark-matter particles
        pos_dm: numpy.ndarray
            Positions of dark-matter particles
        vel_dm: numpy.ndarray
            Velocities of dark-matter particles
        id_s: numpy.ndarray
            IDs of stellar particles
        type_s: numpy.ndarray
            Types of stellar particles
        mass_s: numpy.ndarray
            Masses of stellar particles
        pos_s: numpy.ndarray
            Positions of stellar particles
        vel_s: numpy.ndarray
            Velocities of stellar particles
        file_name: string
            Snapshot file name
        time: float, default: 0
            Time of the snapshot
        format: {'bonsai', 'tipsy'}, default: 'tipsy'
            Format of the snapshot
        unit_len: float, default: 1.0
            Unit length. Default value: 1.0 [kpc]
        unit_vel: float, default: 100.0
            Unit velocity. Default value: 100.0 [km/s]
        unit_mass: float, default: 2.324876e9
            Unit mass. Default value: 2.324876e9 [solar mass]
        """

        pos_dm /= unit_len
        vel_dm /= unit_vel
        mass_dm /= unit_mass 
        pos_s /= unit_len
        vel_s /= unit_vel
        mass_s /= unit_mass

        if format == 'bonsai':
            SnapIOCython.write_bonsai(id_dm.data, type_dm.data, mass_dm.data, pos_dm.flatten().data, vel_dm.flatten().data,
                    id_s.data, type_s.data, mass_s.data, pos_s.flatten().data, vel_s.flatten().data, file_name.encode('utf-8'), time)
            pos_dm *= unit_len
            vel_dm *= unit_vel
            mass_dm *= unit_mass
            pos_s *= unit_len
            vel_s *= unit_vel
            mass_s *= unit_mass

        elif format == 'tipsy':
            SnapIOCython.write_tipsy(id_dm.data, type_dm.data, mass_dm.data, pos_dm.flatten().data, vel_dm.flatten().data,
                    id_s.data, type_s.data, mass_s.data, pos_s.flatten().data, vel_s.flatten().data, file_name.encode('utf-8'), time)
            pos_dm *= unit_len
            vel_dm *= unit_vel
            mass_dm *= unit_mass
            pos_s *= unit_len
            vel_s *= unit_vel
            mass_s *= unit_mass

        else:
            raise Exception(f'{format}: unsupported file format')

