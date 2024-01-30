#include<bits/stdc++.h>
using namespace std;
#define ll long long int 
struct dyn_sa {
	struct node {
		int sa, lcp;
		node *l, *r, *p;
		int sz, mi;
		node(int sa_, int lcp_, node* p_) : sa(sa_), lcp(lcp_),
			l(NULL), r(NULL), p(p_), sz(1), mi(lcp) {}
		void update() {
			sz = 1, mi = lcp;
			if (l) sz += l->sz, mi = min(mi, l->mi);
			if (r) sz += r->sz, mi = min(mi, r->mi);
		}
	};

	node* root;
	vector<ll> tag; // tag of a suffix (reversed id)
	string s; // reversed

	dyn_sa() : root(NULL) {}
	dyn_sa(string s_) : dyn_sa() {
		reverse(s_.begin(), s_.end());
		for (char c : s_) push_front(c);
	}
	~dyn_sa() {
		vector<node*> q = {root};
		while (q.size()) {
			node* x = q.back(); q.pop_back();
			if (!x) continue;
			q.push_back(x->l), q.push_back(x->r);
			delete x;
		}
	}

	int size(node* x) { return x ? x->sz : 0; }
	int mirror(int i) { return s.size()-1 - i; }
	bool cmp(int i, int j) {
		if (s[i] != s[j]) return s[i] < s[j];
		if (i == 0 or j == 0) return i < j;
		return tag[i-1] < tag[j-1];
	}
	void fix_path(node* x) { while (x) x->update(), x = x->p; }
	void flatten(vector<node*>& v, node* x) {
		if (!x) return;
		flatten(v, x->l);
		v.push_back(x);
		flatten(v, x->r);
	}
	void build(vector<node*>& v, node*& x, node* p, int L, int R, ll l, ll r) {
		if (L > R) return void(x = NULL);
		int M = (L+R)/2;
		ll m = (l+r)/2;
		x = v[M];
		x->p = p;
		tag[x->sa] = m;
		build(v, x->l, x, L, M-1, l, m-1), build(v, x->r, x, M+1, R, m+1, r);
		x->update();
	}
	void fix(node*& x, node* p, ll l, ll r) {
		if (3*max(size(x->l), size(x->r)) <= 2*size(x)) return x->update();
		vector<node*> v;
		flatten(v, x);
		build(v, x, p, 0, v.size()-1, l, r);
	}
	node* next(node* x) {
		if (x->r) {
			x = x->r;
			while (x->l) x = x->l;
			return x;
		}
		while (x->p and x->p->r == x) x = x->p;
		return x->p;
	}
	node* prev(node* x) {
		if (x->l) {
			x = x->l;
			while (x->r) x = x->r;
			return x;
		}
		while (x->p and x->p->l == x) x = x->p;
		return x->p;
	}

	int get_lcp(node* x, node* y) {
		if (!x or !y) return 0; // change defaut value here
		if (s[x->sa] != s[y->sa]) return 0;
		if (x->sa == 0 or y->sa == 0) return 1;
		return 1 + query(mirror(x->sa-1), mirror(y->sa-1));
	}
	void add_suf(node*& x, node* p, int id, ll l, ll r) {
		if (!x) {
			x = new node(id, 0, p);
			node *prv = prev(x), *nxt = next(x);
			int lcp_cur = get_lcp(prv, x), lcp_nxt = get_lcp(x, nxt);
			if (nxt) nxt->lcp = lcp_nxt, fix_path(nxt);
			x->lcp = lcp_cur;
			tag[id] = (l+r)/2;
			x->update();
			return;
		}
		if (cmp(id, x->sa)) add_suf(x->l, x, id, l, tag[x->sa]-1);
		else add_suf(x->r, x, id, tag[x->sa]+1, r);
		fix(x, p, l, r);
	}
	void push_front(char c) {
		s += c;
		tag.push_back(-1);
		add_suf(root, NULL, s.size() - 1, 0, 1e18);
	}

	void rem_suf(node*& x, int id) {
		if (x->sa != id) {
			if (tag[id] < tag[x->sa]) return rem_suf(x->l, id);
			return rem_suf(x->r, id);
		}
		node* nxt = next(x);
		if (nxt) nxt->lcp = min(nxt->lcp, x->lcp), fix_path(nxt);

		node *p = x->p, *tmp = x;
		if (!x->l or !x->r) {
			x = x->l ? x->l : x->r;
			if (x) x->p = p;
		} else {
			for (tmp = x->l, p = x; tmp->r; tmp = tmp->r) p = tmp;
			x->sa = tmp->sa, x->lcp = tmp->lcp;
			if (tmp->l) tmp->l->p = p;
			if (p->l == tmp) p->l = tmp->l;
			else p->r = tmp->l;
		}
		fix_path(p);
		delete tmp;
	}
	void pop_front() {
		if (!s.size()) return;
		s.pop_back();
		rem_suf(root, s.size());
		tag.pop_back();
	}
	
	int query(node* x, ll l, ll r, ll a, ll b) {
		if (!x or tag[x->sa] == -1 or r < a or b < l) return s.size();
		if (a <= l and r <= b) return x->mi;
		int ans = s.size();
		if (a <= tag[x->sa] and tag[x->sa] <= b) ans = min(ans, x->lcp);
		ans = min(ans, query(x->l, l, tag[x->sa]-1, a, b));
		ans = min(ans, query(x->r, tag[x->sa]+1, r, a, b));
		return ans;
	}
	int query(int i, int j) { // lcp(s[i..], s[j..])
		if (i == j) return s.size() - i;
		ll a = tag[mirror(i)], b = tag[mirror(j)];
		int ret = query(root, 0, 1e18, min(a, b)+1, max(a, b));
		return ret;
	}
	// optional: get isa[i], sa[i] and lcp[i] (standard index representation)
	int isa(int i) {
		i = mirror(i);
		node* x = root;
		int ret = 0;
		while (x) {
			if (tag[x->sa] < tag[i]) {
				ret += size(x->l)+1;
				x = x->r;
			} else x = x->l;
		}
		return ret;
	}
	// returns a pair with SA[i] and lcp[i]
	pair<int, int> operator[](int i) {
		node* x = root;
		while (1) {
			if (i < size(x->l)) x = x->l;
			else {
				i -= size(x->l);
				if (!i) return {mirror(x->sa), x->lcp};
				i--, x = x->r;
			}
		}
	}
}sa, rev_sa;


const int N = 1e6 + 9;

int power(long long n, long long k, const int mod) {
    int ans = 1 % mod;
    n %= mod;
    if (n < 0) n += mod;
    while (k) {
        if (k & 1) ans = (long long) ans * n % mod;
        n = (long long) n * n % mod;
        k >>= 1;
    }
    return ans;
}

const int MOD1 = 127657753, MOD2 = 987654319;
const int p1 = 137, p2 = 277;
int ip1, ip2;
pair<int, int> pw[N], ipw[N];
void prec() {
    pw[0] =  {1, 1};
    for (int i = 1; i < N; i++) {
        pw[i].first = 1LL * pw[i - 1].first * p1 % MOD1;
        pw[i].second = 1LL * pw[i - 1].second * p2 % MOD2;
    }
    ip1 = power(p1, MOD1 - 2, MOD1);
    ip2 = power(p2, MOD2 - 2, MOD2);
    ipw[0] =  {1, 1};
    for (int i = 1; i < N; i++) {
        ipw[i].first = 1LL * ipw[i - 1].first * ip1 % MOD1;
        ipw[i].second = 1LL * ipw[i - 1].second * ip2 % MOD2;
    }

}
struct Hashing {
    int n;
    string s; // 0 - indexed
    vector<pair<int, int>> hs; // 1 - indexed
    Hashing() {}
    Hashing(string _s) {
        n = _s.size();
        s = _s;
        hs.emplace_back(0, 0);
        for (int i = 0; i < n; i++) {
            pair<int, int> p;
            p.first = (hs[i].first + 1LL * pw[i].first * s[i] % MOD1) % MOD1;
            p.second = (hs[i].second + 1LL * pw[i].second * s[i] % MOD2) % MOD2;
            hs.push_back(p);
        }
    }
    pair<int, int> get_hash(int l, int r) { // 1 - indexed
        assert(1 <= l && l <= r && r <= n);
        pair<int, int> ans;
        ans.first = (hs[r].first - hs[l - 1].first + MOD1) * 1LL * ipw[l - 1].first % MOD1;
        ans.second = (hs[r].second - hs[l - 1].second + MOD2) * 1LL * ipw[l - 1].second % MOD2;
        return ans;
    }
    pair<int, int> get_hash() {
        return get_hash(1, n);
    }
};


const int sz = 2005;
std::vector<vector<int>> OV;
int n; // number of added strings
set<pair<int,int>> prefix_id,candidate_prefix_id, suffix_id, candidate_suffix_id; // isa,index
int new_string_add[sz];
int brute[sz][sz];
std::vector<string> string_list;
std::vector<Hashing> hashes;
int find_match(int i, int j) { // max match of prefix of ith string and suffix of jth suffix
	int mx = 0;
	auto &a = hashes[i];
	auto &b = hashes[j];
	int n1 = string_list[i].size();
	int n2 = string_list[j].size();
	for(int i = min(n1, n2);i && !mx;i--) {
		if(a.get_hash(1, i) == b.get_hash(n2 - i + 1, n2)) mx = i;
	}
	return mx;
}
void add(string a) {
	string_list.push_back(a);
	int cur_len = a.size() + 1;
	Hashing h(a);
	hashes.push_back(h);
	for(int i = 0; i < n; i++) new_string_add[i] += cur_len;
	for(int i = 0; i < n; i++)
		OV[i].push_back(0);
	OV.emplace_back();
	n++;
	OV[n - 1].resize(n, 0);
	string reverse_a = a;
	reverse(reverse_a.begin(), reverse_a.end());
	sa.push_front('#');
	rev_sa.push_front('#');
	for(char &cc:reverse_a) sa.push_front(cc);
	for(char &cc:a) rev_sa.push_front(cc);
	prefix_id.clear();
	suffix_id.clear();
	for(int i = 0; i < n; i++) {
		prefix_id.insert({sa.isa(new_string_add[i]), i});
		suffix_id.insert({rev_sa.isa(new_string_add[i]), i});
	}
	candidate_prefix_id = prefix_id;
	candidate_suffix_id = suffix_id;
	// first task
	auto first_task = [&]() {
		for(int i = cur_len - 1, j = 0; i > 0; i--, j++) {
			int cur_suffix_id = sa.isa(j);
			auto it = candidate_prefix_id.upper_bound({cur_suffix_id, -1});
			while(it != candidate_prefix_id.end()) {
				auto [id, ind] = *it;
				int lcp = sa.query(new_string_add[ind], j);
				// if(n == 3) {
				// 	cout << j << " " << ind  << " " << lcp << "\n";
				// }
				if(lcp < i) break;
				OV[ind][n - 1] = i;
				it = candidate_prefix_id.erase(it);
			}
			it = candidate_prefix_id.upper_bound({cur_suffix_id, -1});
			while(it != candidate_prefix_id.begin()) {
				it--;
				auto [id, ind] = *it;
				int lcp = sa.query(new_string_add[ind], j);
				// if(n == 3) {
				// 	cout << j << " " << ind << " " << lcp << "\n";
				// }
				if(lcp < i) break;
				OV[ind][n - 1] = i;
				it = candidate_prefix_id.erase(it);
			}
		}
	};
	first_task();
	// second task . This will works in reverse suffix array
	auto second_task = [&] {
		for(int i = cur_len - 1, j = 0; i > 0; i--, j++) {
			int cur_suffix_id = rev_sa.isa(j);
			auto it = candidate_suffix_id.upper_bound({cur_suffix_id, -1});
			while(it != candidate_suffix_id.end()) {
				auto [id, ind] = *it;
				int lcp = rev_sa.query(new_string_add[ind], j);
				if(lcp < i) break;
				// if(n == 2) {
				// 	cout << j << " " << ind << " " << lcp << "\n";
				// }
				OV[n - 1][ind] = i;
				it = candidate_suffix_id.erase(it);
			}
			it = candidate_suffix_id.upper_bound({cur_suffix_id, -1});
			while(it != candidate_suffix_id.begin()) {
				it--;
				auto [id, ind] = *it;
				int lcp = rev_sa.query(new_string_add[ind], j);
				if(lcp < i) break;
				OV[n - 1][ind] = i;
				it = candidate_suffix_id.erase(it);
			}
		}
	};
	
	// for(int i = 0; i < n; i++)
		// OV[n - 1][i] = find_match(n - 1, i);
	second_task();
}
void prin() {
	// cout << "\n";
	// cout << n << "\n";
	for(int i = 0; i < n; i++) {
		for(int j = 0; j < n; j++) {
			int mx = find_match(i, j);
			if(mx != OV[i][j]) {
				cout << mx << ' ' << OV[i][j] << ' ' << i << " " << j << "\n";
				exit(0);
			}
			assert(mx == OV[i][j] );
			// cout << OV[i][j] << " \n"[j == n - 1];
		}
	}
}
int32_t main() {
	ios_base::sync_with_stdio(false);
	cin.tie(0);
	prec();
	string c;
	auto for_loop = [&] {
		for(int i = 0; i < 2; i++) {
			cin >> c;
			add(c);
			// prin();
		}
		exit(0);
	};
	// for_loop();
	auto while_loop = [&]() {
		while(cin >> c) {
			add(c);
			// prin();
		}
	};
	while_loop();
	return 0; 
}
