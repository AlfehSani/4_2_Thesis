// This code is used for generating RANDOM DNA sequence
// This code is run in cpp
// For randomization purpose it use mt19937
#include<bits/stdc++.h>
using namespace std;
mt19937_64 rnd(chrono::steady_clock::now().time_since_epoch().count());
string alphabet = "ATCGEFDHIJKLMNOPQRSBUVWXYZabcdefghijklmnopqrstuvwxyz0123456789";
//This function will generate a ranodm DNA sequnce of length equal to 'len'
string build_random_string(int len, int clen = 62) {
    string ans = "";
    for(int i = 0; i < len; i++) {
    	ans += alphabet[rnd() % clen];
    }
    return ans;
}
// This function will generate a vector of size equal to 'len' that comprises of random DNA sequence of fixed length 'sz'
vector<string> string_vector(int len, int sz) {
	std::vector<string> ans(len);
	for(int i = 0; i < len; i++) {
		int cur_len = sz;
		ans[i] = build_random_string(cur_len, 4);
	}
	return ans;
}
int main() {
	ios_base::sync_with_stdio(false);
	cin.tie(0);
	std::vector<string> ans = string_vector(300, 500);
	for(auto &it:ans) cout << it << "\n";	
	return 0; 
}
