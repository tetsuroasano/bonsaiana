#include "SnapIO.hpp"
#include <chrono>
#include <omp.h>


namespace snapio {
	SnapIO::SnapIO() {}

	double SnapIO::read(
			const int rank, const MPI_Comm &comm,
			const std::vector<std::shared_ptr<BonsaiIO::DataTypeBase>> &data, 
			BonsaiIO::Core &in, 
			const int reduce, 
			const bool restartFlag = true)
	{
		double dtRead = 0;
		for (auto &type : data)
	 	{
			auto t0 = std::chrono::system_clock::now();
			if (rank == 0)
				fprintf(stderr, " Reading %s ...\n", type->getName().c_str());
			if (in.read(*type, restartFlag, reduce))
			{
				long long int nLoc = type->getNumElements();
				//long long int nGlb;
				//MPI_Allreduce(&nLoc, &nGlb, 1, MPI_DOUBLE, MPI_SUM, comm);
				if (rank == 0)
				{
					fprintf(stderr, " Read %lld of type %s\n",
							nLoc, type->getName().c_str());
					fprintf(stderr, " ---- \n");
				}
			} 
			else if (rank == 0)
			{
				fprintf(stderr, " %s  is not found, skipping\n", type->getName().c_str());
				fprintf(stderr, " ---- \n");
			}

			auto t1 = std::chrono::system_clock::now();
			dtRead += std::chrono::duration_cast<std::chrono::seconds>(t1 - t0).count();
		}

		return dtRead;
	} // read
	

	double SnapIO::write(
      const int rank, const MPI_Comm &comm,
      //const std::vector<BonsaiIO::DataTypeBase*> &data,
			const std::vector<std::shared_ptr<BonsaiIO::DataTypeBase>> &data, 
      BonsaiIO::Core &out)
  {
    double dtWrite = 0;
    for (const auto &type : data)
    {
			auto t0 = std::chrono::system_clock::now();
      if (rank == 0)
        fprintf(stderr, " Writing %s ... \n", type->getName().c_str());
      long long int nLoc = type->getNumElements();
      //long long int nGlb;
      //MPI_Allreduce(&nLoc, &nGlb, 1, MPI_DOUBLE, MPI_SUM, comm);
      if (nLoc > 0)
      {   
        if (rank == 0)
          fprintf(stderr, " Writing %lld of type %s\n",
              nLoc, type->getName().c_str());
        //assert(out.write(*type));
				out.write(*type);
        if (rank == 0)
          fprintf(stderr, " ---- \n");
      }   
      else if (rank == 0)
      {   
        fprintf(stderr, " %s is empty... not writing \n", type->getName().c_str());
        fprintf(stderr, " ---- \n");
      }   
			auto t1 = std::chrono::system_clock::now();
			dtWrite += std::chrono::duration_cast<std::chrono::seconds>(t1 - t0).count();
    }
  
    return dtWrite;
  }


	void SnapIO::read_bonsai(
			const std::string fileName,
			const int reduceDM,
			const int reduceS,
			std::vector<long long> &idDM,
			std::vector<int> &typeDM,
			std::vector<float> &massDM,
			std::vector<float> &posDM,
			std::vector<float> &velDM,
			std::vector<long long> &idS,
			std::vector<int> &typeS,
			std::vector<float> &massS,
			std::vector<float> &posS,
			std::vector<float> &velS
			)
	{
		int n_rank = 1, rank = 0, comm = 1;
		auto t0 = std::chrono::system_clock::now();
		BonsaiIO::Core in(rank, n_rank, comm, BonsaiIO::READ,  fileName);
		auto t1 = std::chrono::system_clock::now();
		double dtOpen = std::chrono::duration_cast<std::chrono::seconds>(t1 - t0).count();
		if (rank == 0) in.getHeader().printFields();

		double dtRead = 0.0;
		if (reduceDM > 0)
		{
			typedef float float4[4];
			typedef float float3[3];
			long long n = 0;
			// ID Type
			{
				auto idt  = std::make_shared<BonsaiIO::DataType<IDType>>("DM:IDType");
				std::vector<std::shared_ptr<BonsaiIO::DataTypeBase>> dataDM;
				dataDM.push_back(idt);
				dtRead += read(rank, comm, dataDM, in, reduceDM);
				n = idt->size();
				idDM.resize(n);
				typeDM.resize(n);
#pragma omp parallel for
				for (size_t i = 0; i < n; i++)
				{
					idDM[i] = idt->operator[](i).getID();
					typeDM[i] = idt->operator[](i).getType();
				}
			}
			// Pos Mass
			{
				std::vector<std::shared_ptr<BonsaiIO::DataTypeBase>> dataDM;
				auto pos  = std::make_shared<BonsaiIO::DataType<float4>>("DM:POS:real4");
				dataDM.push_back(pos);
				dtRead += read(rank, comm, dataDM, in, reduceDM);
				massDM.resize(n);
				posDM.resize(3*n);
#pragma omp parallel for
				for (size_t i = 0; i < n; i++)
				{
					massDM[i] = pos->operator[](i)[3];
					for (int j = 0; j < 3; j++) 
					{
						posDM[3*i + j] = pos->operator[](i)[j];
					}
				}
			}
			// Vel
			{
				std::vector<std::shared_ptr<BonsaiIO::DataTypeBase>> dataDM;
				auto vel  = std::make_shared<BonsaiIO::DataType<float3>>("DM:VEL:float[3]");
				dataDM.push_back(vel);
				dtRead += read(rank, comm, dataDM, in, reduceDM);
				velDM.resize(3*n);
#pragma omp parallel for
				for (size_t i = 0; i < n; i++)
				{
					for (int j = 0; j < 3; j++) 
					{
						velDM[3*i + j] = vel->operator[](i)[j];
					}
				}
			}

			/*
			auto idt  = std::make_shared<BonsaiIO::DataType<IDType>>("DM:IDType");
			auto pos  = std::make_shared<BonsaiIO::DataType<float4>>("DM:POS:real4");
			auto vel  = std::make_shared<BonsaiIO::DataType<float3>>("DM:VEL:float[3]");

			std::vector<std::shared_ptr<BonsaiIO::DataTypeBase>> dataDM;
			dataDM.push_back(idt);
			dataDM.push_back(pos);
			dataDM.push_back(vel);
			dtRead += read(rank, comm, dataDM, in, reduceDM);

			auto t0 = std::chrono::system_clock::now();

			const auto n = idt->size();
			idDM.resize(n);
			typeDM.resize(n);
			massDM.resize(n);
			posDM.resize(3*n);
			velDM.resize(3*n);
#pragma omp parallel for
			for (size_t i = 0; i < n; i++)
			{
				idDM[i] = idt->operator[](i).getID();
				typeDM[i] = idt->operator[](i).getType();
				massDM[i] = pos->operator[](i)[3];
				for (int j = 0; j < 3; j++) {
					posDM[3*i + j] = pos->operator[](i)[j];
					velDM[3*i + j] = vel->operator[](i)[j];
				}
			}
			*/

			auto t1 = std::chrono::system_clock::now();
			dtRead += std::chrono::duration_cast<std::chrono::seconds>(t1 - t0).count();

		}

		if (reduceS > 0)
		{
			typedef float float4[4];
			typedef float float3[3];
			auto idt  = std::make_shared<BonsaiIO::DataType<IDType>>("Stars:IDType");
			auto pos  = std::make_shared<BonsaiIO::DataType<float4>>("Stars:POS:real4");
			auto vel  = std::make_shared<BonsaiIO::DataType<float3>>("Stars:VEL:float[3]");

			std::vector<std::shared_ptr<BonsaiIO::DataTypeBase>> dataS;
			dataS.push_back(idt);
			dataS.push_back(pos);
			dataS.push_back(vel);
			dtRead += read(rank, comm, dataS, in, reduceS);

			auto t0 = std::chrono::system_clock::now();

			const auto n = idt->size();
			idS.resize(n);
			typeS.resize(n);
			massS.resize(n);
			posS.resize(3*n);
			velS.resize(3*n);
#pragma omp parallel for
			for (size_t i = 0; i < n; i++)
			{
				idS[i] = idt->operator[](i).getID();
				typeS[i] = idt->operator[](i).getType();
				massS[i] = pos->operator[](i)[3];
				for (int j = 0; j < 3; j++) {
					posS[3*i + j] = pos->operator[](i)[j];
					velS[3*i + j] = vel->operator[](i)[j];
				}
			}

			auto t1 = std::chrono::system_clock::now();
			dtRead += std::chrono::duration_cast<std::chrono::seconds>(t1 - t0).count();

		}

		double readBW = in.computeBandwidth();

		t0 = std::chrono::system_clock::now();
		in.close();
		t1 = std::chrono::system_clock::now();
		double dtClose = std::chrono::duration_cast<std::chrono::seconds>(t1 - t0).count();

		if (rank == 0)
		{
			fprintf(stderr, "dtOpen = %g sec \n", dtOpen);
			fprintf(stderr, "dtRead = %g sec \n", dtRead);
			fprintf(stderr, "dtClose= %g sec \n", dtClose);
			fprintf(stderr, "Read BW= %g MB/s \n", readBW/1e6);
		}


	}// read_bondai

void SnapIO::write_bonsai(
					std::vector<long long> &idDM,
					std::vector<int> &typeDM,
					std::vector<float> &massDM,
					std::vector<float> &posDM,
					std::vector<float> &velDM,
					std::vector<long long> &idS,
					std::vector<int> &typeS,
					std::vector<float> &massS,
					std::vector<float> &posS,
					std::vector<float> &velS,
					std::string fileName,
					float time
					)
{
	typedef float float4[4];
	typedef float float3[3];
	int n_rank = 1, rank = 0, comm = 1;
	auto t0 = std::chrono::system_clock::now();
	BonsaiIO::Core out(rank, n_rank, comm, BonsaiIO::WRITE, fileName);
	out.setTime(time);
	auto t1 = std::chrono::system_clock::now();
	double dtOpen = std::chrono::duration_cast<std::chrono::seconds>(t1 - t0).count();

	double dtWrite = 0;

	/* Write DM */
	{
		const long long numDM = idDM.size();
		std::vector<std::shared_ptr<BonsaiIO::DataTypeBase>> data; 
		/* DM:IDType */
		auto ID  = std::make_shared<BonsaiIO::DataType<IDType>>("DM:IDType", numDM);
		for (auto i = 0; i < numDM; i++)
		{
			IDType idt;
			idt.setID(idDM[i]);
			idt.setType(typeDM[i]);
			ID->operator[](i) = idt;
		}
		data.push_back(ID);

		/* DM:POS */
		auto pos  = std::make_shared<BonsaiIO::DataType<float4>>("DM:POS:real4", numDM);
		for (int i = 0; i< numDM; i++) 
		{
			pos->operator[](i)[0] = posDM[3*i + 0];
			pos->operator[](i)[1] = posDM[3*i + 1];
			pos->operator[](i)[2] = posDM[3*i + 2];
			pos->operator[](i)[3] = massDM[i];
		}
		data.push_back(pos);

		/* DM:VEL */
		auto vel  = std::make_shared<BonsaiIO::DataType<float3>>("DM:VEL:float[3]", numDM);
		for (int i = 0; i< numDM; i++) 
		{
			vel->operator[](i)[0] = velDM[3*i + 0];
			vel->operator[](i)[1] = velDM[3*i + 1];
			vel->operator[](i)[2] = velDM[3*i + 2];
		}
		data.push_back(vel);
		
		auto t0 = std::chrono::system_clock::now();
		write(rank, comm, data, out);
		auto t1 = std::chrono::system_clock::now();
		dtWrite += std::chrono::duration_cast<std::chrono::seconds>(t1 - t0).count();
	}


	/* Write Star */
	{
		const long long numS = idS.size();

		std::vector<std::shared_ptr<BonsaiIO::DataTypeBase>> data; 
		/* Stars:IDType */
		auto ID  = std::make_shared<BonsaiIO::DataType<IDType>>("Stars:IDType", numS);
		for (auto i = 0; i < numS; i++)
		{
			IDType idt;
			idt.setID(idS[i]);
			idt.setType(typeS[i]);
			ID->operator[](i) = idt;
		}
		data.push_back(ID);

		/* Stars:POS */
		auto pos  = std::make_shared<BonsaiIO::DataType<float4>>("Stars:POS:real4", numS);
		for (int i = 0; i< numS; i++) 
		{
			pos->operator[](i)[0] = posS[3*i + 0];
			pos->operator[](i)[1] = posS[3*i + 1];
			pos->operator[](i)[2] = posS[3*i + 2];
			pos->operator[](i)[3] = massS[i];
		}
		data.push_back(pos);

		/* Stars:VEL */
		auto vel  = std::make_shared<BonsaiIO::DataType<float3>>("Stars:VEL:float[3]", numS);
		for (int i = 0; i< numS; i++) 
		{
			vel->operator[](i)[0] = velS[3*i + 0];
			vel->operator[](i)[1] = velS[3*i + 1];
			vel->operator[](i)[2] = velS[3*i + 2];
		}
		data.push_back(vel);
		
		auto t0 = std::chrono::system_clock::now();
		write(rank, comm, data, out);
		auto t1 = std::chrono::system_clock::now();
		dtWrite += std::chrono::duration_cast<std::chrono::seconds>(t1 - t0).count();
	}

	double writeBW = out.computeBandwidth();

	t0 = std::chrono::system_clock::now();
	out.close();
	t1 = std::chrono::system_clock::now();
	double dtClose = std::chrono::duration_cast<std::chrono::seconds>(t1 - t0).count();

	if (rank == 0)
	{
		fprintf(stderr, "dtOpen = %g sec \n", dtOpen);
		fprintf(stderr, "dtWrite = %g sec \n", dtWrite);
		fprintf(stderr, "dtClose= %g sec \n", dtClose);
		fprintf(stderr, "write BW= %g MB/s \n", writeBW/1e6);
	}

}// write_bonsai


void SnapIO::read_tipsy(
					const std::string fileName,
					const int reduceDM,
					const int reduceS,
					std::vector<long long> &idDM,
					std::vector<int> &typeDM,
					std::vector<float> &massDM,
					std::vector<float> &posDM,
					std::vector<float> &velDM,
					std::vector<long long> &idS,
					std::vector<int> &typeS,
					std::vector<float> &massS,
					std::vector<float> &posS,
					std::vector<float> &velS
					) 
{
	tipsyIO tio;
	std::vector<ullong> idTypeDM, idTypeS;
	float time;
	tio.readFile2(fileName, reduceDM, reduceS, idTypeDM, massDM, posDM, velDM, idTypeS, massS, posS, velS, time);
	for (auto i = 0; i < idTypeDM.size(); i++)
	{
		IDType idt;
		idt.set(idTypeDM[i]);
		idDM.push_back(idt.getID());
		typeDM.push_back(idt.getType());
	}
	for (auto i = 0; i < idTypeS.size(); i++)
	{
		IDType idt;
		idt.set(idTypeS[i]);
		idS.push_back(idt.getID());
		typeS.push_back(idt.getType());
	}
};


void SnapIO::write_tipsy(
			std::vector<long long> &idDM,
			std::vector<int> &typeDM,
			std::vector<float> &massDM,
			std::vector<float> &posDM,
			std::vector<float> &velDM,
			std::vector<long long> &idS,
			std::vector<int> &typeS,
			std::vector<float> &massS,
			std::vector<float> &posS,
			std::vector<float> &velS,
			std::string fileName,
			float time)
	{
		long long numDM = idDM.size();
		long long numS = idS.size();
		std::vector<ullong> idTypeDM(numDM);
		std::vector<ullong> idTypeS(numS);
#pragma omp parallel for
		for (auto i = 0; i < numDM; i++) {
			IDType idt;
			idt.setID((uint64_t)idDM[i]);
			idt.setType(typeDM[i]);
			idTypeDM[i] = idt.get();
		}
#pragma omp parallel for
		for (auto i = 0; i < numS; i++) {
			IDType idt;
			idt.setID((uint64_t)idS[i]);
			idt.setType(typeS[i]);
			idTypeS[i] = idt.get();
		}
	tipsyIO tio;
	tio.writeFile3(idTypeDM, massDM, posDM, velDM, idTypeS, massS, posS, velS, fileName, time);
/*
		std::vector<std::vector<float>> posDM2D(numDM, std::vector<float>(3));
		std::vector<std::vector<float>> velDM2D(numDM, std::vector<float>(3));
		std::vector<std::vector<float>> posS2D(numS, std::vector<float>(3));
		std::vector<std::vector<float>> velS2D(numS, std::vector<float>(3));
#pragma omp parallel for
		for (auto i = 0; i < numDM; i++) {
			posDM2D[i][0] = posDM[3*i];
			posDM2D[i][1] = posDM[3*i + 1];
			posDM2D[i][2] = posDM[3*i + 2];
			velDM2D[i][0] = velDM[3*i];
			velDM2D[i][1] = velDM[3*i + 1];
			velDM2D[i][2] = velDM[3*i + 2];
		}
#pragma omp parallel for
		for (auto i = 0; i < numS; i++) {
			posS2D[i][0] = posS[3*i];
			posS2D[i][1] = posS[3*i + 1];
			posS2D[i][2] = posS[3*i + 2];
			velS2D[i][0] = velS[3*i];
			velS2D[i][1] = velS[3*i + 1];
			velS2D[i][2] = velS[3*i + 2];
		}

		tipsyIO tio;
		tio.writeFile2(idTypeDM, massDM, posDM2D, velDM2D, idTypeS, massS, posS2D, velS2D, fileName, time);
*/
	}

}// namespace snapio
