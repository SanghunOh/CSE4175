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

unsigned int to_char(char* generator) {
	int len = strlen(generator);

	unsigned int ret = 0;
	int mul = 1;
	for(int i = 0; i < len; i++){
		ret += mul * (generator[len - 1 - i] - '0');
		mul *= 2;
	}

	return ret;
}

char get_bit(unsigned int src, char bit) {
	return (src & (1 << bit)) >> bit;
}

int get_remainder(unsigned int data, unsigned int generator, unsigned int gen_size, int bit_4) {
	unsigned char bit = 15;
	unsigned int _gen = generator << (16 - gen_size);
	unsigned int limit = 1 << (gen_size - 1);
	while (bit >= 0) {
		if(data < limit)
			break;

		char a = get_bit(data, bit);
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
	if(!word) {
		strcpy(str, "00000000");
		str[8] = '\0';

		return;
	}
	while (bits > 0) {
		str[8 - bits] = (get_bit(word, bits - 1) + '0');
		bits--;
	}
	str[8] = '\0';

	return;
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
	unsigned int generator;
	unsigned int generator_size;
	unsigned int dataword_size;
	unsigned int codeword_size;
	std::ifstream input_file;
	std::ofstream output_file;
	char byte_codeword[9];
	int codeword_per_char;
	
	if (argc != 5) {
		print_error("age: ./crc_encoder input_file output_file result_file generator dataword_size");

		exit(0);
	}

	generator_size = strlen(argv[3]);
	generator = to_char(argv[3]);

	inputfile_name = argv[1];
	outputfile_name = argv[2];
	dataword_size = atoi(argv[4]);
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

	if (dataword_size != 4 && dataword_size != 8) {
		print_error("dataword size must be 4 or 8");
		
		exit(0);
	}

	if (dataword_size == 4){
		codeword_per_char = 4 + generator_size - 1;
		codeword_per_char *= 2;
		codeword_per_char = (codeword_per_char - 1) / 8;
		codeword_per_char += 1;
	}
	else {
		codeword_per_char = 8 + generator_size - 1;
		codeword_per_char = (codeword_per_char - 1) / 8;
		codeword_per_char += 1;
	}

	int padding_bits = calculate_padding_bits(generator_size, dataword_size);
	get_bytestring(byte_codeword, padding_bits);
	output_file << padding_bits;

	char c;
	char cnext;
//	input_file.get(c);
	while (!input_file.eof()) {	// read one charactor from file
		std::vector<unsigned char> bytes;
		unsigned int codeword;

		input_file.get(c);

		if(input_file.eof())
			break;

		if (dataword_size == 4){	// data word size: 4
			int upper = c / 16;
			int lower = c % 16;

			// augment upper
			upper = upper << (generator_size - 1);
			// augment lower;
			lower = lower << (generator_size - 1);
			unsigned int upper_rem = get_remainder(upper, generator, generator_size, 1);
			unsigned int lower_rem = get_remainder(lower, generator, generator_size, 1);

			upper += upper_rem;
			lower += lower_rem;

			codeword = (upper << codeword_size) + lower;	
		}
		else if (dataword_size == 8) {	// data word size: 8
			unsigned int data = c;

			data = data << (generator_size - 1);

			int rem = get_remainder(data, generator, generator_size, 0);


			codeword = data + rem;
		}

		for (int i = 0; i < codeword_per_char; i++){
			if (codeword % 256 > 0) {
				bytes.push_back(codeword % 256);
			}
			else if (codeword % 256 == 0)
				bytes.push_back(0);
			codeword /= 256;
		}

		std::reverse(bytes.begin(), bytes.end());

		for(int i = 0; i < bytes.size(); i++){
			output_file << (unsigned char)bytes[i];
		}
		c = cnext;
	}

	input_file.close();
	output_file.close();
}

