#include <iostream>
#include <fstream>
#include <sstream>
#include <map>
#include <string>
#include <vector>
#include <random>
#include <algorithm>
#include <cctype> // For isdigit, isupper, islower
#include <unordered_set>
#include <ctime>
#include <climits>

std::string analyzePasswordStructure(const std::string& password);
std::string findLeastCommonStructure(const std::string& currentStructure, std::vector<std::pair<std::string, int>>& sortedStructures);
bool canTransformWithThreeChanges(const std::string& from, const std::string& to);
struct PCFGStructure {
    std::map<std::string, int> structureFrequencies;
    std::map<char, std::vector<int>> charFrequencies;
};

// Generate random lowercase character
char randomLowercase() {
    static std::mt19937 generator(std::time(nullptr)); 
    static std::uniform_int_distribution<int> distribution('a', 'z');
    return static_cast<char>(distribution(generator));
}

// Generate a random uppercase letter
char randomUppercase() {
    static std::mt19937 generator(std::time(nullptr)); 
    static std::uniform_int_distribution<int> distribution('A', 'Z');
    return static_cast<char>(distribution(generator));
}

// Generate a random digit
char randomDigit() {
    static std::mt19937 generator(std::time(nullptr)); 
    static std::uniform_int_distribution<int> distribution('0', '9');
    return static_cast<char>(distribution(generator));
}

// Generate a random symbol
char randomSymbol() {
    static std::mt19937 generator(std::time(nullptr));
    static std::string symbols = "`~!@#$%^&*()-_=+[{]}\\|;:'\",<.>/? ";
    static std::uniform_int_distribution<int> distribution(0, symbols.size() - 1);
    return symbols[distribution(generator)];
}

std::string strengthenPassword(const std::string& password, 
                               const PCFGStructure& pcfgData,
                               std::vector<std::pair<std::string, int>>& sortedStructures){
    std::string strengthened = password;
    std::string currentStructure = analyzePasswordStructure(strengthened);
    std::string desiredStructure = findLeastCommonStructure(currentStructure, sortedStructures);

    std::mt19937 generator(std::time(nullptr)); 
    int changes = 0;

    auto findLessCommonChar = [&](char type, char currentChar, size_t index) -> char {
        std::vector<std::pair<char, int>> candidates;
        for (const auto& pair : pcfgData.charFrequencies) {
            if ((type == 'L' && islower(pair.first)) ||
                (type == 'U' && isupper(pair.first)) ||
                (type == 'D' && isdigit(pair.first)) ||
                (type == 'S' && ispunct(pair.first)) ||
                (type == 'A' && pair.first != currentChar)) {  // 'A' for any character type
                candidates.emplace_back(pair.first, pair.second[index]);
            }
        }

        if (!candidates.empty()) {
            std::sort(candidates.begin(), candidates.end(),
                      [](const std::pair<char, int>& a, const std::pair<char, int>& b) {
                          return a.second < b.second;
                      });
            for (const auto& candidate : candidates) {
                if (candidate.first != currentChar) {
                    return candidate.first;
                }
            }
        }
        return currentChar;  // Return the original character if no alternative is found
    };

    for (size_t i = 0; i < strengthened.length() && changes < 3; ++i) {
        char currentChar = strengthened[i];
        char desiredCharType = desiredStructure[i];

        if (desiredCharType != currentStructure[i]) {  // Structural change
            char replacement = findLessCommonChar(desiredCharType, currentChar, i);
            if (replacement != currentChar) {
                strengthened[i] = replacement;
                changes++;
            }
        } else if (changes < 3) {  // Frequency-based change
            char replacement = findLessCommonChar('A', currentChar, i);  // 'A' for any type
            if (replacement != currentChar) {
                strengthened[i] = replacement;
                changes++;
            }
        }
    }

    return strengthened;
}

// Function to parse the PCFG output
PCFGStructure parsePCFGOutput(const std::string& filename) {
    PCFGStructure pcfgData;
    std::ifstream file(filename);
    std::string line;
    bool structureDataStarted = false;

    while (getline(file, line)) {
        // Check if the line is a marker or a line indicating the start of sentence structures
        if (line.find("abcdefghijklmnopqrstuvwxyz") != std::string::npos ||
            line.find("0123456789") != std::string::npos ||
            line.find("`~!@#$%^&*()-_=+[{]}\\|;:'\",<.>/? ") != std::string::npos) {
            continue; // Skip marker lines
        }

        if (!structureDataStarted) {
            if (line.find("sentence structures") != std::string::npos) {
                structureDataStarted = true;
                continue;
            }

            std::istringstream iss(line);
            char ch;
            int freq;
            if (iss >> ch) {
                std::vector<int> frequencies;
                while (iss >> freq) {
                    frequencies.push_back(freq);
                }
                pcfgData.charFrequencies[ch] = frequencies;
            }
        } else {
            // Processing sentence structures
            std::istringstream iss(line);
            std::string structure;
            int frequency;
            if (iss >> frequency >> structure) {
                // Expand the structure name inside the loop
                std::string expanded;
                for (size_t i = 0; i < structure.size(); ++i) {
                    if (std::isalpha(structure[i]) && i + 1 < structure.size() && std::isdigit(structure[i + 1])) {
                        int count = structure[i + 1] - '0';
                        expanded.append(count, structure[i]);
                        ++i; // Skip the digit
                    }
                }
                pcfgData.structureFrequencies[expanded] = frequency;
            }
        }
    }

    return pcfgData;
}

// Function to process the file and strengthen each password
std::vector<std::string> processFile(const std::string &filePath, const PCFGStructure& pcfgData, std::vector<std::pair<std::string, int>>& sortedStructures) {
    std::vector<std::string> strengthenedPasswords;
    std::ifstream file(filePath);
    std::string line;
    int count = 0;
    int percent = 0;

    while (std::getline(file, line)) {
        strengthenedPasswords.push_back(strengthenPassword(line, pcfgData, sortedStructures));
        count++;
        if(count == 35000){
            count = 0;
            std::cout << percent++ <<  '%' << std::endl;
        }
    }

    file.close();
    return strengthenedPasswords;
}

std::string analyzePasswordStructure(const std::string& password) {
    std::string structure;
    char lastCharType = '\0';

    for (char c : password) {
        char currentCharType;
        if (std::isupper(c)) {
            currentCharType = 'U';
        } else if (std::islower(c)) {
            currentCharType = 'L';
        } else if (std::isdigit(c)) {
            currentCharType = 'D';
        } else {
            currentCharType = 'S';
        }

        structure += currentCharType;
    }

    return structure;
}

std::vector<std::string> generatePotentialStructures(const std::string& currentStructure) {
    std::vector<std::string> potentialStructures;
    std::string types = "ULSD"; // Uppercase, Lowercase, Symbol, Digit

    // Limit the number of generated structures
    for (int i = 0; i < std::min(static_cast<int>(currentStructure.length()), 3); ++i) {
        for (char type : types) {
            std::string newStructure = currentStructure;
            newStructure[i] = type;
            potentialStructures.push_back(newStructure);
        }
    }
    return potentialStructures;
}

std::string findLeastCommonStructure(const std::string& currentStructure, std::vector<std::pair<std::string, int>>& sortedStructures) {
    // Check existing structures
    for (auto& pair : sortedStructures) {
        if (canTransformWithThreeChanges(currentStructure, pair.first) && pair.second <= 1) {
            //pair.second++; // Increment the frequency
            return pair.first; // Return the first transformable structure
        }
    }

    // Check generated potential structures
    auto potentialStructures = generatePotentialStructures(currentStructure);
    for (const auto& structure : potentialStructures) {
        auto it = std::find_if(sortedStructures.begin(), sortedStructures.end(), [&structure](const std::pair<std::string, int>& p) { return p.first == structure; });
        if (it == sortedStructures.end() && canTransformWithThreeChanges(currentStructure, structure)) {
            sortedStructures.push_back(std::make_pair(structure, 0)); // Add the non-existent structure with frequency 1
            return structure; // Return the non-existent structure
        }
    }

    return currentStructure; // Return the original structure if no better match is found
}


bool canTransformWithThreeChanges(const std::string& from, const std::string& to) {
    if (from.length() != to.length()) return false; // Ensure equal lengths

    int changes = 0;
    for (size_t i = 0; i < from.length(); ++i) {
        if (from[i] != to[i]) {
            ++changes;
        }
        if (changes > 3) {
            return false;
        }
    }
    return changes <= 3;
}

int main(int argc, char *argv[]) {
    if (argc != 2) {
        std::cerr << "Usage: " << argv[0] << " <file>\n";
        return 1;
    }

    PCFGStructure pcfgData = parsePCFGOutput("rockyou.txt.6.4.a.pcfg1");
    
    std::vector<std::pair<std::string, int>> sortedStructures;
    for (const auto& kv : pcfgData.structureFrequencies) {
        sortedStructures.push_back(kv);
    }

    // Sort the vector
    std::sort(sortedStructures.begin(), sortedStructures.end(),
              [](const std::pair<std::string, int>& a, const std::pair<std::string, int>& b) {
                  return a.second < b.second;  // Sort in ascending order of frequency
              });

    // Print the sorted structures and their frequencies
    // for (const auto& pair : sortedStructures) {
    //     std::cout << pair.first << " : " << pair.second << std::endl;
    // }

    std::string filePath = argv[1];
    auto strengthenedPasswords = processFile(filePath, pcfgData, sortedStructures);

    std::string outputFile = filePath + "_strengthened";
    std::ofstream outFile(outputFile);
    for (const std::string &password : strengthenedPasswords) {
        outFile << password << std::endl;
    }
    outFile.close();

    std::cout << "Passwords strengthened" << std::endl;

    return 0;
}