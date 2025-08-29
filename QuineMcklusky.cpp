#include <iostream>
#include <fstream>
#include <string.h>
#include <string>
#include <vector>
#include <iomanip>
#include <algorithm>
#include <iterator>
#include <sstream>
#include <stdio.h>
#include <stdlib.h>
#include <cstring>
#include <math.h>
using namespace std;
int MINTERM_LENGTH;	//bit ��

class inf {
public:
	vector<pair<string, string> > sortinfo(vector<pair<string, string> > min); //minterm�� ������������ �����ϴ� �Լ�
};

vector<pair<string, string> > inf::sortinfo(vector < pair<string, string> > min) //minterm�� ������������ �����ϴ� �Լ�
{
	int i, j, x, y;
	for (i = 0; i < min.size() - 1; i++)
	{
		for (j = 0; j < min.size() - 1 - i; j++)
		{
			x = count(min[j].second.begin(), min[j].second.end(), '1'); //1�� ���� ����
			y = count(min[j + 1].second.begin(), min[j + 1].second.end(), '1'); //1�� ���� ����
			if (x > y) //�տ� �ִ� ���� �ڿ� �ִ� �ͺ��� 1�� �� ���� ������ �ִٸ�
				swap(min[j], min[j + 1]); //��ġ �ٲٱ�
		}
	}
	return min; //������ �Լ��� ����
}


pair<vector<string>, bool> reduce(vector<string>, vector<string>*); 


int main() {

	vector<pair<string, string>> minterms; //���޹��� vector�� minterm. string1: dm, string2: minterm����
	pair<vector<string>, bool> reduced_minterms; //���� �ܰ迡��, minterm�� doncare ������� �׵鸸 �����Ѵ�.
	//1. starred ���� ���� ���Ѵ�. �� �ܰ迡�� before_reducing�� ����, ���׵��� PI�� �ڵ� �߰��� ���̴�.
	//2. �� reduce()�Լ� ȣ�� ������ �������� ������ 1�ܰ� �� reduce �� �׵��� �ް� �ȴ�.
	vector<string> PrimeImplicants; //����������, PI�鸸 ���̰Եȴ�.
	//1. PI�� reduce()�Լ����� �Ź� starred �� �׵��� ������ �߰��ȴ�.
	//2. �������� reduced_minterms�� bool�� false�� �Ǹ� reduced_minterms�� �����ִ� �׵��� ������ �߰��ȴ�.

	//step 1)�ؽ�Ʈ ������ ���� ����ȭ�� �� ���� �Է¹޴� �ܰ�

	char filename[30] = "input_minterm.txt"; //input ���� �̸�
	string s, ch, t; //s: ó�� ���ڿ� �޾� �ӽ÷� ����, ch: minterm or dontcare, t: 2����
	int i;
	inf x;

	ifstream file(filename); //���� �б� �������� ����
	file >> MINTERM_LENGTH; //input���Ͽ��� input bit length�б�

	getline(file, s); //���Ͽ��� �� �پ� �б�
	while (!file.eof()) //������ ���� �����ϱ� ������ �ݺ�
	{
		getline(file, s); //���Ͽ��� �� �پ� �б�
		istringstream f(s); //���ڿ� �Ľ� �Լ� 
		f >> ch >> t; //d���� m������ ch�� ����, minterms�� t�� ����
		minterms.push_back(make_pair(ch, t));
	}
	file.close();
	minterms = x.sortinfo(minterms); //�Է¹��� minterm�� �����ϴ� �Լ� ȣ���ϰ�,���ĵ� ����� �ٽ� vector�� ����

									 //dm������ ������ minterm�� reduced_minterm�� load
	for (int i = 0; i < minterms.size(); i++)
		reduced_minterms.first.push_back(minterms.at(i).second);

	//step 2) �־��� �� ���� �ĺ� ��(Prime Implicants)�� ���ϴ� �ܰ�
	do {

		reduced_minterms = reduce(reduced_minterms.first, &PrimeImplicants);
		sort(reduced_minterms.first.begin(), reduced_minterms.first.end());
		reduced_minterms.first.erase(unique(reduced_minterms.first.begin(), reduced_minterms.first.end()), reduced_minterms.first.end());
	} while (reduced_minterms.second != false);

	pair<vector<string>, bool>().swap(reduced_minterms); //deAlloc

	//step 3) �ĺ� �׵��� �̿��Ͽ� �ʼ� ��(Essential Prime Implicant)�� ���ϴ� �ܰ�
	int PIsize = PrimeImplicants.size(); 
	int minsize = minterms.size();

	//minterm �� ���� �� ���� ����
	vector<pair<string, string>>::iterator minterm_it;					//minterm iterator

	//pi<->mins ǥ�� �����ϴ� ������ ����
	vector<vector<int>> pi_to_mins_idx;									//i��°��Ҵ� PI�� �ǹ�, vector<int>�� covering�ϴ� minterms index�� ����
	vector<vector<int>> min_to_pis_idx;									//i��°��Ҵ� minterm�� �ǹ�, vector<int>�� covered������ PIs index�� ����
	vector<int>minsofpi;
	vector<int>pisofmin;

	//���������� �⺻ �����ѵ�
	for (int i = 0; i < PIsize; i++)
		pi_to_mins_idx.push_back(minsofpi);
	for (int i = 0; i < minsize; i++)
		min_to_pis_idx.push_back(pisofmin);

	vector<int>().swap(minsofpi); //minsofpi�Ҵ�����
	vector<int>().swap(pisofmin); //pisofmin�Ҵ�����

	//cover�� ���� minterm(left minterm)�� �������� �ʿ��� PI
	vector<int>	left_minterm;										//cover�ؾ��ϴ�, ���� minterm�� index����
	vector<int> needpi;												//���������� �ʿ��� PI�� ������ index����
	vector<int>::iterator l_minterm_it;								//left minterm iterator

	vector<int> left_pi;
	vector<int>::iterator l_pi_it;

	vector<int> coveringpis;											//���� �ִ� left minterm�� Ŀ�������� PI�� ����

	int max_covering_PI; // ���߿��� �� �ϳ��� �̾Ƴ���� �� PI		
	int max_coverings;													//PI�� Ŀ������ �� �ִ� ���� ���� left minterm����


	minterm_it = minterms.begin();

	//doncare �����
	while (minterm_it != minterms.end()) {
		if (minterm_it->first == "d")   // minterm_it�� �ش��ϴ� minterm ��� str1 �� "d"�� ���
			minterm_it = minterms.erase(minterm_it);								// �ش� doncare ��Ҹ� ���������� �� �ڸ��� minterm_it�� ����������	
		//
		else
			minterm_it++;
	}
	//PI -> minterm s, minterm -> PI s �� index ���� ���踦 index����ȭ�ϴ´ܰ�	

	minsize = minterms.size(); //minterm doncare ���� �� ũ�� ����	

	//cover ���� �ο� - �����ϴ� �� ������ iterator �Ⱦ��� for�� ����ص� ��	//'-'�� �ƴ� literal�� �ϳ��� ���ϸ� PI�� minterm�� cover �ϸ� üũ
	for (int i = 0; i < PIsize; i++)										//for�� �ݺ�: �� PI����, i counter
	{
		for (int j = 0; j < minsize; j++)
		{											//for�� �ݺ�: �� minterm����, j counter
			bool covers = true;											//PI�� minterm�� Ŀ���ϴ����� üũ�ϴ� ����
		//PI�� minterm cover�� ��� �ֿ� ���� ����
			for (int k = 0; k < MINTERM_LENGTH; k++) {							//literal���� üũ //for�� �ݺ�: �� literal����, k counter
				if (PrimeImplicants.at(i)[k] != '-') {				// i��° PI�� string�� k��° literal�� '-'�ƴϸ� 
					if (PrimeImplicants.at(i)[k] != minterms.at(j).second[k]) {		 //���࿡ literal�� �ϳ��� �ٸ��� cover�� false�� ������ �� �ֿ� ���ؼ� �� break
						covers = false;
						break;
					}
				}
			}
			if (covers)													//��� '-'�ƴ� literal�� ���ؼ� �����Ѱ��
			{
				pi_to_mins_idx.at(i).push_back(j);						//pi_to_minterms[i].push_back(j)�� ����.
				min_to_pis_idx.at(j).push_back(i);						//min_to_pis_idx�� minterms��ŭ �־����. 				//j��° minterm�� i��° pi�� cover���� �ǹ���
			}
		}
	}


	//left minterm pi ����Ű��						
	{
		//minterm ���θ� ���� minterm�� ����Ŵ
		for (int i = 0; i < minterms.size(); i++) {
			left_minterm.push_back(i);
		}
		//pi ���θ� ���� pi�� ����Ŵ
		for (int i = 0; i < PrimeImplicants.size(); i++) {
			left_pi.push_back(i);
		}
	}

	//EPI ã��

	{
		//vector<int>::iterator l_min_coverable_it; //EPI�� cover ������ leftminterm ����� iterator. �� ��� ������ ���� ������ ����. -> for��.
		vector<int>::iterator covered; //���༱���� pi ���� minterm����, left minterm���� �������� �ʰ� �����մٸ� �װ� �����ϱ� ���ؼ�.
		l_minterm_it = left_minterm.begin();
		while (l_minterm_it != left_minterm.end()) {

			if (min_to_pis_idx.at(*l_minterm_it).size() == 1) {


				int epi_idx = min_to_pis_idx.at(*l_minterm_it).at(0); //left minterm�� �ϳ��ۿ� �����ȵŴ� PI
				needpi.push_back(epi_idx);


				//EPI���� leftpi���� �����.
				l_pi_it = find(left_pi.begin(), left_pi.end(), epi_idx);

				l_pi_it = left_pi.erase(l_pi_it);  //iterator�� �����. 

				//���� EPI idx�� �����ϴ� minterm ��Ͽ��� �ϳ��� �ѱ��, left_minterm���� ���Ͽ� ã�´�.

				for (int i = 0; i < pi_to_mins_idx.at(epi_idx).size(); i++) {
					int now_min_crpd_pi = pi_to_mins_idx.at(epi_idx).at(i);


					covered = find(left_minterm.begin(), left_minterm.end(), now_min_crpd_pi);
					if (covered != left_minterm.end()) {	//���༱���� pi ���� minterm����, left minterm���� �������� �ʰ� �����մٸ� //left minterm �� EPI�� �����ϴ� minterm�� �������� 

						l_minterm_it = left_minterm.erase(covered); //l_minterm�� �����Ǿ����ϱ� �ش� ���� ��ȯ
					}
				}
			}
			else
				l_minterm_it++;
		}
	}

	//step 4) ���� ���� minterm�� ��Ÿ�� �� �ִ� �ĺ� �׺��� �����Ͽ�, ��� minterm�� ��Ÿ�� �� �ִ� �ĺ� �� ���� �� ���� Ʈ�������͸� ����ϴ� ������ ����� �ܰ� 
	while (left_minterm.begin() != left_minterm.end()) {							//left minterm ���Ұ� ������ �����������, ������ ����
			//left minterm Ŀ���� ������ ���� ū PI������ üũ�� ���̴�.
		max_coverings = 0;

		//left_minterm�� cover ������ PI ����� ������ coveringpis�� �ױ�
		for (int i = 0; i < left_minterm.size(); i++) {								//left minterm�� ���� -> �����ϴ� PI���� �̾Ƴ���.

			for (int j = 0; j < min_to_pis_idx.at(left_minterm.at(i)).size(); j++) {
				int now_pi_crpd_lmin = min_to_pis_idx.at(left_minterm.at(i)).at(j); //���� left min�� �����ϴ� pi ����Ʈ ��� j��°. corresponded�� ���Ӹ�
				if (find(needpi.begin(), needpi.end(), now_pi_crpd_lmin) == needpi.end()) { // PI�� needpi�� �̹� ���� pi�� �ƴ϶��
																						
					if (find(coveringpis.begin(), coveringpis.end(), now_pi_crpd_lmin) == coveringpis.end()) //now_pi_crpd_lmin�� �̹� coveringpi�� ������ �ƴϸ�
						coveringpis.push_back(now_pi_crpd_lmin); //coveringpi�� �־��ֱ�
				}
			}
		}


		//coveringpis���� �Ѱ��� ���캻��. cover ������ left minterm�� coverings�� �־�鼭 max_covering�� PI max_covering_PI ���ϱ�
		for (int i = 0; i < coveringpis.size(); i++) {												// left minterms ������� �ҷ�����
			int now_cvpi = coveringpis.at(i);
			vector<int> coverings;												//�� PI�� cover ������ left minterms���
			for (int j = 0; j < left_minterm.size(); j++) {											//coveringpis ������κҷ�����
				vector<int>::iterator findlmin_crpd_pi = find(pi_to_mins_idx.at(now_cvpi).begin(), pi_to_mins_idx.at(now_cvpi).end(), left_minterm.at(j)); //coveringpis i��° ���� pi -> pi_to_mins_idx ���� mins, ���� left_minterm.at(j)�� ã�´�. ...������ size�� �̹� ���ε�? �ȵ��ư����� ���ݾ�

				if (findlmin_crpd_pi != pi_to_mins_idx.at(now_cvpi).end()) { //left minterm�� ���� pi����mins���� ã�� ����� ���縸�ϸ�
					coverings.push_back(left_minterm.at(j)); //coverings�� left minterm �ֱ�
				}
			}
			//cout<<endl; //ForDebug
			if (coverings.size() > max_coverings) {				//�Ѱ��� covering pi�� ���ؼ�, covering������ left minterm�� ���� ����ص״� max left coverings���� ũ�ٸ�
				max_coverings = coverings.size();				//���ִ븦  coverings size�� �����ϰ�
				max_covering_PI = now_cvpi;					//���� �����ϴ� PI�� i��° covevering pi�� ����
			}
			vector<int>().swap(coverings);
		}														//���������� ���� ū covering PI�� max_covering_PI

		//���� ū covering PI�� needpi�� �ְ�, �׿� �����ϴ� left minterm ���ؼ� ����
		needpi.push_back(max_covering_PI);					
		for (int i = 0; i < max_coverings; i++) {					//max_coverings = max_covering_PI�� �ϴ� left minterm ����
			auto p = find(left_minterm.begin(), left_minterm.end(), pi_to_mins_idx.at(max_covering_PI).at(i));		//max_covering_PI�� cover ������ minterm�� ���ʷ� left minterm���� ã�Ƽ� ��������
			left_minterm.erase(p);							
		}
		//���������� ������ left minterm���� �ִ� coverings�� PI�� covering �ϴ� minterm�� �̾Ƴ��鼭 left_minterm�� 0�̵�
	}


	vector<int>().swap(left_minterm);
	vector<int>().swap(left_pi);
	vector<vector<int>>().swap(pi_to_mins_idx);									//���� �Ⱦ�, �Ҵ� ����
	vector<vector<int>>().swap(min_to_pis_idx);
	vector<int>().swap(coveringpis);

	int transistors;
	int bit;											//����ϴ� �� transistor ����, ���� bit ����
	int AND = 0;														//AND ����Ʈ�� ����ϴ� transistor ����

	ofstream fout("result.txt");


	//step 5) ���������� �� PI ���հ� ������ SOP form���� ��Ÿ���� ���� transistor ���� ��� �ܰ�
	for (int i = 0; i < needpi.size(); i++) {							//needpi ��ü�� ����
		fout << PrimeImplicants.at(needpi.at(i)) << endl;
		bit = 0;
		for (int j = 0; j < MINTERM_LENGTH; j++) {								//�츮�� ó���� PI ���� �����ڸ���
			if (PrimeImplicants.at(needpi.at(i))[j] != '-')			//needpi ù��° idx���� ������Ų PI��, j��° ���ڰ� -�� �ƴϸ�
				bit++;													//�ش� PI�� bit �� �߰�
		}
		AND = AND + 2 * bit + 2;										//�� PI�� AND ���� �� �ʿ��� ��Ʈ �� 
	}
	fout << endl;
	transistors = 2 * static_cast<unsigned long long>(MINTERM_LENGTH) + AND + (2 * needpi.size() + 2);		//�� Ʈ�������� ����
	fout << "Cost (# of transistors):" << transistors;
	
	fout.close();
	return 0;
}

pair<vector<string>, bool> reduce(vector<string> before_reducing, vector<string>* PI_GetStarred)
{
	vector<string> reduced_minterms_now; //�̹� �ܰ��� reduced minterms
	bool reducing_occured; //�̹� �ܰ迡��.��� minterm �ֵ� reduce�Ǿ�����, ���ϸ鼭 true�� �ٲܰ�. ���Ŀ� before_reducing�� pair�� ��� ��ȯ
	int pos_reducing; //bit���� ��ġ ���
	int count_reducing = 0; //bit���� ���� ���	
	bool is_minterm_reducable; //�� minterm�� ���ؼ�, �ٸ� minterm�� reduce �Ǿ��ٸ�. �� ���� minterm�� ���������� ���ؼ�

	vector<string> is_starred;
	vector<string>::iterator bef_red_it_outer;
	vector<string>::iterator bef_red_it_inner;

	//�ָ��� ���ϸ� reduce �ܰ�
	reducing_occured = false;
	bef_red_it_outer = before_reducing.begin();
	while (bef_red_it_outer != before_reducing.end())
	{
		is_minterm_reducable = false; //�� minterm�� ���ؼ� �⺻�����δ� minterm�� reducable���� �ʴٰ� �ʱ�ȭ�ؾ߉´�. //����note: ������ȉ�.

		bef_red_it_inner = before_reducing.begin();
		while (bef_red_it_inner != before_reducing.end())
		{
			if (bef_red_it_inner == bef_red_it_outer) { //�ڱ� �ڽŰ��� �� skip
				bef_red_it_inner++;
				continue;
			}
			else {

				count_reducing = 0;//�� minterm�ָ��� üũ�� pair���� 0���� �ʱ�ȭ. 
				//���� �� bit �� ���ϸ� �� minterm�� bit �ٸ� üũ
				for (int k = 0; k < MINTERM_LENGTH; k++) {
					if ((*bef_red_it_outer)[k] != (*bef_red_it_inner)[k])  //�� bitüũ�ϸ鼭 bit�� �ٸ��� //���� note: �ǹ̻� vector�� at, string�� []ó��
					{											
						if ((*bef_red_it_outer)[k] == '-' || (*bef_red_it_inner)[k] == '-') { //���� �ٸ� �� bit �߿� '-'bit�� �ٸ��� �װ� ó�� ���ϴϱ�( �������� �ϳ��� 1�ְ� �ϳ��� '-'������ ���ǹ���)
							count_reducing = 0;
							break; //�ΰ��� bit�� �ٸ���, �ϳ��� '-'�� �ִٸ� �� �ǹ� �����Ƿ� break
						}
						else //��Ʈ�� �ٸ��� �ΰ� ��� '-'�� �ƴϸ�
						{
							pos_reducing = k; //bit�������� 
							count_reducing++;	//bit���� ����						
						}
					}
					if (count_reducing > 1)
						break;
					//���⼭ �ٸ��� break���ش�.
				}
				//for(k)�� ������(�� minterm �ֿ� ����,hamming dist = 1 ���� Ȯ��), �ش� ������ '-'ġȯ�ؼ� reduced_minterms_now�� �߰�.
				//��ü�ְ˻縦 �ϴϱ�, �ڱ� �ڽ��� ������ ���� �ִ����� �����ؾ߉´�.

				if (count_reducing == 1) //hamming dist = 1�̶�� �ǹ�										
				{

					is_minterm_reducable = true;  //���Ŀ� before_reducing����, Fully Checked �� �� minterm�� ���ؼ�, ���� doncare�� ���� �� �����Ƿ� ����������.
					string newMinterm = *bef_red_it_outer;  //�׳� newminterm �� ���縦 �ϰ�
					newMinterm[pos_reducing] = '-'; //���������� ���Ҹ� '-'�� �ٲ�д�. 

					if (find(reduced_minterms_now.begin(), reduced_minterms_now.end(), newMinterm) == reduced_minterms_now.end())
					{
						reduced_minterms_now.push_back(newMinterm); //reduce�Ȱ� �߰��Ѵ�.
					}
				}
				bef_red_it_inner++;
			}
		}
//for(j)�� ������(�� minterm�� ���ؼ� ������ minterm�� ���̴����� Ȯ������), 
		if (is_minterm_reducable == true) {
			reducing_occured = true;
		}
		else {
			is_starred.push_back(*bef_red_it_outer);
		}
		bef_red_it_outer++;
	}

	for (int i = 0; i < is_starred.size(); i++) 			
	{
		PI_GetStarred->push_back(is_starred[i]); 
	}

	pair<vector<string>, bool> p;
	if (reducing_occured == true)												//�Ұ� ����, ���� ������ ���� ����
	{
		vector<string>().swap(before_reducing);									//���� �Ⱦ�, �Ҵ� ����
		p = make_pair(reduced_minterms_now, reducing_occured);
	}
	else																		//�Ұ� ����, ���� ���� ����
	{
		p = make_pair(before_reducing, reducing_occured);
		//	cout << "Reduce End" << endl; ForDebug
	}
	return p;
}