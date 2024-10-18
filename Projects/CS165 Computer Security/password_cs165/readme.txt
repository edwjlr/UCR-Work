Password Strengthener README
----------------------------

The Password Strengthener analyzes and strengthens passwords by using Markov chain-based training. The program analyzes the passwords structure and in under 3 changes tries to strengthen the password by converting it into a password structure that occurs less frequently. Below are the steps for splitting a large password file, training a model on the split data, compiling the Password Strengthening program, and running it to enhance password strength.

Setup to build and operate:

Step 1: Splitting the Password File
First, split the rockyou.txt password file into smaller chunks for efficient processing. 
./splitpwd 6 rockyou.txt


Step 2: Train the Model
Train the Markov model on the split data. This step generates a probabilistic model based on the password structure found in the dataset.
./markov 1 rockyou.txt.6.4.a


Step 3: Compile the Password Strengthening Program
g++ -o strength strengthen.cpp


Step 4: Run the Password Strengthening Program
./strength rockyou.txt.6.1.a


Step 5: Run the PCFGC program to estimate a guess number for a password.
/pcfgc 1 rockyou.txt.6.1.a_strengthened rockyou.txt.6.4.a.pcfg1  > crack_file


Step 6: Run pwdstats to output the guess number estimates for the strengthened passwords
pwdstats < crack-file
