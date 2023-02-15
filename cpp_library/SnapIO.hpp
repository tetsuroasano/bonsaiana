#pragma once
typedef int MPI_Comm;
#include "BonsaiIO4py.h"
#include "IDType.h"
#include "tipsyIO.h"
#include <iostream>
#include <vector>
#include <string>
#include <memory>

namespace snapio {
	class SnapIO {
		private:
			static double read(
					const int rank, const MPI_Comm &comm,
					const std::vector<std::shared_ptr<BonsaiIO::DataTypeBase>> &data, 
					BonsaiIO::Core &in, 
					const int reduce, 
					const bool restartFlag);

			static double write(
					const int rank, const MPI_Comm &comm,
					const std::vector<std::shared_ptr<BonsaiIO::DataTypeBase>> &data, 
					BonsaiIO::Core &out);


		public:
			SnapIO();
			static void read_bonsai(
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
					);

			static void write_bonsai(
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
					);

			static void read_tipsy(
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
					);

			static void write_tipsy(
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
					);
	};
}
