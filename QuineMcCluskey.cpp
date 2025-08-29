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
int MINTERM_LENGTH;	//bit 수

class inf {
public:
	vector<pair<string, string> > sortinfo(vector<pair<string, string> > min); //minterm들 오름차순으로 정렬하는 함수
};

vector<pair<string, string> > inf::sortinfo(vector < pair<string, string> > min) //minterm들 오름차순으로 정렬하는 함수
{
	int i, j, x, y;
	for (i = 0; i < min.size() - 1; i++)
	{
		for (j = 0; j < min.size() - 1 - i; j++)
		{
			x = count(min[j].second.begin(), min[j].second.end(), '1'); //1의 개수 세기
			y = count(min[j + 1].second.begin(), min[j + 1].second.end(), '1'); //1의 개수 세기
			if (x > y) //앞에 있는 것이 뒤에 있는 것보다 1을 더 많이 가지고 있다면
				swap(min[j], min[j + 1]); //위치 바꾸기
		}
	}
	return min; //정렬한 함수를 리턴
}


pair<vector<string>, bool> reduce(vector<string>, vector<string>*);


int main() {

	vector<pair<string, string>> minterms; //전달받은 vector의 minterm. string1: dm, string2: minterm내용
	pair<vector<string>, bool> reduced_minterms; //현재 단계에서, minterm을 doncare 묶어버린 항들만 포함한다.
	//1. starred 항은 포함 안한다. 각 단계에서 before_reducing에 남아, 그항들은 PI에 자동 추가할 것이다.
	//2. 매 reduce()함수 호출 때마다 기존값을 버리고 1단계 더 reduce 된 항들을 받게 된다.
	vector<string> PrimeImplicants; //최종적으로, PI들만 모이게된다.
	//1. PI는 reduce()함수에서 매번 starred 된 항들을 값으로 추가된다.
	//2. 마지막에 reduced_minterms의 bool이 false가 되면 reduced_minterms에 남아있는 항들이 값으로 추가된다.

	//step 1)텍스트 파일을 통해 간소화할 논리 식을 입력받는 단계

	char filename[30] = "input_minterm.txt"; //input 파일 이름
	string s, ch, t; //s: 처음 문자열 받아 임시로 저장, ch: minterm or dontcare, t: 2진수
	int i;
	inf x;

	ifstream file(filename); //파일 읽기 전용으로 열기
	file >> MINTERM_LENGTH; //input파일에서 input bit length읽기

	getline(file, s); //파일에서 한 줄씩 읽기
	while (!file.eof()) //파일의 끝에 도달하기 전까지 반복
	{
		getline(file, s); //파일에서 한 줄씩 읽기
		istringstream f(s); //문자열 파싱 함수 
		f >> ch >> t; //d인지 m인지를 ch에 저장, minterms를 t에 저장
		minterms.push_back(make_pair(ch, t));
	}
	file.close();
	minterms = x.sortinfo(minterms); //입력받은 minterm들 정렬하는 함수 호출하고,정렬된 결과를 다시 vector에 저장

	//dm데이터 제거해 minterm을 reduced_minterm에 load
	for (int i = 0; i < minterms.size(); i++)
		reduced_minterms.first.push_back(minterms.at(i).second);

	//step 2) 주어진 논리 식의 후보 항(Prime Implicants)를 구하는 단계
	do {

		reduced_minterms = reduce(reduced_minterms.first, &PrimeImplicants);
		sort(reduced_minterms.first.begin(), reduced_minterms.first.end());
		reduced_minterms.first.erase(unique(reduced_minterms.first.begin(), reduced_minterms.first.end()), reduced_minterms.first.end());
	} while (reduced_minterms.second != false);

	pair<vector<string>, bool>().swap(reduced_minterms); //deAlloc

	//step 3) 후보 항들을 이용하여 필수 항(Essential Prime Implicant)를 구하는 단계
	int PIsize = PrimeImplicants.size();
	int minsize = minterms.size();

	//minterm 항 삭제 시 쓰는 변수
	vector<pair<string, string>>::iterator minterm_it;					//minterm iterator

	//pi<->mins 표를 정의하는 데이터 변수
	vector<vector<int>> pi_to_mins_idx;									//i번째요소는 PI를 의미, vector<int>는 covering하는 minterms index를 저장
	vector<vector<int>> min_to_pis_idx;									//i번째요소는 minterm을 의미, vector<int>는 covered가능한 PIs index를 저장
	vector<int>minsofpi;
	vector<int>pisofmin;

	//이차원벡터 기본 대기시켜둠
	for (int i = 0; i < PIsize; i++)
		pi_to_mins_idx.push_back(minsofpi);
	for (int i = 0; i < minsize; i++)
		min_to_pis_idx.push_back(pisofmin);

	vector<int>().swap(minsofpi); //minsofpi할당해제
	vector<int>().swap(pisofmin); //pisofmin할당해제

	//cover할 남은 minterm(left minterm)과 마지막에 필요한 PI
	vector<int>	left_minterm;										//cover해야하는, 남은 minterm의 index벡터
	vector<int> needpi;												//최종적으로 필요한 PI들 전부의 index벡터
	vector<int>::iterator l_minterm_it;								//left minterm iterator

	vector<int> left_pi;
	vector<int>::iterator l_pi_it;

	vector<int> coveringpis;											//지금 있는 left minterm을 커버가능한 PI들 전부

	int max_covering_PI; // 그중에서 단 하나의 뽑아내기로 한 PI		
	int max_coverings;													//PI가 커버링할 수 있는 가장 많은 left minterm개수


	minterm_it = minterms.begin();

	//doncare 지우기
	while (minterm_it != minterms.end()) {
		if (minterm_it->first == "d")   // minterm_it에 해당하는 minterm 요소 str1 이 "d"일 경우
			minterm_it = minterms.erase(minterm_it);								// 해당 doncare 요소를 지워버리고 그 자리에 minterm_it를 물려버린다	
		//
		else
			minterm_it++;
	}
	//PI -> minterm s, minterm -> PI s 의 index 대응 관계를 index벡터화하는단계	

	minsize = minterms.size(); //minterm doncare 삭제 후 크기 변경	

	//cover 관계 부여 - 삭제하는 게 없으면 iterator 안쓰고 for문 사용해도 됨	//'-'이 아닌 literal을 하나씩 비교하며 PI가 minterm을 cover 하면 체크
	for (int i = 0; i < PIsize; i++)										//for문 반복: 각 PI마다, i counter
	{
		for (int j = 0; j < minsize; j++)
		{											//for문 반복: 각 minterm마다, j counter
			bool covers = true;											//PI가 minterm을 커버하는지를 체크하는 변수
			//PI와 minterm cover를 모든 쌍에 대해 조사
			for (int k = 0; k < MINTERM_LENGTH; k++) {							//literal마다 체크 //for문 반복: 각 literal마다, k counter
				if (PrimeImplicants.at(i)[k] != '-') {				// i번째 PI의 string의 k번째 literal이 '-'아니면 
					if (PrimeImplicants.at(i)[k] != minterms.at(j).second[k]) {		 //만약에 literal이 하나라도 다르면 cover를 false로 내리고 이 쌍에 대해서 비교 break
						covers = false;
						break;
					}
				}
			}
			if (covers)													//모든 '-'아닌 literal에 대해서 동일한경우
			{
				pi_to_mins_idx.at(i).push_back(j);						//pi_to_minterms[i].push_back(j)가 먹힘.
				min_to_pis_idx.at(j).push_back(i);						//min_to_pis_idx에 minterms만큼 넣어뒀음. 				//j번째 minterm은 i번째 pi에 cover됨을 의미함
			}
		}
	}


	//left minterm pi 대기시키기						
	{
		//minterm 전부를 남은 minterm에 대기시킴
		for (int i = 0; i < minterms.size(); i++) {
			left_minterm.push_back(i);
		}
		//pi 전부를 남은 pi에 대기시킴
		for (int i = 0; i < PrimeImplicants.size(); i++) {
			left_pi.push_back(i);
		}
	}

	//EPI 찾기

	{
		//vector<int>::iterator l_min_coverable_it; //EPI가 cover 가능한 leftminterm 목록의 iterator. 그 목록 내용은 지울 이유가 없다. -> for문.
		vector<int>::iterator covered; //만약선택한 pi 대응 minterm들이, left minterm에서 지워지지 않고 남아잇다면 그걸 대응하기 위해서.
		l_minterm_it = left_minterm.begin();
		while (l_minterm_it != left_minterm.end()) {

			if (min_to_pis_idx.at(*l_minterm_it).size() == 1) {


				int epi_idx = min_to_pis_idx.at(*l_minterm_it).at(0); //left minterm에 하나밖에 대응안돼는 PI
				needpi.push_back(epi_idx);


				//EPI부터 leftpi에서 지운다.
				l_pi_it = find(left_pi.begin(), left_pi.end(), epi_idx);

				l_pi_it = left_pi.erase(l_pi_it);  //iterator는 지운다. 

				//지운 EPI idx에 대응하는 minterm 목록에서 하나씩 넘기며, left_minterm에서 비교하여 찾는다.

				for (int i = 0; i < pi_to_mins_idx.at(epi_idx).size(); i++) {
					int now_min_crpd_pi = pi_to_mins_idx.at(epi_idx).at(i);


					covered = find(left_minterm.begin(), left_minterm.end(), now_min_crpd_pi);
					if (covered != left_minterm.end()) {	//만약선택한 pi 대응 minterm들이, left minterm에서 지워지지 않고 남아잇다면 //left minterm 중 EPI가 대응하는 minterm을 지워버림 

						l_minterm_it = left_minterm.erase(covered); //l_minterm이 삭제되었으니까 해당 지점 반환
					}
				}
			}
			else
				l_minterm_it++;
		}
	}

	//step 4) 가장 많은 minterm을 나타낼 수 있는 후보 항부터 선택하여, 모든 minterm을 나타낼 수 있는 후보 항 조합 중 적은 트랜지스터를 사용하는 조합을 만드는 단계 
	while (left_minterm.begin() != left_minterm.end()) {							//left minterm 원소가 완전히 사라질때까지, 없으면 종료
		//left minterm 커버링 가능한 가장 큰 PI개수를 체크할 것이다.
		max_coverings = 0;

		//left_minterm을 cover 가능한 PI 목록을 가져와 coveringpis에 쌓기
		for (int i = 0; i < left_minterm.size(); i++) {								//left minterm의 원소 -> 대응하는 PI들을 뽑아낸다.

			for (int j = 0; j < min_to_pis_idx.at(left_minterm.at(i)).size(); j++) {
				int now_pi_crpd_lmin = min_to_pis_idx.at(left_minterm.at(i)).at(j); //현재 left min에 대응하는 pi 리스트 요소 j번째. corresponded의 줄임말
				if (find(needpi.begin(), needpi.end(), now_pi_crpd_lmin) == needpi.end()) { // PI가 needpi에 이미 넣은 pi가 아니라면

					if (find(coveringpis.begin(), coveringpis.end(), now_pi_crpd_lmin) == coveringpis.end()) //now_pi_crpd_lmin를 이미 coveringpi에 넣은게 아니면
						coveringpis.push_back(now_pi_crpd_lmin); //coveringpi에 넣어주기
				}
			}
		}


		//coveringpis들을 한개씩 살펴본다. cover 가능한 left minterm을 coverings에 넣어가면서 max_covering인 PI max_covering_PI 구하기
		for (int i = 0; i < coveringpis.size(); i++) {												// left minterms 순서대로 불러오기
			int now_cvpi = coveringpis.at(i);
			vector<int> coverings;												//각 PI가 cover 가능한 left minterms목록
			for (int j = 0; j < left_minterm.size(); j++) {											//coveringpis 순서대로불러오기
				vector<int>::iterator findlmin_crpd_pi = find(pi_to_mins_idx.at(now_cvpi).begin(), pi_to_mins_idx.at(now_cvpi).end(), left_minterm.at(j)); //coveringpis i번째 원소 pi -> pi_to_mins_idx 대응 mins, 에서 left_minterm.at(j)를 찾는다. ...각자의 size는 이미 수인데? 안돌아갈리가 없잖아

				if (findlmin_crpd_pi != pi_to_mins_idx.at(now_cvpi).end()) { //left minterm을 현재 pi대응mins에서 찾는 결과가 존재만하면
					coverings.push_back(left_minterm.at(j)); //coverings에 left minterm 넣기
				}
			}
			//cout<<endl; //ForDebug
			if (coverings.size() > max_coverings) {				//한개의 covering pi에 대해서, covering가능한 left minterm의 수가 기록해뒀던 max left coverings보다 크다면
				max_coverings = coverings.size();				//그최대를  coverings size로 갱신하고
				max_covering_PI = now_cvpi;					//최종 선택하는 PI를 i번째 covevering pi로 갱신
			}
			vector<int>().swap(coverings);
		}														//최종적으로 가장 큰 covering PI가 max_covering_PI

		//가장 큰 covering PI를 needpi에 넣고, 그에 대응하는 left minterm 구해서 삭제
		needpi.push_back(max_covering_PI);
		for (int i = 0; i < max_coverings; i++) {					//max_coverings = max_covering_PI가 하는 left minterm 개수
			auto p = find(left_minterm.begin(), left_minterm.end(), pi_to_mins_idx.at(max_covering_PI).at(i));		//max_covering_PI가 cover 가능한 minterm을 차례로 left minterm에서 찾아서 지워버림
			left_minterm.erase(p);
		}
		//최종적으로 현재의 left minterm마다 최대 coverings의 PI와 covering 하는 minterm을 뽑아내면서 left_minterm은 0이됨
	}


	vector<int>().swap(left_minterm);
	vector<int>().swap(left_pi);
	vector<vector<int>>().swap(pi_to_mins_idx);									//이제 안씀, 할당 해제
	vector<vector<int>>().swap(min_to_pis_idx);
	vector<int>().swap(coveringpis);

	int transistors;
	int bit;											//사용하는 총 transistor 개수, 연결 bit 개수
	int AND = 0;														//AND 게이트에 사용하는 transistor 개수

	ofstream fout("result.txt");


	//step 5) 최종적으로 고른 PI 조합과 조합을 SOP form으로 나타내기 위한 transistor 개수 출력 단계
	for (int i = 0; i < needpi.size(); i++) {							//needpi 전체에 대해
		fout << PrimeImplicants.at(needpi.at(i)) << endl;
		bit = 0;
		for (int j = 0; j < MINTERM_LENGTH; j++) {								//우리가 처리할 PI 길이 각글자마다
			if (PrimeImplicants.at(needpi.at(i))[j] != '-')			//needpi 첫번째 idx부터 대응시킨 PI의, j번째 글자가 -이 아니면
				bit++;													//해당 PI의 bit 수 추가
		}
		AND = AND + 2 * bit + 2;										//이 PI를 AND 구현 시 필요한 비트 수 
	}
	fout << endl;
	transistors = 2 * static_cast<unsigned long long>(MINTERM_LENGTH) + AND + (2 * needpi.size() + 2);		//총 트랜지스터 개수
	fout << "Cost (# of transistors):" << transistors;

	fout.close();

	cout << "Result saved in result.txt" << endl;
	return 0;
}

pair<vector<string>, bool> reduce(vector<string> before_reducing, vector<string>* PI_GetStarred)
{
	vector<string> reduced_minterms_now; //이번 단계의 reduced minterms
	bool reducing_occured; //이번 단계에서.어떠한 minterm 쌍도 reduce되었을시, 비교하면서 true로 바꿀것. 이후에 before_reducing과 pair로 담겨 반환
	int pos_reducing; //bit차이 위치 기록
	int count_reducing = 0; //bit차이 개수 기록	
	bool is_minterm_reducable; //각 minterm에 대해서, 다른 minterm과 reduce 되었다면. 비교 끝난 minterm은 지워버리기 위해서

	vector<string> is_starred;
	vector<string>::iterator bef_red_it_outer;
	vector<string>::iterator bef_red_it_inner;

	//쌍마다 비교하며 reduce 단계
	reducing_occured = false;
	bef_red_it_outer = before_reducing.begin();
	while (bef_red_it_outer != before_reducing.end())
	{
		is_minterm_reducable = false; //각 minterm에 대해서 기본적으로는 minterm이 reducable하지 않다고 초기화해야함.

		bef_red_it_inner = before_reducing.begin();
		while (bef_red_it_inner != before_reducing.end())
		{
			if (bef_red_it_inner == bef_red_it_outer) { //자기 자신과의 비교 skip
				bef_red_it_inner++;
				continue;
			}
			else {

				count_reducing = 0;//각 minterm쌍마다 체크할 pair개수 0으로 초기화. 
				//쌍의 한 bit 씩 비교하며 두 minterm의 bit 다름 체크
				for (int k = 0; k < MINTERM_LENGTH; k++) {
					if ((*bef_red_it_outer)[k] != (*bef_red_it_inner)[k])  //각 bit체크하면서 bit가 다르면 //개발 note: 의미상 vector는 at, string은 []처리
					{
						if ((*bef_red_it_outer)[k] == '-' || (*bef_red_it_inner)[k] == '-') { //서로 다른 두 bit 중에 '-'bit가 다르면 그건 처리 안하니까( 같은곳에 하나는 1있고 하나느 '-'있으면 무의미함)
							count_reducing = 0;
							break; //두개의 bit가 다른데, 하나라도 '-'가 있다면 비교 의미 없으므로 break
						}
						else //비트는 다른데 두개 모두 '-'가 아니면
						{
							pos_reducing = k; //bit차이지점 
							count_reducing++;	//bit차이 개수						
						}
					}
					if (count_reducing > 1)
						break;
					//여기서 다르면 break해준다.
				}
				//for(k)가 끝나고(이 minterm 쌍에 대해,hamming dist = 1 임을 확인), 해당 지점만 '-'치환해서 reduced_minterms_now에 추가.
				//전체 쌍 검사를 하니까, 자기 자신을 포함한 적이 있는지를 생각해야 함.

				if (count_reducing == 1) //hamming dist = 1이라는 의미										
				{

					is_minterm_reducable = true;  //이후에 before_reducing에서, Fully Checked 된 이 minterm에 대해서, 여러 doncare로 묶일 수 있으므로 지워버린다.
					string newMinterm = *bef_red_it_outer;  //그냥 newminterm 에 복사를 하고
					newMinterm[pos_reducing] = '-'; //차이지점의 원소를 '-'로 바꿔둔다. 

					if (find(reduced_minterms_now.begin(), reduced_minterms_now.end(), newMinterm) == reduced_minterms_now.end())
					{
						reduced_minterms_now.push_back(newMinterm); //reduce된걸 추가한다.
					}
				}
				bef_red_it_inner++;
			}
		}
		//for(j)가 끝나고(이 minterm에 대해서 나머지 minterm과 묶이는지를 확인했음), 
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
	if (reducing_occured == true)												//소거 성공, 새로 생성된 벡터 전달
	{
		vector<string>().swap(before_reducing);									//이제 안씀, 할당 해제
		p = make_pair(reduced_minterms_now, reducing_occured);
	}
	else																		//소거 실패, 기존 벡터 전달
	{
		p = make_pair(before_reducing, reducing_occured);
		//	cout << "Reduce End" << endl; ForDebug
	}
	return p;
}
