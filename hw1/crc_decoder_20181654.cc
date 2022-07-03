#include <iostream>
#include <cstring>
#include <cstdlib>
#include <string>
#include <fstream>
#include <vector>
#include <algorithm>

void print_error(std::string msg){
	std::cout << msg << std::endl;
	return;
}

unsigned int to_char(char* word) {
	int len = strlen(word);

	unsigned int ret = 0;
	int mul = 1;
	for(int i = 0; i < len; i++){
		ret += mul * (word[len - 1 - i] - '0');
		mul *= 2;
	}

	return ret;
}

char get_bit(int src, char bit) {
	return (src & (1 << bit)) >> bit;
}

int get_remainder(unsigned int data, unsigned int generator, unsigned int gen_size, int bit_4) {
	unsigned char bit = 15;
	unsigned int _gen = generator << (16 - gen_size);
	unsigned int limit = 1 << (gen_size - 1);
	printf("%d\n", data);
	while (bit >= 0) {
		if(data < limit)
			break;

		if (get_bit(data, bit)) {
			data = data ^ _gen;
		}

		bit--;
		_gen /= 2;
	}

	return data;
}

void get_bytestring(char *str, int word) {
	int bits = 8;
	while (bits > 0) {
		str[8 - bits] = (get_bit(word, bits - 1) + '0');
		bits--;
	}
	str[8] = '\0';

	return;
}

unsigned int read_padding_bits(std::ifstream &inputfile) {
	char byte;

	inputfile >> byte;

	return  byte - '0';
}

int calculate_padding_bits(unsigned int gen_size, unsigned int dataword_size) {
	int codeword_size;
	if (dataword_size == 4) {
		codeword_size = 4 + gen_size - 1;
		codeword_size *= 2;
	}
	else {
		codeword_size = 8 + gen_size - 1;
	}
	int i = 0;
	int mul = 8;
	while(1) {
		if (codeword_size <= mul) {
			break;
		}
		mul += 8;
	}

	return mul - codeword_size;
}

int main(int argc, char* argv[]){
	char *inputfile_name;
	char *outputfile_name;
	char *resultfile_name;
	unsigned int generator;
	unsigned int generator_size;
	unsigned int dataword_size;
	unsigned int codeword_size;
	std::ifstream input_file;
	std::ofstream output_file;
	std::ofstream result_file;
	char byte_codeword[32];
	std::vector<unsigned int> bytes;
	int error_cnt = 0;
	int codeword_cnt = 0;
	
	if (argc != 6) {
		print_error("age: ./crc_encoder input_file output_file result_file generator dataword_size");

		exit(0);
	}

	generator_size = strlen(argv[4]);
	generator = to_char(argv[4]);

	inputfile_name = argv[1];
	outputfile_name = argv[2];
	resultfile_name = argv[3];
	dataword_size = atoi(argv[5]);
	codeword_size = dataword_size + generator_size - 1;

	input_file.open(inputfile_name);
	if (input_file.fail()) {
		print_error("input file open error");

		exit(0);
	}

	output_file.open(outputfile_name);
	if (output_file.fail()) {
		print_error("output file open error");

		exit(0);
	}
	
	result_file.open(resultfile_name);
	if (result_file.fail()) {
		print_error("result file open error");

		exit(0);
	}

	if (dataword_size != 4 && dataword_size != 8) {
		print_error("dataword size must be 4 or 8");
		
		exit(0);
	}

	int padding_bits;

	unsigned char c;
	unsigned char* next = (unsigned char*)malloc(sizeof(unsigned char));

	input_file.read((char*)next, 1);
	padding_bits = next[0] - '0';

	input_file.read((char*)next, 1);
	byte_codeword[8] = '\0';
	while (!input_file.eof()) {
	//	input_file >> cnext;
		if(input_file.eof())
			break;

		if (dataword_size == 4) {
			unsigned int overall_codeword;
			unsigned int codeword_upper;
			unsigned int codeword_lower;
			
			codeword_upper = next[0];
			input_file.read((char*)next, 1);
			codeword_lower = next[0];
			input_file.read((char*)next, 1);

			//printf("%u %u\n", codeword_upper, codeword_lower);

			overall_codeword = codeword_upper << 8;
			overall_codeword += codeword_lower;

			codeword_lower = overall_codeword % (1 << codeword_size);
			codeword_upper = overall_codeword / (1 << codeword_size);
			//printf("%d\n", overall_codeword); 
			if(codeword_upper / (1 << codeword_size) != 0){
				codeword_upper = codeword_upper % (1 << codeword_size);
				error_cnt++;
			}

//			printf("%d %d\n", codeword_upper, codeword_lower);
			unsigned int error_upper = get_remainder(codeword_upper, generator, generator_size, 1);
			unsigned int error_lower = get_remainder(codeword_lower, generator, generator_size, 1);

			if(error_upper)
				error_cnt++;
			if (error_lower)
				error_cnt++;
			codeword_cnt += 2;
			
			overall_codeword = codeword_upper >> (generator_size - 1) << 4;
			overall_codeword += codeword_lower >> (generator_size - 1);

			output_file << (unsigned char) overall_codeword;
		}
		else if (dataword_size == 8) {
			unsigned int overall_codeword;
			unsigned int codeword_upper;
			unsigned int codeword_lower;

			codeword_upper = next [0];
			input_file.read((char*)next, 1);
			codeword_lower = next[0];
			input_file.read((char*)next, 1);

			overall_codeword = codeword_upper << 8;
			overall_codeword += codeword_lower;
			
			codeword_lower = overall_codeword % (1 << codeword_size);
			codeword_upper = overall_codeword / (1 << codeword_size);

			if(codeword_upper / (1 << codeword_size) != 0) {
				codeword_upper = codeword_upper % (1 << codeword_size);
				error_cnt++;
			}
			unsigned int error = get_remainder(overall_codeword, generator, generator_size, 1);
			overall_codeword = overall_codeword >> (generator_size - 1);
			if (error != 0) {
				error_cnt++;
			}
			codeword_cnt += 2;

			output_file << (unsigned char)overall_codeword;
		}
	}
	
	result_file << codeword_cnt << " " << error_cnt;

	input_file.close();
	output_file.close();
	result_file.close();
}

