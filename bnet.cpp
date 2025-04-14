#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <unordered_map>
#include <string>
#include <stdexcept>
#include <iomanip>
#include <algorithm>
#include <cctype>

using namespace std;
using sxn_Record = vector<int>;                     
using sxn_Dataset = vector<sxn_Record>;             
using sxn_ColumnMap = unordered_map<string, int>;   

sxn_Dataset sxn_dataset;
int sxn_totalRecords = 0;
sxn_ColumnMap sxn_columnMap = {{"B", 0}, {"G", 1}, {"C", 2}, {"F", 3}};

vector<string> sxn_split(const string &str) {
    vector<string> tokens;
    istringstream stream(str);
    string token;
    while (stream >> token) {
        tokens.push_back(token);
    }
    return tokens;
}

string sxn_trim(const string &str) {
    size_t start = str.find_first_not_of(" \t");
    size_t end = str.find_last_not_of(" \t");
    return (start == string::npos || end == string::npos) ? "" : str.substr(start, end - start + 1);
}

double sxn_calculateProbability(const vector<pair<string, char>> &conditions) {
    try {
        int matchCount = 0;
        for (const auto &record : sxn_dataset) {
            bool match = true;
            for (const auto &condition : conditions) {
                int colIndex = sxn_columnMap.at(condition.first);
                if ((condition.second == 'T' && record[colIndex] != 1) ||
                    (condition.second == 'F' && record[colIndex] != 0)) {
                    match = false;
                    break;
                }
            }
            if (match) {
                ++matchCount;
            }
        }
        return static_cast<double>(matchCount) / sxn_totalRecords;
    } catch (const out_of_range &e) {
        throw invalid_argument("Invalid variable in conditions: " + string(e.what()));
    }
}

string sxn_toUpperCase(const string &str) {
    string result = str;
    transform(result.begin(), result.end(), result.begin(), ::toupper);
    return result;
}

void sxn_validateQuery(const string &query) {
    vector<string> validVariables = {"B", "G", "C", "F"};
    vector<string> parts = sxn_split(query);

    if (parts.empty()) {
        throw invalid_argument("Query must contain at least one variable.");
    }

    for (auto &part : parts) {
        for (char &c : part) {
            c = toupper(c);
        }
    }

    unordered_map<string, int> seenQueryVariables;
    unordered_map<string, int> seenEvidenceVariables;
    bool isEvidence = false;

    for (size_t i = 0; i < parts.size(); ++i) {
        string var = parts[i];

        if (var == "GIVEN") {
            if (isEvidence) {
                throw invalid_argument("Multiple 'GIVEN' keywords are not allowed.");
            }
            isEvidence = true;
            continue;
        }

        if (var.size() != 2 || 
            find(validVariables.begin(), validVariables.end(), string(1, var[0])) == validVariables.end() || 
            (var[1] != 'T' && var[1] != 'F')) {
            throw invalid_argument("Invalid query format: " + var + ". Use variables like Bt, Gf, etc.");
        }

        if (isEvidence) {
            if (++seenEvidenceVariables[var] > 1) {
                throw invalid_argument("Duplicate variables detected in evidence: " + var + ".");
            }
        } else {
            if (++seenQueryVariables[var] > 1) {
                throw invalid_argument("Duplicate variables detected in query: " + var + ".");
            }
        }
    }

    auto givenPosition = find(parts.begin(), parts.end(), "GIVEN");
    if (givenPosition != parts.end()) {
        if (givenPosition == parts.begin()) {
            throw invalid_argument("Query variables must be provided before 'GIVEN'.");
        }
        if (givenPosition + 1 == parts.end()) {
            throw invalid_argument("Evidence variables must be provided after 'GIVEN'.");
        }
    }
}

double sxn_computeConditionalProbability(const vector<pair<string, char>> &queryVars,
                                         const vector<pair<string, char>> &evidenceVars = {}) {
    try {
        vector<sxn_Record> filteredDataset;
        int queryAndEvidenceMatchCount = 0;

        for (const auto &record : sxn_dataset) {
            bool matchesEvidence = true;

            for (const auto &condition : evidenceVars) {
                int colIndex = sxn_columnMap.at(condition.first);
                if ((condition.second == 'T' && record[colIndex] != 1) ||
                    (condition.second == 'F' && record[colIndex] != 0)) {
                    matchesEvidence = false;
                    break;
                }
            }

            if (matchesEvidence) {
                filteredDataset.push_back(record);
            }
        }

        for (const auto &record : filteredDataset) {
            bool matchesQuery = true;

            for (const auto &condition : queryVars) {
                int colIndex = sxn_columnMap.at(condition.first);
                if ((condition.second == 'T' && record[colIndex] != 1) ||
                    (condition.second == 'F' && record[colIndex] != 0)) {
                    matchesQuery = false;
                    break;
                }
            }

            if (matchesQuery) {
                ++queryAndEvidenceMatchCount;
            }
        }

        int evidenceMatchCount = filteredDataset.size();
        if (evidenceMatchCount == 0) {
            throw runtime_error("No matching records for evidence variables.");
        }

        double probability = static_cast<double>(queryAndEvidenceMatchCount) / evidenceMatchCount;
        return probability;
    } catch (const out_of_range &e) {
        throw invalid_argument("Invalid variable in conditions: " + string(e.what()));
    }
}

void sxn_processQuery(const string &query) {
    try {
        sxn_validateQuery(query);

        vector<string> parts = sxn_split(sxn_toUpperCase(query));
        vector<pair<string, char>> queryVariables, evidenceVariables;
        bool isEvidence = false;

        for (const auto &part : parts) {
            if (part == "GIVEN") {
                isEvidence = true;
                continue;
            }
            if (isEvidence) {
                evidenceVariables.emplace_back(string(1, part[0]), part[1]);
            } else {
                queryVariables.emplace_back(string(1, part[0]), part[1]);
            }
        }

        double probability = sxn_computeConditionalProbability(queryVariables, evidenceVariables);
        cout << "Probability: " << fixed << setprecision(9) << probability << endl;
    } catch (const invalid_argument &e) {
        cout << "Error: " << e.what() << endl;
    } catch (const exception &e) {
        cout << "Unexpected error: " << e.what() << endl;
    }
}

int main(int argc, char *argv[]) {
    if (argc != 2) {
        cerr << "Usage: " << argv[0] << " <training_data_file>" << endl;
        return 1;
    }

    string fileName = argv[1];
    ifstream file(fileName);
    if (!file.is_open()) {
        cerr << "Error: Unable to open file " << fileName << endl;
        return 1;
    }

    string line;
    while (getline(file, line)) {
        line = sxn_trim(line);
        vector<string> tokens = sxn_split(line);
        tokens.erase(remove_if(tokens.begin(), tokens.end(), [](const string &s) { return s.empty(); }), tokens.end());

        try {
            if (tokens.size() != sxn_columnMap.size()) {
                throw invalid_argument("Incorrect number of values in line: " + line);
            }

            sxn_Record record;
            for (const auto &token : tokens) {
                if (!all_of(token.begin(), token.end(), ::isdigit)) {
                    throw invalid_argument("Non-numeric value found: " + token);
                }
                record.push_back(stoi(token));
            }

            sxn_dataset.push_back(record);
        } catch (const invalid_argument &e) {
            cerr << "Error in input file: " << line << ". Error: " << e.what() << endl;
            return 1;
        }
    }

    sxn_totalRecords = sxn_dataset.size();

    while (true) {
        cout << "Enter query (or type 'none' to exit): ";
        string query;
        getline(cin, query);

        if (query == "none") {
            break;
        }

        sxn_processQuery(query);
    }

    return 0;
}
