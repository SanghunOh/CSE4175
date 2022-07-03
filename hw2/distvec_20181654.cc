#include <bits/stdc++.h>

#define FOLDER 0
#define PRINTTOFILE 1
#define MAX 10000000

std::pair<int, int> routingTable[101][101];
int direction[101][101];

typedef struct {
	int from;
	int to;
	int weight;
} Edge;

struct compare
{
	bool operator()(std::pair<int, int> a, std::pair<int, int> b) 
	{
		if (a.second == b.second)
			return a.first > b.first;
		return a.second > b.second;
	}
};

void tieBreak(std::vector<Edge> edges, int N, int start)
{
	std::pair<bool, int> visit[101];
	std::queue<std::pair<int, int> > q;

	q.push(std::make_pair(start, 0));
	visit[start] = std::make_pair(true, 0);

	while (!q.empty()) {
		int cur = q.front().first;
		int dist = q.front().second;
		q.pop();

		for (size_t i = 0; i < edges.size(); i++) {
			if (edges[i].from != cur && edges[i].to != cur)
				continue;
			int next = edges[i].from == cur ? edges[i].to : edges[i].from;

			if (visit[next].first == true) {
				if (routingTable[start][next].second == dist + edges[i].weight && \
						direction[start][next] > direction[start][cur]) {
					direction[start][next] = direction[start][cur];
				}
				continue;
			}
			if (routingTable[start][next].first == start) {
				direction[start][next] = next;
				visit[next] = std::make_pair(true, cur);
				q.push(std::make_pair(next, routingTable[start][next].second));

				continue;
			}
			if (routingTable[start][next].second == dist + edges[i].weight) {
				direction[start][next] = direction[start][cur];
				visit[next] = std::make_pair(true, cur);
				q.push(std::make_pair(next, routingTable[start][next].second));

				continue;
			}
		}
	}
}

void bellmanFord(std::vector<Edge> edges, int N, int E, int start)
{
	for (int i = 0; i < E; i++) {
		routingTable[start][i] = std::make_pair(-1, MAX);
	}
	routingTable[start][start].second = 0;

	for (int i = 0; i < N - 1; i++) {
		for (size_t j = 0; j < edges.size(); j++) {
			int from = edges[j].from;
			int to = edges[j].to;
			int weight = edges[j].weight;

			int dist = routingTable[start][from].second + weight;
			if (routingTable[start][from].second == MAX && routingTable[start][to].second == MAX)
				continue;

			if (routingTable[start][to].second > dist) {
				routingTable[start][to] = std::make_pair(from, dist);
			}

			dist = routingTable[start][to].second + weight;
			
			if (routingTable[start][from].second > dist) {
				routingTable[start][from] = std::make_pair(to, dist);
			}
		}
	}
}

void chooseDirection(int N, int start)
{
	for (int i = 0; i < N; i++) {
		int t = routingTable[start][i].first;
		int pre = i;
		if (t == -1 && routingTable[start][i].second == MAX) {
			direction[start][i] = -1;
			continue;
		}
		if (i == start)
			direction[start][i] = i;
		else {
			while (t != start){
				pre = t;
				t = routingTable[start][t].first;
			}
			direction[start][i] = pre;
		}
	}
}

std::vector<int> sendMessage(char message[1001], int from, int to, \
		std::vector<std::pair<int, int> > topology[], int N, int M)
{
	std::vector<int> hops;
	int rt = from;
	int cost = routingTable[from][to].second;

	while (rt != to) {
		hops.push_back(rt);
		
		rt = direction[rt][to];
		if (routingTable[rt][to].second == MAX) {
			hops.clear();

			return hops;
		}
	}
	hops.push_back(cost);

	return hops;
}

void printroutingTable(FILE* file, int N)
{
	for (int i = 0; i < N; i++) {
		for (int j = 0; j < N; j++) {
			if (routingTable[i][j].second != MAX)
				fprintf(file, "%d %d %d\n", j, direction[i][j], routingTable[i][j].second);
		}
		fprintf(file, "\n");
	}
}

int find(std::vector<Edge> edges, int from, int to, int weight)
{
	for (size_t i = 0; i < edges.size(); i++) {
		if ((edges[i].from == from && edges[i].to == to) || (edges[i].from == to && edges[i].to == from))
			return i;
	}

	return -1;
}

void printMessage(FILE* file, std::vector<std::pair<int, int> > topology[], std::pair<int, int> messageNode[],\
		char messages[][1001], int N, int M) 
{
	for (int i = 0; i < M; i++) {
		int from = messageNode[i].first;
		int to = messageNode[i].second;
		std::vector<int> res = sendMessage(messages[i], from, to, topology, N, M);

		if (!res.size()) {
			fprintf(file, "from %d to %d cost infinite hops unreachable message %s", from, to, messages[i]);
		}
		else {
			fprintf(file, "from %d to %d cost %d hops ", from, to, res.back());
			res.pop_back();
			for (size_t j = 0; j < res.size(); j++)
				fprintf(file, "%d ", res[j]);
			fprintf(file, "message %s", messages[i]);
		}
	}
	fprintf(file, "\n");
}

int main(int argc, char** argv)
{
	FILE* topologyFile, *messageFile, *changesFile;
	FILE* outputFile;
	int N, M;
	char dir[30];
	int from, to, weight;
	char buf[1001];
	char messages[101][1001];
	std::pair<int, int> messageNode[101];
	int i;
	int len;
	std::vector<std::pair<int, int> > topology[101];
	std::vector<Edge> edges;

	if (argc != 4)
		std::cout << "usage: distvec topologyfile messagesfile changesfile\n";

#if FOLDER == 1
	sprintf(dir, "./1/%s", argv[1]);
	topologyFile = fopen(dir, "r");
	sprintf(dir, "./1/%s", argv[2]);
	messageFile = fopen(dir, "r");
	sprintf(dir, "./1/%s", argv[3]);
	changesFile = fopen(dir, "r");
#elif FOLDER == 2
	sprintf(dir, "./2/%s", argv[1]);
	topologyFile = fopen(dir, "r");
	sprintf(dir, "./2/%s", argv[2]);
	messageFile = fopen(dir, "r");
	sprintf(dir, "./2/%s", argv[3]);
	changesFile = fopen(dir, "r");
#elif FOLDER == 3
	sprintf(dir, "./3/%s", argv[1]);
	topologyFile = fopen(dir, "r");
	sprintf(dir, "./3/%s", argv[2]);
	messageFile = fopen(dir, "r");
	sprintf(dir, "./3/%s", argv[3]);
	changesFile = fopen(dir, "r");
#elif FOLDER == 4
	sprintf(dir, "./4/%s", argv[1]);
	topologyFile = fopen(dir, "r");
	sprintf(dir, "./4/%s", argv[2]);
	messageFile = fopen(dir, "r");
	sprintf(dir, "./4/%s", argv[3]);
	changesFile = fopen(dir, "r");
#elif FOLDER == 5
	sprintf(dir, "./5/%s", argv[1]);
	topologyFile = fopen(dir, "r");
	sprintf(dir, "./5/%s", argv[2]);
	messageFile = fopen(dir, "r");
	sprintf(dir, "./5/%s", argv[3]);
	changesFile = fopen(dir, "r");
#else
	sprintf(dir, "./%s", argv[1]);
	topologyFile = fopen(dir, "r");
	sprintf(dir, "./%s", argv[2]);
	messageFile = fopen(dir, "r");
	sprintf(dir, "./%s", argv[3]);
	changesFile = fopen(dir, "r");
#endif
	if (!topologyFile || !messageFile || !changesFile) {
		std::cout << "Error: open input file\n";
		exit(1);
	}
	outputFile = fopen("output_dv.txt", "w");

	fscanf(topologyFile, "%d", &N);
	while ((fscanf(topologyFile, "%d %d %d", &from, &to, &weight)) != -1) {
		Edge t;
		t.from = from;
		t.to = to;
		t.weight = weight;
		edges.push_back(t);
	}

	M = 0;
	while ((fgets(buf, 1001, messageFile)) != NULL) {
		char* delim;
		char *bufP = buf;
		
		delim = strchr(bufP, ' ');
		len = delim - bufP;
		buf[len++] = '\0';
		messageNode[M].first = atoi(bufP);
		bufP = delim + 1;

		delim = strchr(bufP, ' ');
		len = delim - bufP;
		buf[len++] = '\0';
		messageNode[M].second = atoi(bufP);
		bufP = delim + 1;

		delim = strchr(bufP, '\0');
		len = delim - bufP;
		
		strncpy(messages[M], bufP, len);
		M++;
	}

	for (i = 0; i < N; i++) {
		bellmanFord(edges, N, edges.size(), i);
		chooseDirection(N, i);
		tieBreak(edges, N, i);
	}

#if PRINTTOFILE != 1
	printroutingTable(stdout, N);
	printMessage(stdout, topology, messageNode, messages, N, M);
#else
	printroutingTable(outputFile, N);
	printMessage(outputFile, topology, messageNode, messages, N, M);
#endif
	
	while ((fscanf(changesFile, "%d %d %d", &from, &to, &weight)) != -1) {
		Edge t;
		t.from = from;
		t.to = to;
		t.weight = weight;
		int idx = find(edges, from, to, weight);

		if (idx == -1) {
			if (weight != -999) {
				edges.push_back(t);
			}
		}
		else {
			std::vector<Edge>::iterator it = edges.begin() + idx;
			if (weight == -999) {
				edges.erase(it);
			}
			else {
				it->weight = weight;
			}
		}

		for (i = 0; i < N; i++) {
			bellmanFord(edges, N, edges.size(), i);
			chooseDirection(N, i);
			tieBreak(edges, N, i);
		}

#if PRINTTOFILE != 1
		printroutingTable(stdout, N);
		printMessage(stdout, topology, messageNode, messages, N, M);
#else
		printroutingTable(outputFile, N);
		printMessage(outputFile, topology, messageNode, messages, N, M);
#endif
	}

	std::cout << "Complete. Output file written to output_dv.txt\n";
}
