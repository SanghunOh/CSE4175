#include <bits/stdc++.h>

#define FOLDER 0
#define PRINTTOFILE 1
#define MAX 10000000

std::pair<int, int> shortestPath[101][101];
int direction[101][101];

struct compare
{
	bool operator()(std::pair<int, int> a, std::pair<int, int> b) 
	{
		if (a.second == b.second)
			return a.first > b.first;
		return a.second > b.second;
	}
};

void tieBreak(std::vector<std::pair<int, int> >topology[], int N, int start)
{
	std::pair<bool, int> visit[101];
	std::queue<std::pair<int, int> > q;

	q.push(std::make_pair(start, 0));
	visit[start] = std::make_pair(true, 0);

	while (!q.empty()) {
		int cur = q.front().first;
		int dist = q.front().second;
		q.pop();

		for (size_t i = 0; i < topology[cur].size(); i++) {
			int next = topology[cur][i].first;

			if (visit[next].first == true) {
				if (shortestPath[start][next].second == dist + topology[cur][i].second && cur < visit[next].second) {
					direction[start][next] = direction[start][cur];
				}
				continue;
			}
			if (shortestPath[start][next].first == start) {
				direction[start][next] = next;
				visit[next] = std::make_pair(true, cur);
				q.push(std::make_pair(next, shortestPath[start][next].second));

				continue;
			}
			if (shortestPath[start][next].second == dist + topology[cur][i].second) {
				direction[start][next] = direction[start][cur];
				visit[next] = std::make_pair(true, cur);
				q.push(std::make_pair(next, shortestPath[start][next].second));

				continue;
			}
		}
	}
}

void dijkstra(std::vector<std::pair<int, int> > topology[], int N, int start)
{
	std::priority_queue<std::pair<int, int>, std::vector<std::pair<int, int> >, compare> minVertexHeap;

	for (int i = 0; i < N; i++)
		shortestPath[start][i] = std::make_pair(-1, MAX);
	shortestPath[start][start].second = 0;
	direction[start][start] = start;
	minVertexHeap.push(std::make_pair(start, 0));


	while (!minVertexHeap.empty()) {
		int cur = minVertexHeap.top().first;
		int d = minVertexHeap.top().second;
		minVertexHeap.pop();

		if (shortestPath[start][cur].second < d)
			continue;

		for (size_t i = 0; i < topology[cur].size(); i++) {
			int to = topology[cur][i].first;
			int weight = topology[cur][i].second;

			int dist = d + weight;

			if (dist < shortestPath[start][to].second) {
				shortestPath[start][to].second = dist;
				shortestPath[start][to].first = cur;
				minVertexHeap.push(std::make_pair(to, dist));
			}
		}
	}
}

void chooseDirection(int N, int start)
{
	for (int i = 0; i < N; i++) {
		int t = shortestPath[start][i].first;
		int pre = i;
		if (t == -1 && shortestPath[start][i].second == MAX) {
			direction[start][i] = -1;
			continue;
		}
		if (i == start)
			direction[start][i] = i;
		else {
			while (t != start){
				pre = t;
				t = shortestPath[start][t].first;
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
	int cost = shortestPath[from][to].second;

	while (rt != to) {
		hops.push_back(rt);
		
		rt = direction[rt][to];
		if (shortestPath[rt][to].second == MAX) {
			hops.clear();

			return hops;
		}
	}
	hops.push_back(cost);

	return hops;
}

void printShortestPath(FILE* file, int N)
{
	for (int i = 0; i < N; i++) {
		for (int j = 0; j < N; j++) {
			if (shortestPath[i][j].second != MAX)
				fprintf(file, "%d %d %d\n", j, direction[i][j], shortestPath[i][j].second);
		}
		fprintf(file, "\n");
	}
}

int find(std::vector<std::pair<int, int> > topology[], int from, int to, int weight)
{
	for (size_t i = 0; i < topology[from].size(); i++) {
		if (topology[from][i].first == to)
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

	if (argc != 4)
		std::cout << "usage: linkstate topologyfile messagesfile changesfile\n";

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
	outputFile = fopen("output_ls.txt", "w");

	fscanf(topologyFile, "%d", &N);
	while ((fscanf(topologyFile, "%d %d %d", &from, &to, &weight)) != -1) {
		topology[from].push_back(std::make_pair(to, weight));
		topology[to].push_back(std::make_pair(from, weight));
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
		dijkstra(topology, N, i);
		chooseDirection(N, i);
		tieBreak(topology, N, i);
	}

#if PRINTTOFILE != 1
	printShortestPath(stdout, N);
	printMessage(stdout, topology, messageNode, messages, N, M);
#else
	printShortestPath(outputFile, N);
	printMessage(outputFile, topology, messageNode, messages, N, M);
#endif

	
	
	while ((fscanf(changesFile, "%d %d %d", &from, &to, &weight)) != -1) {
		int idx = find(topology, from, to, weight);

		if (idx == -1) {
			if (weight != -999) {
				topology[from].push_back(std::make_pair(to, weight));
				topology[to].push_back(std::make_pair(from, weight));
			}
		}
		else {
			std::vector<std::pair<int, int> >::iterator it = topology[from].begin() + idx;
			if (weight == -999) {
				topology[from].erase(it);
				idx = find(topology, to, from, weight);
				it = topology[to].begin() + idx;
				topology[to].erase(it);
			}
			else {
				it->second = weight;
				idx = find(topology, to, from, weight);
				it = topology[to].begin() + idx;
				it->second = weight;
			}
		}

		for (i = 0; i < N; i++) {
			dijkstra(topology, N, i);
			chooseDirection(N, i);
			tieBreak(topology, N, i);
		}
#if PRINTTOFILE != 1
		printShortestPath(stdout, N);
		printMessage(stdout, topology, messageNode, messages, N, M);
#else
		printShortestPath(outputFile, N);
		printMessage(outputFile, topology, messageNode, messages, N, M);
#endif
	}

	std::cout << "Complete. Output file written to output_ls.txt\n";
}
