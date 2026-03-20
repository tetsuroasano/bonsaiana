#include "SnapIO.hpp"
#include <chrono>
#include <cstdlib>
#include <string>


int main(int argc, char* argv[]) {

	if (argc != 5 && argc != 6) {
		std::cout << "fileInBonsai fileInTipsy fileOutTipsyBase reduce nProcs (=1)" << std::endl;
		return 1;
	}
  const std::string fileInBonsai (argv[1]);
  const std::string fileInTipsy (argv[2]);
  const std::string fileOutTipsyBase (argv[3]);
	const int reduce = std::atoi(argv[4]);
	int nProcs = 1;
	if (argc == 6)  nProcs =  std::atoi(argv[5]);

	std::cout << nProcs << std::endl;
	
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

	// MW
	snapio::SnapIO::read_bonsai(fileInBonsai, reduce, reduce, idDM, typeDM, massDM, posDM, velDM, idS, typeS, massS, posS, velS);
	std::vector<double> posCm(3);
	// Offset subtraction
	for (long long i = 0; i < idDM.size(); i++) {
		for (long long j = 0; j < 3; j++) {	
			posCm[j] += (double)posDM[3*i + j];
		}
	}
	for (long long i = 0; i < idS.size(); i++) {
		for (long long j = 0; j < 3; j++) {	
			posCm[j] += (double)posS[3*i + j];
		}
	}
	for (int i = 0; i < 3; i++) posCm[i] /= (idDM.size() + idS.size());
	for (long long i = 0; i < idDM.size(); i++) {
		for (long long j = 0; j < 3; j++) {	
			posDM[3*i + j] -= posCm[j];
		}
	}
	for (long long i = 0; i < idS.size(); i++) {
		for (long long j = 0; j < 3; j++) {	
			posS[3*i + j] -= posCm[j];
		}
	}

	// Dwarf(s) 
	snapio::SnapIO::read_tipsy(fileInTipsy, 0, reduce, idDM, typeDM, massDM, posDM, velDM, idS, typeS, massS, posS, velS);
	std::cout << "N_DM All: " <<  idDM.size() << std::endl;
	std::cout << "N_S All : " << idS.size() << std::endl;

	// mass x reduce
	for (long long i = 0; i < idDM.size(); i++) {
		massDM[i] *= reduce;
	}
	for (long long i = 0; i < idS.size(); i++) {
		massS[i] *= reduce;
	}

	// Output
	if (nProcs == 1) {
		snapio::SnapIO::write_tipsy(idDM, typeDM, massDM, posDM, velDM, idS, typeS, massS, posS, velS, fileOutTipsyBase, 0);
		return 0;
	}

	const long long deltaNumDM = (long long)idDM.size()/nProcs;
	const long long deltaNumS = (long long)idS.size()/nProcs;
	long long leftDM = 0;
	long long rightDM = deltaNumDM;
	long long leftS = 0;
	long long rightS = deltaNumS;
	std::vector<long long> idDMSub(deltaNumDM);
	std::vector<int> typeDMSub(deltaNumDM);
	std::vector<float> massDMSub(deltaNumDM);
	std::vector<float> posDMSub(3*deltaNumDM);
	std::vector<float> velDMSub(3*deltaNumDM);
	std::vector<long long> idSSub(deltaNumS);
	std::vector<int> typeSSub(deltaNumS);
	std::vector<float> massSSub(deltaNumS);
	std::vector<float> posSSub(3*deltaNumS);
	std::vector<float> velSSub(3*deltaNumS);
	for (int p = 0; p < nProcs; p++) {
		if (p == nProcs - 1) {
			rightDM = idDM.size();
			rightS = idS.size();

			idDMSub.clear();
			typeDMSub.clear();
			massDMSub.clear();
			posDMSub.clear();
			velDMSub.clear();
			idSSub.clear();
			typeSSub.clear();
			massSSub.clear();
			posSSub.clear();
			velSSub.clear();

			idDMSub.resize(rightDM - leftDM);
			typeDMSub.resize(rightDM - leftDM);
			massDMSub.resize(rightDM - leftDM);
			posDMSub.resize(3*(rightDM - leftDM));
			velDMSub.resize(3*(rightDM - leftDM));
			idSSub.resize(rightS - leftS);
			typeSSub.resize(rightS - leftS);
			massSSub.resize(rightS - leftS);
			posSSub.resize(3*(rightS - leftS));
			velSSub.resize(3*(rightS - leftS));
		}
		std::copy(idDM.begin() + leftDM, idDM.begin() + rightDM, idDMSub.begin());
		std::copy(typeDM.begin() + leftDM, typeDM.begin() + rightDM, typeDMSub.begin());
		std::copy(massDM.begin() + leftDM, massDM.begin() + rightDM, massDMSub.begin());
		std::copy(posDM.begin() + 3*leftDM, posDM.begin() + 3*rightDM, posDMSub.begin());
		std::copy(velDM.begin() + 3*leftDM, velDM.begin() + 3*rightDM, velDMSub.begin());
		std::copy(idS.begin() + leftS, idS.begin() + rightS, idSSub.begin());
		std::copy(typeS.begin() + leftS, typeS.begin() + rightS, typeSSub.begin());
		std::copy(massS.begin() + leftS, massS.begin() + rightS, massSSub.begin());
		std::copy(posS.begin() + 3*leftS, posS.begin() + 3*rightS, posSSub.begin());
		std::copy(velS.begin() + 3*leftS, velS.begin() + 3*rightS, velSSub.begin());

		std::string fileOutTipsy = fileOutTipsyBase + "-" + std::to_string(p);
		snapio::SnapIO::write_tipsy(idDMSub, typeDMSub, massDMSub, posDMSub, velDMSub, idSSub, typeSSub, massSSub, posSSub, velSSub, fileOutTipsy, 0);
		leftDM = rightDM;
		rightDM += deltaNumDM;
		leftS = rightS;
		rightS += deltaNumS;
	}

	return 0;
}
