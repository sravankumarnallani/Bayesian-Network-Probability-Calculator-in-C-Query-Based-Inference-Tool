---

**Name and UTA ID:**  


---

**Programming Language Used:**  
C++  
Version: GCC 11.3.0  

---

**Code Structure:**  

1. **Main Program File:**  
   - `bnet.cpp`  
     Contains the implementation of the probability calculation and query processing logic.

2. **Functions:**  
   - `sxn_split`: Splits a string into tokens based on spaces.  
   - `sxn_trim`: Trims leading and trailing whitespace from a string.  
   - `sxn_calculateProbability`: Calculates the unconditional probability for given conditions.  
   - `sxn_toUpperCase`: Converts a string to uppercase.  
   - `sxn_validateQuery`: Validates the input query format.  
   - `sxn_computeConditionalProbability`: Computes the conditional probability given query and evidence variables.  
   - `sxn_processQuery`: Processes a query and calculates the result.  

3. **Global Variables:**  
   - `sxn_Dataset`: Stores the dataset loaded from the input file.  
   - `sxn_columnMap`: Maps variable names (e.g., B, G, C, F) to their column indices.  
   - `sxn_totalRecords`: Stores the total number of records in the dataset.  

---

**How to Run the Code:**  

1. **Compilation Instructions:**  
   - Make sure you have GCC installed (version 11.3.0 or compatible).  
   - Use the following command to compile the code:  
     ```bash
     g++ -o probability_calculator bnet.cpp -std=c++17
     ```
   - This will generate an executable file named `probability_calculator`.

2. **Execution Instructions:**  
   - Run the executable by providing the path to the training data file as an argument:  
     ```bash
     ./probability_calculator <training_data_file>
     ```
     Example:  
     ```bash
     ./probability_calculator training_data.txt
     ```
   - After execution, you can enter queries in the format:  
     ```
     Bt Ct GIVEN Gf
     ```
     - `Bt` means variable `B` is `True`.  
     - `Ct` means variable `C` is `True`.  
     - `GIVEN Gf` means calculate the conditional probability of `Bt` and `Ct` given `G` is `False`.  
   - To exit, type:  
     ```
     none
     ```

3. **Input File Format:**  
   - Each line of the input file represents a record in the dataset.  
   - Values must be integers (`0` for `False`, `1` for `True`).  
   - Columns correspond to variables `B`, `G`, `C`, `F` in that order.  
   - Example:  
     ```
     1 0 1 1
     0 1 0 0
     ```

---

**Running on ACS Omega (Optional):**  
- This code is compatible with ACS Omega if GCC 11.3.0 or a compatible version is available.  
- Ensure to compile with `g++` as specified above and run the executable with the provided instructions.

---
