#include "mcs.h"

constexpr inline static size_t binom(size_t n, size_t k) noexcept
{
	if (k > n)
		return 0; // out of range
	if (k == 0 || k == n)
		return 1; // edge

	return (binom(n - 1, k - 1) * n) / k;
}

// Generates forms of ones and zeros in a binary sequence
// with maximal length.
// Every combination begins and ends with 1.
//
// matches: Number of ones in the binary sequence
//
// Returns: Vector of Forms objects representing all Forms
std::vector<Form> Form::generateAllForms(uint64_t length, uint64_t mismatchK)
{
	std::vector<Form> allForms;
	uint64_t ones = 2;
	if (ones > length - mismatchK)
		ones = length - mismatchK;
	if (ones < 2)
	{ 
		throw std::runtime_error("Matches per form must be equal or greater than 2!");
		exit(1);
	}
		
	uint64_t max_zeros = length - ones;


	allForms.reserve(binom(length - 1, max_zeros));
	// Recursive function to generate forms
	std::function<void(uint64_t, uint64_t, kMismatchIntegerType::uint_type)> generateForms;
	generateForms = [&allForms, &generateForms](uint64_t remainOnes, uint64_t remainZeros, kMismatchIntegerType::uint_type curFormInt) 
	{
		if (!remainOnes)
			allForms.push_back(Form((curFormInt << 1) | 1));
		if (remainOnes)
			generateForms(remainOnes - 1, remainZeros, (curFormInt << 1) | 1);
		if (remainZeros)
			generateForms(remainOnes, remainZeros - 1, curFormInt << 1);
	};

	generateForms(ones - 2, max_zeros, 1);

	return allForms;
}


std::string Form::getStringFromPosition(const std::string& str, size_t pos) const
{
	kMismatchIntegerType::uint_type formIntSeq = this->sequenceInt;
	kMismatchIntegerType::uint_type one = static_cast<kMismatchIntegerType::uint_type>(1);
	std::string result(this->getSize(), '_');
	for (size_t i = 0; i < this->getSize(); ++i, ++pos)
	{
		if (formIntSeq & one)
			result[i] = str[pos];  // Copy character from original string
		formIntSeq >>= 1;
	}

	return result;
}



bool Combination::contains(const Form& form) const
{
	kMismatchIntegerType::uint_type formInt = form.sequenceInt;
	kMismatchIntegerType::uint_type combinationInt = this->sequenceInt;

	//Run until the MSB of integer type is 1
	while (!((static_cast<kMismatchIntegerType::uint_type>(1) << (kMismatchIntegerType::UINT_TYPE_SIZE - 1)) & formInt))
	{
		/*
			First is a combination, second is a forms
			(0, 0) -> 1
			(0, 1) -> 0
			(1, 0) -> 1
			(1, 1) -> 1
			NOT(NOT(combination) AND form) = combination OR NOT(form)
			we should check if we are getting all ones
		*/
		if ((combinationInt | ~formInt) == ~static_cast<kMismatchIntegerType::uint_type>(0))
			return true;
		formInt <<= 1;
	}

	//Last check for MSB is 1
	if ((combinationInt | ~formInt) == ~static_cast<kMismatchIntegerType::uint_type>(0))
		return true;

	return false;
}

kMismatchIntegerType::uint_type cutRightZeros(kMismatchIntegerType::uint_type n)
{
	while (!(n & 1))
		n >>= 1;
	return n;
}

std::set<Form> Combination::getAllForms(uint64_t matches) const
{
	std::set<Form> allInnerForms;
	kMismatchIntegerType::uint_type combInt = this->sequenceInt;

	// Recursive function to generate combinations
	std::function<void(uint64_t, kMismatchIntegerType::uint_type, kMismatchIntegerType::uint_type)> generateInnerForms;
	generateInnerForms = [&allInnerForms, &generateInnerForms, &combInt]
	(uint64_t remainOnes, kMismatchIntegerType::uint_type curPosition, kMismatchIntegerType::uint_type curFormInt)
		{
			if (!remainOnes) {
				allInnerForms.insert(Form(cutRightZeros(curFormInt)));
				return;
			}

			while (!((static_cast<uint64_t>(1) << (kMismatchIntegerType::UINT_TYPE_SIZE - 1)) & curPosition))
			{
				if (combInt & curPosition)
					generateInnerForms(remainOnes - 1, curPosition << 1, curFormInt | curPosition);
				curPosition <<= 1;
			}

			if ((combInt & curPosition) && (remainOnes == 1))
				allInnerForms.insert(Form(cutRightZeros(curFormInt | curPosition)));

		};

	generateInnerForms(matches, 1, 0);

	return allInnerForms;
}

// Generates combinations of ones and zeros in a binary sequence
// with a fixed length and a specific number of zeros (mismatchK).
// Every combination begins with 1.
//
// length: Length of the binary sequence
// mismatchK: Number of zeros in the binary sequence
//
// Returns: Vector of Combination objects representing all combinations
std::vector<Combination> Combination::generateAllCombinations(uint64_t length, uint64_t mismatchK)
{
	std::vector<Combination> allCombinations;
	uint64_t zeros = mismatchK;
	uint64_t ones = length - mismatchK;

	allCombinations.reserve(binom(length - 1, zeros));

	// Recursive function to generate combinations
	std::function<void(uint64_t, uint64_t, kMismatchIntegerType::uint_type)> generateCombinations;
	generateCombinations = [&allCombinations, &generateCombinations]
	(uint64_t remainOnes, uint64_t remainZeros, kMismatchIntegerType::uint_type curCombinationInt) {
		if (!remainOnes && !remainZeros)
			allCombinations.push_back(Combination(curCombinationInt));
		if (remainOnes)
			generateCombinations(remainOnes - 1, remainZeros, (curCombinationInt << 1) | 1);
		if (remainZeros)
			generateCombinations(remainOnes, remainZeros - 1, curCombinationInt << 1);
		};

	generateCombinations(ones - 1, zeros, 1);

	return allCombinations;
}



const std::vector<Form>& MCS::getMcsForms() const
{
	return this->mcsForms;
}

MCS MCS::buildMCSNaiveMultithreaded(std::vector<std::string>& queries, uint64_t mismatchK)
{
	MCS resultMCS;
	uint64_t length = 1;
	for (auto& query : queries)
		length = query.length() > length ? query.length() : length;
	if (mismatchK > length)
	{
		throw std::runtime_error("Mismatch number can not be greater than query length!");
		exit(1);
	}

	auto combinations = Combination::generateAllCombinations(length, mismatchK);
	auto forms = Form::generateAllForms(length, mismatchK);
	while (!combinations.empty())
	{
		//Calcualate the form that contributes for the maximal number of combinations
		auto bestFormToCombinationNumberPair = std::transform_reduce(
			std::execution::par,  // Use a parallel execution policy
			forms.begin(), forms.end(),  // Input range
			std::pair<Form, uint64_t>{forms.front(), 0},  // Initial value

			// Binary operation for reduction
			// It takes two pairs of <Form, uint64_t> as input and returns the one with the greater count.
			[](const std::pair<Form, uint64_t>& a, const std::pair<Form, uint64_t>& b) {
				if (a.second == b.second)
					return a.first < b.first ? a : b;
				return a.second > b.second ? a : b;
			},

			// Unary operation for transformation
			// It takes a Form as input and calculates the number of combinations containing that form.
			// Returns a pair of <Form, uint64_t>, where the uint64_t value represents the count of combinations containing the form.
			[&combinations](const Form& form) {
				uint64_t curFormCombinationNumber = 0;
				// Iterate over each combination
				for (const Combination& combination : combinations) {
					if (combination.contains(form)) {
						curFormCombinationNumber++;
					}
				}
				// Return a pair containing the form and the count of combinations containing it
				return std::pair<Form, uint64_t>{form, curFormCombinationNumber};
			});


		//Adding the best form the the MCS
		resultMCS.mcsForms.push_back(bestFormToCombinationNumberPair.first);

		//Removing the combinations, containing the best form form
		auto newEndIt = std::remove_if(combinations.begin(), combinations.end(),
			[&bestFormToCombinationNumberPair](const Combination& combination) {
				return combination.contains(bestFormToCombinationNumberPair.first);
			});
		combinations.erase(newEndIt, combinations.end());

	}

	//std::sort(resultMCS.mcsForms.begin(), resultMCS.mcsForms.end());

	return resultMCS;
}

const void MCS::saveToFile(std::string fileName) const
{
	std::ofstream file(fileName);
	if (!file) {
		throw std::runtime_error("Unable to open file: " + fileName);
	}
	for (Form form : this->mcsForms)
	{
		file << form << std::endl;
	}
	file.close();
}
#pragma GCC optimize("O1")
MCS MCS::loadFromFile(std::string fileName)
{
	MCS resultMCS;
	std::ifstream file(fileName);
	if (!file) {
		throw std::runtime_error("Unable to open file: " + fileName);
	}
	std::vector<std::string> queries;
	std::string line;
	while (std::getline(file, line)) 
	{
		size_t num = 0;
		for (auto ch : line)
		{
			num <<= 1;
			if (ch != '0' && ch != '1')
				throw std::runtime_error("Wrong MCS file content. Unexpected character: " + ch);
			num |= std::stoi(&ch);
		}
		resultMCS.mcsForms.push_back(Form(num));
	}
	return resultMCS;
}
#pragma GCC optimize("O3")
MCS::MCS()
{
	this->mcsForms = std::vector<Form>();
}
