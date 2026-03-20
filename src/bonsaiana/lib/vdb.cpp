#include "SnapIO.hpp"
#include <openvdb/openvdb.h>
#include <string>
#include <iostream>
#include <cmath>
#include <openvdb/tools/PointConversion.h>

using namespace openvdb;

struct Particle {
    openvdb::Vec3d position;
    double density;
    // Add other attributes as needed (e.g., color, velocity, etc.).
};

void createVDBFromParticles(const std::vector<Particle>& particles, double voxelSize, const std::string& outputFilename) {
    // Create an empty VDB grid with a specified voxel size
    math::Transform::Ptr transform = math::Transform::createLinearTransform(voxelSize);
    tools::PointAttributeVector<math::Vec3d> points;
    tools::PointAttributeVector<float> densities;
    openvdb::CoordBBox bbox;

    // Populate the VDB grid with particle data
    for (const Particle& particle : particles) {
        points.push_back(particle.position);
        densities.push_back(static_cast<float>(particle.density));
        // Update the bounding box to encompass all particle positions
        bbox.expand(openvdb::Coord(particle.position.x(), particle.position.y(), particle.position.z()));
    }

    // Create a VDB grid from particle data
    openvdb::FloatGrid::Ptr grid = tools::createPointDensityGrid(points, densities, *transform, voxelSize, bbox);

    // Save the VDB grid to a file
    openvdb::io::File file(outputFilename);
    file.write(grid);
    file.close();
}



	template<class GridType>
void populateGrid(GridType& posgrid, long long Npar, std::vector<float> pos)
{
	using ValueT = typename GridType::ValueType;
	typename GridType::Accessor posaccessor = posgrid.getAccessor();

	openvdb::Coord xyz;
	int& x = xyz[0];
	int& y = xyz[1];
	int& z = xyz[2];
	double dx = 0.25;
	double dy = 0.25;
	double dz = 0.25;
	double x0 = -15;
	double x1 = 15;
	double y0 = -15;
	double y1 = 15;
	double z0 = -15;
	double z1 = 15;
	int Nx = int((x1-x0)/dx);
	int Ny = int((y1-y0)/dy);
	int Nz = int((z1-z0)/dx);
	std::cout << Nx << std::endl;
	int hist[Nx][Ny][Nz];
	for (int i = 0; i < Nx; i++) {
		for (int j = 0; j < Ny; j++) {
			for (int k = 0; k < Nz; k++) {
				hist[i][j][k] = 0;
			}
		}
	}
	for (long long i = 0; i < Npar; i++) {
		int ix = int((pos[3*i + 0]-x0)/dx);
		if (ix < 0 || ix >= Nx) continue;
		int iy = int((pos[3*i + 1]-y0)/dy);
		if (iy < 0 || iy >= Ny) continue;
		int iz = int((pos[3*i + 2]-z0)/dz);
		if (iz < 0 || iz >= Nz) continue;
		hist[ix][iy][iz] += 1;
	}
	for (int i = 0; i < Nx; i++) {
		for (int j = 0; j < Ny; j++) {
			for (int k = 0; k < Nz; k++) {
				x = i;
				y = j;
				z = k;
				ValueT out = ValueT(log(hist[i][j][k]));
				posaccessor.setValue(xyz, out);
			}
		}
	}

}


int main(int argc, char* argv[])
{

	if (argc != 3) {
		std::cout << "fileInBonsai  reduce" << std::endl;
		return 1;
	}
	const std::string fileInBonsai (argv[1]);
	const int reduce = std::atoi(argv[2]);

	std::vector<long long> idDM;
	std::vector<int> typeDM;
	std::vector<float> massDM;
	std::vector<float> posDM;
	std::vector<float> velDM;

	std::vector<long long> idS;
	std::vector<int> typeS;
	std::vector<float> massS;
	std::vector<float> posS;
	std::vector<float> velS;
	snapio::SnapIO::read_bonsai(fileInBonsai, 0, reduce, idDM, typeDM, massDM, posDM, velDM, idS, typeS, massS, posS, velS);

	openvdb::initialize();
	openvdb::FloatGrid::Ptr posgrid = openvdb::FloatGrid::create(0.0);
	populateGrid(*posgrid, idS.size(), posS);
	posgrid->setGridClass(openvdb::GRID_FOG_VOLUME);
	posgrid->setName("density");
	std::string filename = "qm.vdb";
	openvdb::io::File file(filename);
	openvdb::GridPtrVec grids;
	grids.push_back(posgrid);
	file.write(grids);
	file.close();

	return 0;
}
